#include "adc_read.h"

#define PIN_NUM_MISO 19
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK  18
#define ADC_A_CS   5

 void adcTask(void *pvParameter)
 {
	while (1) {
		gpio_num_t multiplexPins[5] = {GPIO_NUM_25, GPIO_NUM_26, GPIO_NUM_27, GPIO_NUM_14, GPIO_NUM_12};
		esp_adc_cal_characteristics_t characteristics;
		unsigned int ardPins[5] = {1, 2, 3, 4, 5};
		gpioConfig(multiplexPins, &characteristics);

		setBits(2, multiplexPins);
		read_indi_pin(multiplexPins, characteristics);
		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
 }


esp_err_t spiConnect()
{
	esp_err_t ret;
    spi_device_handle_t spi;
	int x;
	uint16_t data[2];

	//Bus configuration Struct
    spi_bus_config_t buscfg={
        .miso_io_num=PIN_NUM_MISO,
        .mosi_io_num=PIN_NUM_MOSI,
        .sclk_io_num=PIN_NUM_CLK,
        .quadwp_io_num=-1,
        .quadhd_io_num=-1
    };

    //ADC device configuration
    spi_device_interface_config_t devcfg={
    		.clock_speed_hz=100000,            //Clock out at 10 MHz
			.mode=0,                                //SPI mode 0
            .spics_io_num=ADC_A_CS,               //CS pin
            .queue_size=2, //Currently queue 2 transactions
        };

	//Initialize the SPI bus
	ret = spi_bus_initialize(VSPI_HOST, &buscfg, 0);
	assert(ret == ESP_OK);

	//Attach the ADC to the SPI bus
	ret = spi_bus_add_device(VSPI_HOST, &devcfg, &spi);
	while(1) {
		//transaction definition Struct
		spi_transaction_t t = {
			.flags = SPI_TRANS_USE_RXDATA |SPI_TRANS_USE_TXDATA,
			.length = 24,// total data length is in bits
			.tx_data[0] = 1,
			.tx_data[1] = 255
		};

		//Transmit transaction
		esp_err_t err = spi_device_transmit(spi, &t);
		if (err != ESP_OK) {
			printf("Send buffer failed, err = %d", err);
		}

		//Read out data
		data[0] = t.rx_data[0];
		data[1] = t.rx_data[1];
		tenBitCon(t.rx_data);

		printf("Data[0]: %d, Data[1]: %d, Data[2]:%d\n", data[0], data[1], t.rx_data[2]);
		fflush(stdout);
		vTaskDelay(2000 / portTICK_PERIOD_MS);
	}
	return ret;
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
	printf("The value out of 1023 is: %d\n", adcVal);

	return adcVal;
}






void gpioConfig(gpio_num_t* multiplexPins, esp_adc_cal_characteristics_t* characteristics)
{
    int i;

    //Output Configuration
    gpio_config_t multiConfig; //Multiplexer configuration variable
    multiConfig.mode = GPIO_MODE_OUTPUT;  //set as output mode
    //Configure multiplexer pins into output mode
    for (i = 0; i < 5; i++) {
    	multiConfig.pin_bit_mask |= (1ULL<<multiplexPins[i]);
    }
    gpio_config(&multiConfig);

    //ADC Configuration
    adc1_config_width(ADC_WIDTH_BIT_10);
    adc1_config_channel_atten(ADC1_TEST_CHANNEL, ADC_ATTEN_DB_11);
    esp_adc_cal_get_characteristics(V_REF, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_10, characteristics);

}


void setBits(unsigned int devicePin, gpio_num_t* gpio_pin)
{
   for(int bit=0;bit < 5; bit++)
   {
      printf("%i ", devicePin & 0x01);
      gpio_set_level(gpio_pin[bit], (devicePin & 0x01));
      devicePin = devicePin >> 1;
   }

   fflush(stdout);
   printf("/n");
}


void read_indi_pin(gpio_num_t* gpio_pin, esp_adc_cal_characteristics_t characteristics)
{
    uint32_t voltage;
    while(1){
        voltage = adc1_to_voltage(ADC1_TEST_CHANNEL, &characteristics);
        printf("%d mV\n",voltage);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

}