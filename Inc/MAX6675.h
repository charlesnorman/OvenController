/*
 * MAX66.75.h
 *
 *  Created on: Jan 22, 2023
 *      Author: Charles
 */

#ifndef INC_MAX6675_H_
#define INC_MAX6675_H_
#include "stm32f4xx_hal.h"

#define CELCIUS 0
#define FAHRENHEIT 1

void Max6675_Init(SPI_HandleTypeDef spi, GPIO_TypeDef *port, uint16_t pin, uint8_t format);
float Max6675_Read_Temp(void);

extern SPI_HandleTypeDef MAX6675_spi;
extern GPIO_TypeDef *_port;
extern uint16_t _pin;

#endif /* INC_MAX6675_H_ */
