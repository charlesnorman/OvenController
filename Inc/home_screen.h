/*
 * menu.h
 *
 *  Created on: Jan 28, 2023
 *      Author: Charles
 */

#ifndef INC_HOME_SCREEN_H_
#define INC_HOME_SCREEN_H_

#include "stdint.h"
#include "time.h"
#include <stdbool.h>



typedef enum {
	OFF,
	ON,
	PRE_HEATING,
	TIMED_OUT,
	AUTO_TUNE
} Status;

#define SCREEN_UPDATE_PERIOD 2000

extern bool dirty;

void DisplayHomeScreen();
void UpdateTemperatures(float actual, float target, bool dirty);
void UpdateTimes(uint16_t actual, uint16_t target, bool dirty);
void UpdateProgramStep(uint8_t actual, bool dirty);
void UpdateStatus(Status status, bool dirty);
void UpdateOutputDisplay(uint16_t value, bool dirty);



#endif /* INC_HOME_SCREEN_H_ */
