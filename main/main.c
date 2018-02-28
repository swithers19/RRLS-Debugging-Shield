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


void app_main(void)
{
	esp_err_t ret;
	xTaskCreate(adcReadTask, "adcReadTask", 2048, NULL, 1, NULL);
}

