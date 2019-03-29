#ifndef TRAN_FORM_H
#define TRAN_FORM_H

#include "cJSON.h"

#include "../wifi_transmit/wifi_transmit.h"
#include "../ard_to_esp_serial/ard_to_esp_serial.h"

EventGroupHandle_t transmitGroup;
SemaphoreHandle_t OutputDebugSem;

SemaphoreHandle_t JsonToUart;
const int ADC_READ_BIT;
const int CONFIG_READ_BIT;
const int DEBUG_BIT;
cJSON *configRoot;
cJSON *adcRoot;

char *adcJsonString;
char *configJsonString;

void adcToJson();
void configToJson();
void jsonToUart();
void publishConfigTask();
void handleDebug(const char* jsonDebug);
void publishDebugTask();

#endif
