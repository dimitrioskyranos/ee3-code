#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_http_server.h"
#include "driver/uart.h"
#include "driver/gpio.h"

// Wi-Fi credentials
#define WIFI_SSID "iPhone - Dimitris"
#define WIFI_PASS "123456789"

// Static IP configuration
#define STATIC_IP "172.20.10.3"
#define GATEWAY_IP "172.20.10.1"
#define NETMASK_IP "255.255.255.0"

// UART configuration
#define UART_NUM UART_NUM_1
#define TXD_PIN (GPIO_NUM_17)
#define RXD_PIN (GPIO_NUM_18)
#define BUF_SIZE 8192

// Event group for Wi-Fi connection
static EventGroupHandle_t s_wifi_event_group;
const int WIFI_CONNECTED_BIT = BIT0;

static const char *TAG = "main";

uint8_t imgBuffer[BUF_SIZE];
int imgSize = 0;
bool imageReady = false;

// HTTP server handle
httpd_handle_t server = NULL;

// Forward declaration of uart_event_task
static void uart_event_task(void *pvParameters);

// Wi-Fi event handler
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        esp_wifi_connect();
        xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

// Wi-Fi initialization with static IP configuration
static void wifi_init(void) {
    s_wifi_event_group = xEventGroupCreate();
    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;

    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, &instance_any_id);
    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, &instance_got_ip);

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
    esp_wifi_start();

    // Static IP configuration
    esp_netif_ip_info_t ip_info;
    ip_info.ip.addr = esp_ip4addr_aton(STATIC_IP);
    ip_info.gw.addr = esp_ip4addr_aton(GATEWAY_IP);
    ip_info.netmask.addr = esp_ip4addr_aton(NETMASK_IP);

    esp_netif_set_ip_info(esp_netif_get_handle_from_ifkey("WIFI_STA_DEF"), &ip_info);
}

// HTTP POST handler to receive image data
esp_err_t upload_image_post_handler(httpd_req_t *req) {
    if (req->content_len <= 0 || req->content_len > BUF_SIZE) {
        ESP_LOGE(TAG, "Invalid content length: %d", req->content_len);
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid content length");
        return ESP_FAIL;
    }

    uint8_t *img_data = malloc(req->content_len);
    if (img_data == NULL) {
        ESP_LOGE(TAG, "Failed to allocate memory for image data");
        return ESP_ERR_NO_MEM;
    }

    int received = httpd_req_recv(req, (char *)img_data, req->content_len);
    if (received <= 0) {
        ESP_LOGE(TAG, "Error reading POST data");
        free(img_data);
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to read data");
        return ESP_FAIL;
    }

    ESP_LOGI(TAG, "Received image data of size %d bytes", received);

    memcpy(imgBuffer, img_data, received);
    imgSize = received;
    imageReady = true;

    free(img_data);

    const char *resp = "Image received successfully!";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

// HTTP server initialization
static httpd_handle_t start_http_server(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    httpd_handle_t server = NULL;
    if (httpd_start(&server, &config) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start HTTP server");
        return NULL;
    }

    httpd_uri_t upload_uri = {
        .uri = "/upload",
        .method = HTTP_POST,
        .handler = upload_image_post_handler,
        .user_ctx = NULL};

    if (httpd_register_uri_handler(server, &upload_uri) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register URI handler");
        httpd_stop(server);
        return NULL;
    }

    return server;
}

// Wait for Wi-Fi connection and print the IP address
static void wait_for_wifi_connection(void) {
    ESP_LOGI(TAG, "Waiting for Wi-Fi connection...");
    // Wait for the Wi-Fi connection to be established
    xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT, pdTRUE, pdTRUE, portMAX_DELAY);

    // Once connected, print the IP address
    ESP_LOGI(TAG, "Connected to Wi-Fi! IP Address: %s", STATIC_IP);

    // Ensure the HTTP server is started only once
    if (server == NULL) {
        server = start_http_server();
        if (server == NULL) {
            ESP_LOGE(TAG, "HTTP server initialization failed");
            return;
        }

        ESP_LOGI(TAG, "HTTP server started at http://%s/upload", STATIC_IP);
    }
}

void app_main(void) {
    ESP_ERROR_CHECK(nvs_flash_init());
    wifi_init();

    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE};
    uart_param_config(UART_NUM, &uart_config);
    uart_set_pin(UART_NUM, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM, BUF_SIZE * 2, 0, 0, NULL, 0);

    // Wait for Wi-Fi connection and print the IP address
    wait_for_wifi_connection();

    // Start UART event task
    xTaskCreate(uart_event_task, "uart_event_task", 4096, NULL, 5, NULL);
}

// UART Event Task
static void uart_event_task(void *pvParameters) {
    uint8_t data[BUF_SIZE];
    while (1) {
        int len = uart_read_bytes(UART_NUM, data, BUF_SIZE, pdMS_TO_TICKS(20));
        if (len > 0) {
            if ((imgSize + len) < BUF_SIZE) {
                memcpy(imgBuffer + imgSize, data, len);
                imgSize += len;
                if (imgSize >= 2 && imgBuffer[imgSize - 2] == 0xFF && imgBuffer[imgSize - 1] == 0xD9) {
                    ESP_LOGI(TAG, "End of image detected");
                    imageReady = true;
                }
            } else {
                imgSize = 0; // Reset on overflow
            }
        }
    }
}
