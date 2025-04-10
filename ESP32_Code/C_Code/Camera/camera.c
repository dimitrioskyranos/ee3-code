#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "driver/uart.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_http_server.h"
#include "nvs_flash.h"

#define UART_NUM           UART_NUM_1
#define BUF_SIZE           (2048)
#define QUEUE_SIZE         (10)
#define START_SIGNAL       "IMG_START"
#define END_SIGNAL         "IMG_END"
#define MAX_IMAGE_SIZE     (1024 * 1024)  // 1MB max

#define WIFI_SSID          "iPhonevanMaarten"
#define WIFI_PASS          "abcabcabc"

#define STATIC_IP          "172.20.10.4"
#define GATEWAY_IP         "172.20.10.1"
#define NETMASK_IP         "255.255.255.240"

static const char *TAG = "UART_HTTP";

// Event groups
static EventGroupHandle_t s_wifi_event_group;
const int WIFI_CONNECTED_BIT = BIT0;
const int IMAGE_READY_BIT    = BIT1;

static esp_netif_t *netif = NULL;
static int reconnect_attempts = 0;

// Image storage
static QueueHandle_t uart_queue;
static uint8_t *image_buffer = NULL;
static uint8_t *last_image_buffer = NULL;
static int image_size = 0;
static int last_image_size = 0;
static int received_size = 0;

static esp_err_t post_image_handler(httpd_req_t *req);
static esp_err_t get_image_handler(httpd_req_t *req);

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        reconnect_attempts++;
        ESP_LOGW(TAG, "Wi-Fi disconnected. Attempt #%d to reconnect...", reconnect_attempts);
        esp_wifi_connect();
        xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "Got IP Address: " IPSTR, IP2STR(&event->ip_info.ip));
        reconnect_attempts = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

static void wifi_init(void) {
    s_wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    netif = esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                                        &wifi_event_handler, NULL, NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS
        }
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

// HTTP handlers
static esp_err_t post_image_handler(httpd_req_t *req) {
    ESP_LOGI(TAG, "Received HTTP POST request at /image");

    if (last_image_buffer != NULL && last_image_size > 0) {
        httpd_resp_set_type(req, "image/jpeg");
        httpd_resp_send(req, (const char *)last_image_buffer, last_image_size);
    } else {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "No image found.");
    }
    return ESP_OK;
}

static esp_err_t get_image_handler(httpd_req_t *req) {
    httpd_resp_set_hdr(req, "Cache-Control", "no-store, no-cache, must-revalidate");

    ESP_LOGI(TAG, "Received HTTP GET request at /getimage");
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group, IMAGE_READY_BIT,
                                           pdFALSE, pdTRUE, pdMS_TO_TICKS(5000));
    if ((bits & IMAGE_READY_BIT) == 0) {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Image not ready.");
        return ESP_OK;
    }

    if (last_image_buffer != NULL && last_image_size > 0) {
        httpd_resp_set_type(req, "image/jpeg");
        httpd_resp_send(req, (const char *)last_image_buffer, last_image_size);
    } else {
        httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "No image found.");
    }
    return ESP_OK;
}

static void start_http_server(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t post_image_uri = {
            .uri = "/image",
            .method = HTTP_POST,
            .handler = post_image_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &post_image_uri);

        httpd_uri_t get_image_uri = {
            .uri = "/getimage",
            .method = HTTP_GET,
            .handler = get_image_handler,
            .user_ctx = NULL
        };
        httpd_register_uri_handler(server, &get_image_uri);
        ESP_LOGI(TAG, "HTTP server started successfully.");
    } else {
        ESP_LOGE(TAG, "Failed to start HTTP server!");
    }
}

// UART state machine
typedef enum {
    WAIT_FOR_START,
    READ_SIZE,
    READ_IMAGE
} recv_state_t;

