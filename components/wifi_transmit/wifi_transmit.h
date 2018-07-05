#ifndef WIFIT_H
#define WIFIT_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "mqtt_client.h"

//#define WIFI_SSID "BelongTAA4QNQP"
//#define WIFI_PASS "RXENQ6JR3A"

#define WIFI_SSID "SamsPhone"
#define WIFI_PASS "Fusion12"

void main_task(void *pvParameter);
esp_err_t event_handler(void *ctx, system_event_t *event);
void wifiInit();

esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event);
void mqtt_app_start(void);
int publishtoTopic(const char* topic, const char* data, int queueMsg);

#endif
