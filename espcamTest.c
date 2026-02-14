#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <esp_http_server.h>
#include <esp_camera.h>
#include <esp_wifi.h>
#include <nvs_flash.h>

#define SSID "KAIYURAN 3620"
#define PASSWORD "%5p4A328"

static httpd_handle_t server = NULL;

esp_err_t jpg_stream_handler(httpd_req_t *req) {
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    httpd_resp_set_type(req, "image/jpeg");
    httpd_resp_send(req, (const char *)fb->buf, fb->len);
    esp_camera_fb_return(fb);
    return ESP_OK;
}

esp_err_t index_handler(httpd_req_t *req) {
    const char resp[] = "<!DOCTYPE html><html><body>"
        "<h1>ESP32-CAM Stream</h1>"
        "<img src='/stream' style='width:100%;'>"
        "</body></html>";
    httpd_resp_send(req, resp, strlen(resp));
    return ESP_OK;
}

void start_webserver(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 80;

    httpd_uri_t index_uri = {
        .uri = "/",
        .method = HTTP_GET,
        .handler = index_handler,
    };

    httpd_uri_t stream_uri = {
        .uri = "/stream",
        .method = HTTP_GET,
        .handler = jpg_stream_handler,
    };

    httpd_start(&server, &config);
    httpd_register_uri_handler(server, &index_uri);
    httpd_register_uri_handler(server, &stream_uri);
}

void wifi_init(void) {
    nvs_flash_init();
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_mode(WIFI_MODE_STA);
}

void app_main(void) {
    wifi_init();
    start_webserver();
    printf("Web server started at http://192.168.x.x\n");
}