#ifndef TRAN_FORM_H
#define TRAN_FORM_H

#include "cJSON.h"

#include "../wifi_transmit/wifi_transmit.h"
#include "../ard_to_esp_serial/ard_to_esp_serial.h"

void uartToJson();
void jsonToUart();
void publishConfig();
void handleDebug();

#endif
