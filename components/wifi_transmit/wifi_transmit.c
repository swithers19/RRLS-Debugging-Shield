#include "wifi_transmit.h"

const int WIFI_CONNECTED_BIT = BIT0;
const int MQTT_CONNECTED_BIT = BIT1;
static const char *TAG = "MQTT_SAMPLE";

//Initialisation of wifi
void wifiInit(){
	// create the event group to handle wifi events
	wifi_event_group = xEventGroupCreate();
	tcpip_adapter_init();
	ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));

	// initialize the wifi stack in STAtion mode with config in RAM
	wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&wifi_init_config));
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

	// configure the wifi connection and start the interface
	wifi_config_t wifi_config = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASS,
        },
    };
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
	printf("Connecting to %s\n", WIFI_SSID);

	xEventGroupWaitBits(wifi_event_group, WIFI_CONNECTED_BIT, false, true, portMAX_DELAY);
}

// Wifi event handler
esp_err_t event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
		case SYSTEM_EVENT_STA_START:
			esp_wifi_connect();
			printf("Got here\n");
			break;

		case SYSTEM_EVENT_STA_GOT_IP:
			xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
			//Connected to esp client
		    esp_mqtt_start(MQTT_HOST, MQTT_PORT, "ESP32-Client28062018", MQTT_USER, MQTT_PASS);
		    //mqttBit = xEventGroupCreate();
			break;
		case SYSTEM_EVENT_STA_DISCONNECTED:
			xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
			ESP_ERROR_CHECK(esp_wifi_start());
			break;
		default:
			break;
		}
	return ESP_OK;
}

//Callback function when message arrives
void mqtt_message_cb(const char *topic, uint8_t *payload, size_t len)
{
	if (strcmp(topic, "/RRLSsamW/debug")==0) {
		handleDebug(topic);
	}
    printf("incoming\t%s:%s (%d)\n", topic, payload, (int)len);
}

//Callback function which handles status of MQTT connection
void mqtt_status_cb(esp_mqtt_status_t status)
{
    switch (status)
    {
    case ESP_MQTT_STATUS_CONNECTED:
        esp_mqtt_subscribe("/RRLSsamW/config", 0);
        esp_mqtt_subscribe("/RRLSsamW/debug", 0);
        xEventGroupSetBits(wifi_event_group, MQTT_CONNECTED_BIT);
        break;
    case ESP_MQTT_STATUS_DISCONNECTED:
        xEventGroupClearBits(wifi_event_group, MQTT_CONNECTED_BIT);
	    esp_mqtt_start(MQTT_HOST, MQTT_PORT, "ESP32-Client28062018", NULL, NULL);
        break;
    }
}

