#include "adc_read.h"

esp_err_t spiBusInit(spi_bus_config_t* spiBusCfg, int reInit)
{
	static int busInit = 0;
	esp_err_t ret=-1;

	//Bus configuration Struct
	spiBusCfg->miso_io_num = PIN_NUM_MISO;
	spiBusCfg->mosi_io_num=PIN_NUM_MOSI;
	spiBusCfg->sclk_io_num=PIN_NUM_CLK;
	spiBusCfg->quadwp_io_num=-1;
	spiBusCfg->quadhd_io_num=-1;

	//Initialize the SPI bus
	if (busInit == 0 || reInit != 0) {
		ret = spi_bus_initialize(VSPI_HOST, spiBusCfg, 0);
		if (ret == ESP_OK) {
			busInit = 1;
			printf("Init successful\n");
		}
		else {
			printf("Init unsuccessful\n");
		}
	}
	assert(ret == ESP_OK);
	return ret;
}


esp_err_t spiDeviceConnect(spi_device_interface_config_t* adcCfg, spi_device_handle_t* spiDevHandle, int slavePin)
{
    esp_err_t ret = -1;

	//ADC device configuration
    adcCfg->clock_speed_hz=100000;            //Clock out at 10 MHz
    adcCfg->mode=0;                           //SPI mode 0
	adcCfg->spics_io_num=slavePin;            //CS pin
	adcCfg->queue_size=8; 					   //Currently queue 2 transactions

	ret =spi_bus_add_device(VSPI_HOST, adcCfg, spiDevHandle);
	return ret;
}


int spiTransRead(spi_device_handle_t spiDevHandle, uint8_t cmd)
{
	int reading = - 1;
	uint8_t shiftedCmd= cmd << 4;

	spi_transaction_t t = {
		.flags = SPI_TRANS_USE_RXDATA |SPI_TRANS_USE_TXDATA,
		.length = 24,// total data length is in bits
		.tx_data[0] = 1,
		.tx_data[1] = 255
	};
	t.tx_data[1] = shiftedCmd;

	//Transmit transaction
	esp_err_t err = spi_device_transmit(spiDevHandle, &t);
	if (err != ESP_OK) {
		printf("Send buffer failed, err = %d", err);
	}
	else {
		reading = tenBitCon(t.rx_data);
	}
	return reading;
}


int tenBitCon(uint8_t* readData)
{
	int i;
	int adcVal;
	int x[2];
	int interm = readData[1];

	for (i = 0;i < 2; i++) {
		x[i] = (interm % 2)* pow(2, i+8);
		interm/=2;
	}
	adcVal = x[0] + x[1] + readData[2];
	return adcVal;
}


float voltageConv(uint16_t adc) {
	float voltage=0;
	voltage = adc * 4.6 / 5;
	return voltage;
}


void adcReadTask(void *pvParameters)
{
	int i;
	esp_err_t ret;
	spi_bus_config_t spiBusCfg = {}; 					//Bus config variable
	spi_device_interface_config_t adcCfg = {};			//adc1 device interface variable
    spi_device_handle_t adc1SpiHandle;
	int data[8];

	spiBusInit(&spiBusCfg, 0); 	   								//Re/Initialise bus and variables
	spiDeviceConnect(&adcCfg, &adc1SpiHandle, ADC_1_CS);		//Attach the ADC to the SPI bus

	while(1) {
		xEventGroupWaitBits(transmitGroup, CONFIG_READ_BIT|DEBUG_BIT, false, false, portMAX_DELAY);
		//transaction definition Structure
		for (i=0; i<8; i++) {
			data[i] = spiTransRead(adc1SpiHandle, (8+i));
			//printf("Data[%d] %d\n", i, data[i]);
		}
		adcToJson(&data);
		xEventGroupSetBits(transmitGroup, ADC_READ_BIT);
	}
}





