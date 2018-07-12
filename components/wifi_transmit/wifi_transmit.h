#ifndef WIFIT_H
#define WIFIT_H

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "esp_mqtt.h"

//#define WIFI_SSID "BelongTAA4QNQP"
//#define WIFI_PASS "RXENQ6JR3A"

#define WIFI_SSID "SamsPhone"
#define WIFI_PASS "Fusion12"

#define MQTT_HOST "iot.eclipse.org"
#define MQTT_PORT "1883"
#define MQTT_PASS "cz9PlfBpnNF1"
#define MQTT_USER "dnqgyvcy"

const int WIFI_CONNECTED_BIT;
const int MQTT_CONNECTED_BIT;

//mqtt and wifi event bits
EventGroupHandle_t mqttBit;
EventGroupHandle_t wifi_event_group;
//esp_mqtt_client_handle_t client;
static TaskHandle_t task = NULL;

esp_err_t event_handler(void *ctx, system_event_t *event);
void wifiInit();

//MQTT callbacks
void mqtt_status_cb(esp_mqtt_status_t status);
void mqtt_message_cb(const char *topic, uint8_t *payload, size_t len);

#endif
