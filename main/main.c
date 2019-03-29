#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"

#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_system.h"

#include "esp_mqtt.h"

#include "../components/adc_read/adc_read.h"
#include "../components/ard_to_esp_serial/ard_to_esp_serial.h"
#include "../components/wifi_transmit/wifi_transmit.h"


void app_main()
{
	//Initialise uart interface
	uartInit();
	// disable the default wifi logging
	esp_log_level_set("wifi", ESP_LOG_NONE);
	// initialize NVS
	ESP_ERROR_CHECK(nvs_flash_init());
	//Initialise mqtt and wifi interface
	esp_mqtt_init(mqtt_status_cb, mqtt_message_cb, 1024, 12000);
	wifiInit();

	//Wait for mqtt and wifi to be initialised
	xEventGroupWaitBits(wifi_event_group, (WIFI_CONNECTED_BIT+MQTT_CONNECTED_BIT),false, true, portMAX_DELAY);

	//Eventgroup to coordinate tasks
	transmitGroup = xEventGroupCreate();
	xTaskCreate(Debug_tx, "uart_tx_task", 1024*2, NULL, 5, NULL);
	xTaskCreate(configReq_tx, "uart_tx_task", 4096, NULL, 2, NULL);
	xTaskCreate(adcReadTask, "adcReadTask", 4096, NULL, 1, NULL);
	xTaskCreate(rx_event_task, "uart_rxEvent_task", 5000, NULL, 3, NULL);

    //Transmit task of ADC + config
    xTaskCreate(publishConfigTask, "publishConfigTask",4096, NULL, 4, NULL);
}


