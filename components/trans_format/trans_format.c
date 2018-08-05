#include "trans_format.h"

const int ADC_READ_BIT = BIT0;
const int CONFIG_READ_BIT = BIT1;
const int DEBUG_BIT = BIT2;
const char* deviceLookup[] = {"LED", "Ultrasonic Sensor", "Temperature Sensor", "Switch", "LED Matrix"};
char *adcJsonString = NULL;
char *configJsonString = NULL;


void adcToJson(uint16_t **adcArray)
{
	int i = 0;
	adcRoot = cJSON_CreateObject();
	cJSON *readings = cJSON_CreateArray();
	cJSON_AddItemToObject(adcRoot, "Readings", readings);
	for(i = 0; i<8; i++) {
		cJSON *reading = cJSON_CreateNumber((int)adcArray[i]);
		cJSON_AddItemToArray(readings, reading);
	}
}

//Config UART data to JSON format for transmission
void configToJson(BoardConfig board, int devCnt)
{
    //char *configJsonString = NULL;
    int i= 0, j = 0;
    //initiate JSON object
    configRoot  = cJSON_CreateObject();
    cJSON_AddStringToObject(configRoot, "Mode", "Config");
	//Assign name to JSON file
	if (board.device[0].deviceID >-1) {
		switch (board.device[0].deviceID){
			case 1:
				cJSON_AddStringToObject(configRoot, "board", "Arduino Nano");
				break;
			default:
				cJSON_AddStringToObject(configRoot, "board", "Generic Board");
				break;
		}
	}
	cJSON *peripherals = cJSON_CreateArray();
	cJSON_AddItemToObject(configRoot, "Peripherals", peripherals);

	//Creating JSON file cycling through device configuration
	for (i = 0; i<devCnt; i++) {
		cJSON *device = cJSON_CreateObject();
		cJSON_AddItemToArray(peripherals, device);
		cJSON_AddStringToObject(device, "peripheral type", deviceLookup[board.device[i].deviceID-1]);
		cJSON_AddNumberToObject(device, "deviceID", board.device[i].id);
		cJSON_AddNumberToObject(device, "mode",  board.device[i].mode);
		cJSON_AddNumberToObject(device, "sensor reading",  board.device[i].sensorRead);
		//Adding pin array of device
		cJSON *pinArray = cJSON_CreateArray();
		for (j = 0;j<4; j++) {
			cJSON *pin = cJSON_CreateNumber(board.device[i].pinArr[j]);
			cJSON_AddItemToArray(pinArray, pin);
		}
		cJSON_AddItemToObject(device, "pin array", pinArray);
	}
}

//Publish configuration settings
void publishConfigTask()
{
	for(;;) {
		xEventGroupWaitBits(wifi_event_group, MQTT_CONNECTED_BIT, false, true, portMAX_DELAY);
		xEventGroupWaitBits(transmitGroup, CONFIG_READ_BIT|ADC_READ_BIT, true, true, portMAX_DELAY);
		cJSON_AddItemToObject(configRoot,"Readings", adcRoot);
		char* jsonString = NULL;
		jsonString = cJSON_Print(configRoot);
		if (jsonString == NULL) {
			printf("NULL\n");
		}
		else {
			esp_mqtt_publish("/RRLSsamW/config", (uint8_t*)jsonString, strlen(jsonString), 2, false);
		}
		cJSON_Delete(configRoot);
		configRoot = NULL;
		free(jsonString);
	}
}


//Handle incoming debug JSON
void handleDebug(const char* jsonDebug)
{
	//xSemaphoreTake(JsonToUart, portMAX_DELAY);
	cJSON* debugJSON = NULL;
	cJSON* periphIterator = NULL;
	char* debugString = NULL;
	int i = 0;
	debugOut.duration = 0;
	debugJSON = cJSON_Parse(jsonDebug);
	if (debugJSON != NULL) {
		debugOut.duration = cJSON_GetObjectItemCaseSensitive(debugJSON, "duration")->valueint;
		debugOut.cnt = cJSON_GetObjectItemCaseSensitive(debugJSON, "count")->valueint;
		periphIterator = cJSON_GetObjectItemCaseSensitive(debugJSON, "peripherals");
		cJSON_ArrayForEach(debugJSON, periphIterator) {
			debugOut.devDebugSettings[i].id = cJSON_GetObjectItemCaseSensitive(debugJSON, "id")->valueint;
			debugOut.devDebugSettings[i].mode = cJSON_GetObjectItemCaseSensitive(debugJSON, "mode")->valueint;
			if (debugOut.devDebugSettings[i].mode > 10) {
				//debugOut.devDebugSettings[i].packet = cJSON_GetObjectItemCaseSensitive(debugJSON, "payload")->valueint;
			}
			i++;
		}
		debugString = cJSON_Print(debugJSON);
		if (debugString != NULL) {
			printf("%s\n", debugString);
		}
	}
	cJSON_Delete(debugJSON);
	free(debugString);
}



