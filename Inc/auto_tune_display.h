/*
 * auto_tune_display.h
 *
 *  Created on: Jun 24, 2023
 *      Author: Charles
 */

#ifndef INC_AUTO_TUNE_DISPLAY_H_
#define INC_AUTO_TUNE_DISPLAY_H_

#include "fonts.h"
#include <stdint.h>

void displayWorkingScreen();
void displayFinishedScreen();
void updateWorkingScreen();
void setPIDValues(float p, float i, float d);
void setCycleNumber(uint8_t totalCycles, uint8_t thisCycle);

#endif /* INC_AUTO_TUNE_DISPLAY_H_ */
