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
<<<<<<< HEAD

#include "../components/adc_read/adc_read.h"
#include "../components/ard_to_esp_serial/ard_to_esp_serial.h"
=======

#include "../components/adc_read/adc_read.h"

>>>>>>> 9f6756f62565a14297771ed990d07d1c1675101c

void app_main()
{
<<<<<<< HEAD
	uartInit();
	xTaskCreate(adcReadTask, "adcReadTask", 2048, NULL, 1, NULL);
    xTaskCreate(rx_task, "uart_rx_task", 1024*2, NULL, configMAX_PRIORITIES, NULL);
    xTaskCreate(tx_task, "uart_tx_task", 1024*2, NULL, configMAX_PRIORITIES-1, NULL);
=======
	esp_err_t ret;
	xTaskCreate(adcReadTask, "adcReadTask", 2048, NULL, 1, NULL);
>>>>>>> 9f6756f62565a14297771ed990d07d1c1675101c
}

