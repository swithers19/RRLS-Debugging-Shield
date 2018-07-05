#include "trans_format.h"
const char* deviceLookup[] = {"LED", "Ultrasonic Sensor"};


void uartToJson(BoardConfig board, int devCnt) {
    char *jsonString = NULL;
    int i = 0;

    //initiate JSON object
	cJSON *devConfigJSON = cJSON_CreateObject();
	//Assign name to JSON file
	if (board.device[0].deviceID >-1) {
		switch (board.device[0].deviceID){
			case 1:
				cJSON_AddStringToObject(devConfigJSON, "name", "Arduino Nano");
				break;
			default:
				cJSON_AddStringToObject(devConfigJSON, "name", "Generic Board");
				break;
		}
	}
	for (i = 0; i<devCnt; i++) {
		cJSON *device = cJSON_CreateObject();
		cJSON_AddItemToObject(devConfigJSON, "dd", device);
		//cJSON_AddNumberToObject(device, "deviceType", board.device[i].deviceID);
		cJSON_AddNumberToObject(device, "deviceID", board.device[i].id);
		cJSON_AddNumberToObject(device, "mode",  board.device[i].mode);
		cJSON_AddNumberToObject(device, "sensor reading",  board.device[i].sensorRead);
		/*
		int j = 0;
		cJSON *pinArray = cJSON_CreateArray();
		for (j = 0;j<4; j++) {
			cJSON_AddItemToArray(pinArray, (cJSON*)board.device[i].pinArr[j]);
		}
		cJSON_AddItemToArray(device, pinArray);
		//cJSON_AddItemToArray();*/
	}
    jsonString = cJSON_Print(devConfigJSON);
    printf("%s", jsonString);
	publishtoTopic("/RRSsamW/config", jsonString, 0);
    cJSON_Delete(devConfigJSON);
}

void jsonToUart() {

}

void publishConfig() {

}

void handleDebug(){


}

