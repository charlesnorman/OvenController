/*
 * auto_tune_display.c
 *
 *  Created on: Jun 24, 2023
 *      Author: Charles
 */
#include "auto_tune_display.h"

float Kp, Ki, Kd;
uint8_t target_cycles, current_cycle;

void setPIDValues(float p, float i, float d)
{
	Kp = p;
	Ki = i;
	Kd = d;
}

void setCycleNumber(uint8_t target, uint8_t current)
{
	target_cycles = target;
	current_cycle = current;
}

void displayWorkingScreen()
{
	ST7735_FillRectangle(0, 0, 160, 128, BLACK);

	ST7735_WriteString(0 * Font_11x18.width, 0 * Font_11x18.height, "##",
			Font_11x18, WHITE, BLACK);
	ST7735_WriteString(3 * Font_11x18.width, 0 * Font_11x18.height, "of",
				Font_11x18, WHITE, BLACK);
	ST7735_WriteString(6 * Font_11x18.width, 0 * Font_11x18.height, "##",
				Font_11x18, WHITE, BLACK);
	ST7735_Select();
}
