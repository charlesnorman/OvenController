/*
 * encoder.h
 *
 *  Created on: Apr 4, 2023
 *      Author: Charles
 */

#ifndef INC_ENCODER_H_
#define INC_ENCODER_H_

#include "stm32f4xx.h"
#include "stdint.h"

#define ENCODER_VALUE	TIM2->CNT
#define ENCODER_UP				1
#define ENCODER_DOWN			-1
#define ENCODER_NO_CHANGE		0


int8_t PollEncoder();

extern uint8_t lastEncoderValue;

#endif /* INC_ENCODER_H_ */
