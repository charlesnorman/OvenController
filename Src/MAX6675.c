/*
 * MAX6675.c
 *
 *  Created on: Jan 22, 2023
 *      Author: Charles
 */
#include "MAX6675.h"

SPI_HandleTypeDef MAX6675_spi;
GPIO_TypeDef *_port;
uint16_t _pin;
uint8_t _format;

void Max6675_Init(SPI_HandleTypeDef spi, GPIO_TypeDef *port, uint16_t pin, uint8_t format) {
	MAX6675_spi = spi;
	_port = port;
	_pin = pin;
	_format = format;
}

float Max6675_Read_Temp(void) {
	float Temp = 0;
	uint8_t DATARX[2];
	HAL_GPIO_WritePin(_port, _pin, GPIO_PIN_RESET); //Low state to enable SPI
	HAL_SPI_Receive(&MAX6675_spi, DATARX, 1, 50);						//Data transfer
	for(int x = 0; x < 150; x++){}
	HAL_GPIO_WritePin(_port, _pin, GPIO_PIN_SET);//High state to stop SPI
	Temp = ((((DATARX[0] | DATARX[1] << 8))) >> 3);
	Temp *= 0.25;

	if(_format == FAHRENHEIT){
		Temp *= (9.0/5.0);
		Temp += 32;
	}

	return Temp;
}
