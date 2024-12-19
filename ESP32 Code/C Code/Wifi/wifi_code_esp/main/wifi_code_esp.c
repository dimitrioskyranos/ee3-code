#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "esp_http_server.h"

#define WIFI_SSID "iPhone - Dimitris"
#define WIFI_PASS "123456789"

#define WEBSOCKET_PORT 81
#define UART_NUM UART_NUM_1
#define TXD_PIN (GPIO_NUM_17)
#define RXD_PIN (GPIO_NUM_18)
#define BUF_SIZE 8192

static EventGroupHandle_t s_wifi_event_group;
const int WIFI_CONNECTED_BIT = BIT0;

static const char *TAG = "main";

uint8_t imgBuffer[BUF_SIZE];
int imgSize = 0;
bool imageReady = false;

// WebSocket server handle
esp_websocket_server_handle_t server = NULL;

// HTTP Server
static const char *TAG_HTTP = "HTTP Server";

// Wi-Fi event handler
static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data) {
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        esp_wifi_connect();
        xEventGroupClearBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

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
}

static void websocket_task(void *arg) {
    while (1) {
        if (imageReady) {
            ESP_LOGI(TAG, "Sending image via WebSocket...");
            esp_websocket_server_send_bin(server, NULL, imgBuffer, imgSize);
            imgSize = 0;
            imageReady = false;
        }
        vTaskDelay(pdMS_TO_TICKS(100)); // Delay for sending updates
    }
}

static void uart_event_task(void *pvParameters) {
    uint8_t data[BUF_SIZE];
    while (1) {
        int len = uart_read_bytes(UART_NUM, data, BUF_SIZE, 20 / portTICK_RATE_MS);
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

// Asynchronous response for POST request
static esp_err_t post_handler(httpd_req_t *req) {
    char buf[100];
    int ret;
    
    // Read the POST request body (assuming it's image data or some signal)
    ret = httpd_req_recv(req, buf, req->content_len);
    if (ret <= 0) {
        ESP_LOGE(TAG_HTTP, "Failed to read POST data");
        return ESP_FAIL;
    }

    // Process the POST request, for example, by triggering image sending
    ESP_LOGI(TAG_HTTP, "Received POST data: %s", buf);

    // You can add logic to trigger image processing or other tasks here

    return ESP_OK;
}

// Create URI (Uniform Resource Identifier) for the server which is added to default gateway
static const httpd_uri_t post_uri_handler = {
    .uri = "/upload",           // URL added to WiFi's default gateway
    .method = HTTP_POST,
    .handler = post_handler,
    .user_ctx = NULL,
};

static void websocket_app_start(void) {
    httpd_handle_t server = NULL;
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    ESP_LOGI(TAG_HTTP, "Starting HTTP server on port: '%d'", config.server_port);
    if (httpd_start(&server, &config) == ESP_OK) {
        ESP_LOGI(TAG_HTTP, "Registering URI handler");
        httpd_register_uri_handler(server, &post_uri_handler);
    }
}

void app_main(void) {
    ESP_ERROR_CHECK(nvs_flash_init());
    wifi_init();

    // Initialize UART
    const uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE};
    uart_param_config(UART_NUM, &uart_config);
    uart_set_pin(UART_NUM, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_driver_install(UART_NUM, BUF_SIZE * 2, 0, 0, NULL, 0);

    // Start WebSocket server
    esp_websocket_server_config_t ws_config = {
        .port = WEBSOCKET_PORT};
    server = esp_websocket_server_init(&ws_config);

    // Start HTTP server with POST request handling
    websocket_app_start();

    // Create tasks
    xTaskCreate(websocket_task, "websocket_task", 4096, NULL, 5, NULL);
    xTaskCreate(uart_event_task, "uart_event_task", 4096, NULL, 5, NULL);

    ESP_LOGI(TAG, "Setup Complete, WebSocket available at ws://172.20.10.3:81");
}