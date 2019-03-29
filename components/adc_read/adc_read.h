#include <stdio.h>
#include <string.h>
#include <math.h>

//FreeRTOS system
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"
//GPIO
#include "driver/gpio.h"
//ADC Config
#include "driver/adc.h"
//SPI
#include "driver/spi_master.h"
#include "soc/gpio_struct.h"
#include "sdkconfig.h"
//Logging
#include "esp_log.h"

#include "../trans_format/trans_format.h"

#define PIN_NUM_MISO 19
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK 18
#define ADC_1_CS 5


void adcReadTask(void *pvParameters);

esp_err_t spiBusInit(spi_bus_config_t* spiBusCfg, int reInit);

esp_err_t spiDeviceConnect(spi_device_interface_config_t* adcCfg, spi_device_handle_t* spiDevHandle, int slavePin);

int spiTransRead(spi_device_handle_t spiDevHandle, uint8_t cmd);

int tenBitCon(uint8_t* readData);

float voltageConv(uint16_t adc);

