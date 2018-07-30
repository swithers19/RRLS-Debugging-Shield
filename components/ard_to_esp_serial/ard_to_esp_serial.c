#include "ard_to_esp_serial.h"

const int CURR_PROCESS_BIT = BIT0;
const int STRUCT_COMPLETE_BIT = BIT1;
const int TRANSMIT_BIT = BIT2;
const int DEBUG_RUNNING_BIT = BIT3;
EventGroupHandle_t debug_sync_group = NULL;

void uartInit() {
    const uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_2, &uart_config);
    uart_set_pin(UART_NUM_2, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    // We won't use a buffer for sending data.
    uart_driver_install(UART_NUM_2, RX_BUF_SIZE * 2, 0, 20, &uart2_queue, 0);
    uart_enable_pattern_det_intr(UART_NUM_2, '+', 2, 10000, 10, 10);
    uart_pattern_queue_reset(UART_NUM_2, 20);
}


int sendUartData(const char* logName, const char* data)
{
    const int len = strlen(data);
    uart_flush_input(UART_NUM_2);
    const int txBytes = uart_write_bytes(UART_NUM_2, data, len);
    return txBytes;
}


void configReq_tx()
{
	//wait for wifi and mqtt
	configSem = xSemaphoreCreateBinary();
	xSemaphoreGive(configSem);
    static const char *TX_TASK_TAG = "ConfigReqTX";
    esp_log_level_set(TX_TASK_TAG, ESP_LOG_INFO);
    for (;;) {
        xSemaphoreTake(configSem,  2000);
        sendUartData(TX_TASK_TAG, "/");
        ESP_LOGI(TX_TASK_TAG, "Requesting config information using /");
    }
}


void Debug_tx()
{
	char* debugOutput = (char*)malloc(sizeof(debugSet));
	TickType_t durDelay = 0;
	debugSem = xSemaphoreCreateBinary();
	debug_sync_group = xEventGroupCreate();
	EventBits_t transCheck = BIT8;

    static const char *TX_TASK_TAG = "DebugTX";
    esp_log_level_set(TX_TASK_TAG, ESP_LOG_INFO);

    for (;;) {
    	int i = 0;
    	//New message revieved
    	xEventGroupWaitBits(debug_sync_group, CURR_PROCESS_BIT, false, true, portMAX_DELAY);
    	//Format and output data
        debugSerialPrep(debugOutput);
        sendUartData(TX_TASK_TAG, debugOutput);
        //Print Data
        for (i = 0; i<8;i++) {
        	printf("%d", debugOutput[i]);
        	printf(",");
        }
        transCheck = xEventGroupWaitBits(debug_sync_group, TRANSMIT_BIT, false, true, 2000);
        if ((transCheck & (CURR_PROCESS_BIT | TRANSMIT_BIT)) == 1) {
            //Delay to avoid redoing task
        	durDelay = debugOut.duration;
            vTaskDelay(durDelay);
            xEventGroupClearBits(debug_sync_group, TRANSMIT_BIT|CURR_PROCESS_BIT);
        }
    }
    free(debugOutput);

}

//Prepares incoming MQTT signals
void debugSerialPrep(char* serialBuff) {
	int i;
	int total = 0;
	serialBuff[0] = '&';
	serialBuff[1] = (debugOut.duration>>8) & 0xFF;
	serialBuff[2] = debugOut.duration & 0xFF;
	for (i = 0; i<debugOut.cnt; i++) {
		serialBuff[(i*2)+3] = debugOut.devDebugSettings[i].id;
		serialBuff[(i*2)+4] = debugOut.devDebugSettings[i].mode;
		total = i*2+4;
	}
	serialBuff[total+1] = 244;
}


//Rx incoming message handler
void rx_event_task() {
	uart_event_t event;
	size_t buffered_size;
	uint8_t* dtmp = (uint8_t*) malloc(1024);

	for(;;) {
		//Waiting for UART event.
		if(xQueueReceive(uart2_queue, (void * )&event, (portTickType)portMAX_DELAY)) {
			bzero(dtmp, 1024);
			ESP_LOGI("Tag", "uart[%d] event:", UART_NUM_2);
			switch(event.type) {
            case UART_DATA:
                break;
			case UART_PATTERN_DET:
				uart_get_buffered_data_len(UART_NUM_2, &buffered_size);
				int pos = uart_pattern_pop_pos(UART_NUM_2);
				ESP_LOGI("Tag", "[UART PATTERN DETECTED] pos: %d, buffered size: %d", pos, buffered_size);
				if (pos == -1) {
					uart_flush_input(UART_NUM_2);
				}
				else {
					uart_read_bytes(UART_NUM_2, dtmp, pos, 10 / portTICK_PERIOD_MS);
					uint8_t pat[2 + 1];
					memset(pat, 0, sizeof(pat));
					uart_read_bytes(UART_NUM_2, pat, 2, 10 / portTICK_PERIOD_MS);
					//Configuration Detail flag
					if (dtmp[0] =='^' ) {
						configProcessing(dtmp, pos);
						ESP_LOGI("Dev", "read pat : %s", pat);
					}
					//Debug Flag
					else if (dtmp[pos-2] == '~') {
						xEventGroupSetBits(debug_sync_group, TRANSMIT_BIT);
						ESP_LOGI("Debug:", "Received ~");
				    	//set debugbit in transmit group
					}
				}
				break;
			default:
				ESP_LOGI("Tag", "uart event type: %d", event.type);
				break;
			}
		}

	}
}

//Configuring a struct for the config set-up and transmitting via JSON
void configProcessing(uint8_t* buffer, int length) {
	int i,j;
	uint8_t devCnt = 0;
	uint8_t storePoint = 0;

	devCnt = buffer[1]; //Counter for expected number of devices
	if ((9*devCnt+2) == length) {
		//Assign variables to struct based on byte values
		for (i = 0; i<devCnt; i++) {
			conDevices.device[i].deviceID = buffer[(i*9)+2];
			conDevices.device[i].id = buffer[(i*9)+3];
			conDevices.device[i].mode = buffer[(i*9)+4];
			conDevices.device[i].sensorRead = (buffer[(i*9)+5]<<8) + buffer[(i*9)+6];
			for (j= 0; j< 4; j++) {
				conDevices.device[i].pinArr[j] = buffer[(i*9)+7+j];
			}
		}
	}
	configToJson(conDevices, devCnt);
	xEventGroupSetBits(transmitGroup, CONFIG_READ_BIT);
	xSemaphoreGive(configSem); //Allow config to send again
}
