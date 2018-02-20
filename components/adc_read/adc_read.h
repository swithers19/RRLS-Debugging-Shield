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
#include "esp_adc_cal.h"

//SPI
#include "driver/spi_master.h"
#include "soc/gpio_struct.h"

#define V_REF 1117
#define ADC1_TEST_CHANNEL (ADC1_CHANNEL_6)

void setBits(unsigned int devicePin, gpio_num_t* gpio_pin);
void gpioConfig(gpio_num_t* multiplexPins, esp_adc_cal_characteristics_t* characteristics);
void read_indi_pin(gpio_num_t* gpio_pin, esp_adc_cal_characteristics_t characteristics);
void adcTask(void *pvParameter);

esp_err_t spiConnect();
int tenBitCon(uint8_t* readData);