static void uart_event_task(void *arg) {
    xEventGroupClearBits(s_wifi_event_group, IMAGE_READY_BIT);

    uart_event_t event;
    uint8_t *data = malloc(BUF_SIZE);
    static uint8_t temp_buffer[BUF_SIZE];
    static int temp_len = 0;
    recv_state_t state = WAIT_FOR_START;

    while (1) {
        if (xQueueReceive(uart_queue, &event, pdMS_TO_TICKS(50))) {
            if (event.type == UART_DATA) {
                int len = uart_read_bytes(UART_NUM, data, event.size, pdMS_TO_TICKS(50));
                if (len > 0) {
                    if (temp_len + len >= BUF_SIZE) {
                        ESP_LOGW(TAG, "Buffer overflow. Resetting...");
                        temp_len = 0;
                        state = WAIT_FOR_START;
                        continue;
                    }
                    memcpy(temp_buffer + temp_len, data, len);
                    temp_len += len;
                }
            }
        }

        switch (state) {
            case WAIT_FOR_START: {
                char *start_ptr = (char *)memmem(temp_buffer, temp_len, START_SIGNAL, strlen(START_SIGNAL));
                if (start_ptr != NULL) {
                    int header_len = start_ptr - (char *)temp_buffer + strlen(START_SIGNAL);
                    memmove(temp_buffer, temp_buffer + header_len, temp_len - header_len);
                    temp_len -= header_len;
                    state = READ_SIZE;
                }
                break;
            }

            case READ_SIZE: {
                if (temp_len >= 4) {
                    image_size = temp_buffer[0] |
                                 (temp_buffer[1] << 8) |
                                 (temp_buffer[2] << 16) |
                                 (temp_buffer[3] << 24);

                    if (image_size <= 0 || image_size > MAX_IMAGE_SIZE) {
                        ESP_LOGE(TAG, "Invalid image size: %d", image_size);
                        temp_len = 0;
                        state = WAIT_FOR_START;
                        break;
                    }

                    if (image_buffer) free(image_buffer);
                    image_buffer = malloc(image_size);
                    if (!image_buffer) {
                        ESP_LOGE(TAG, "Failed to allocate image buffer");
                        state = WAIT_FOR_START;
                        break;
                    }

                    memmove(temp_buffer, temp_buffer + 4, temp_len - 4);
                    temp_len -= 4;
                    received_size = 0;
                    state = READ_IMAGE;
                }
                break;
            }

            case READ_IMAGE: {
                int copy_len = image_size - received_size;
                if (copy_len > temp_len) copy_len = temp_len;

                memcpy(image_buffer + received_size, temp_buffer, copy_len);
                received_size += copy_len;
                memmove(temp_buffer, temp_buffer + copy_len, temp_len - copy_len);
                temp_len -= copy_len;

                if (received_size == image_size) {
                    ESP_LOGI(TAG, "✅ Image fully received (%d bytes)", image_size);
                    if (last_image_buffer) free(last_image_buffer);
                    last_image_buffer = image_buffer;
                    last_image_size = image_size;
                    xEventGroupSetBits(s_wifi_event_group, IMAGE_READY_BIT);
                    image_buffer = NULL;
                    received_size = 0;
                    image_size = 0;
                    state = WAIT_FOR_START;
                }
                break;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(20));  // Yield
    }

    free(data);
    vTaskDelete(NULL);
}

static void uart_init(void) {
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM, &uart_config);
    uart_set_pin(UART_NUM, 17, 16, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM, BUF_SIZE * 2, 0, QUEUE_SIZE, &uart_queue, 0);

    // Run UART task on Core 1
    xTaskCreatePinnedToCore(uart_event_task, "uart_event_task", 8192, NULL, 1, NULL, 1);
    ESP_LOGI(TAG, "UART initialized");
}

void app_main(void) {
    ESP_LOGI(TAG, "Starting application...");
    ESP_ERROR_CHECK(nvs_flash_init());
    wifi_init();
    ESP_LOGI(TAG, "Waiting for Wi-Fi...");
    xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT, pdFALSE, pdTRUE, portMAX_DELAY);
    start_http_server();
    uart_init();
}