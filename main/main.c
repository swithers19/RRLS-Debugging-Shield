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

#include "../components/adc_read/adc_read.h"
#include "../components/ard_to_esp_serial/ard_to_esp_serial.h"
#include "../components/wifi_transmit/wifi_transmit.h"


void app_main()
{
	uartInit();
	// disable the default wifi logging
	esp_log_level_set("wifi", ESP_LOG_NONE);

	// initialize NVS
	ESP_ERROR_CHECK(nvs_flash_init());
	wifiInit();
	mqtt_app_start();
	//xTaskCreate(adcReadTask, "adcReadTask", 2048, NULL, 1, NULL);
    // xTaskCreate(rx_task, "uart_rx_task", 1024*2, NULL, configMAX_PRIORITIES, NULL);
	//xTaskCreate(Debug_tx, "uart_tx_task", 1024*2, NULL, configMAX_PRIORITIES-1, NULL);
    xTaskCreate(rx_event_task, "uart_rxEvent_task", 5000, NULL, 3, NULL);
    xTaskCreate(configReq_tx, "uart_tx_task", 1024*2, NULL, 4, NULL);

}


