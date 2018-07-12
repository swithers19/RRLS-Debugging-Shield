#ifndef ARDTOESP_H
#define ARDTOESP_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "soc/uart_struct.h"
#include "string.h"

#include "../trans_format/trans_format.h"

#define TXD_PIN (GPIO_NUM_17)
#define RXD_PIN (GPIO_NUM_16)

typedef struct DeviceConfig {
   int deviceID;
   int id;
   int mode;
   int sensorRead;
   int pinArr[4];

} DeviceConfig;

typedef struct {
	DeviceConfig device[15];
}BoardConfig;

typedef struct {
	int id;
	int mode;
	int8_t packet[10];
}debugMode;

typedef struct {
	uint16_t duration;
	uint8_t cnt;
	debugMode devDebugSettings[15];
}debugSet;

static const int RX_BUF_SIZE = 1024;
static QueueHandle_t uart2_queue;
static SemaphoreHandle_t configSem;
static SemaphoreHandle_t debugSem;
static BoardConfig conDevices;
static debugSet debugOut;

void uartInit();
int sendUartData(const char* logName, const char* data);

//Request flag for config data
void configReq_tx();

//Processes the configuration data sent from the Arduino
void configProcessing(uint8_t* buffer, int length);
void rx_event_task();
void debugSerialPrep(char* serialBuff);
void Debug_tx();

#endif
