/*
 * menu.c
 *
 *  Created on: Jan 28, 2023
 *      Author: Charles
 */

#include <home_screen.h>
#include "fonts.h"
#include "main.h"

char sActualTemperature[5];
char sTargetTemperature[5];
float last_actual_temp, last_target_temp;

struct sTimeStruct targetTimeStruct;
struct sTimeStruct actualTimeStruct;

char sActualTime[9];
char sTargetTime[9];
uint16_t last_actual_time, last_target_time;

char sOutputValue[5];
uint8_t last_output_value = 0xFF;

Status lastStatus = OFF;

void DisplayHomeScreen()
{

 	ST7735_FillRectangle(0, 0, 160, 128, BLACK);

	/*Temperature Header*/

	ST7735_WriteString(44, 0, "TEMPERATURE", Font_7x10, WHITE, BLACK);
	ST7735_WriteString(0, 10, " ACTUAL       TARGET", Font_7x10, WHITE,
	BLACK);

	/*Time Header*/
	ST7735_WriteString(60, 45, "TIME", Font_7x10, WHITE, BLACK);
	ST7735_WriteString(0, 57, " ACTUAL       TARGET", Font_7x10, WHITE,
	BLACK);

	/*Program Step*/
	ST7735_WriteString(5, 91, "Program step:# of #", Font_7x10, WHITE,
	BLACK);

	/*Power*/
	ST7735_WriteString(5, 111, " OFF", Font_7x10, RED,
	BLACK);

}
uint32_t lastTemperatureUpdate = 0;
void UpdateTemperatures(float actual, float target, bool dirty)
{
	uint32_t now = HAL_GetTick();
	if ((now - SCREEN_UPDATE_PERIOD) > lastTemperatureUpdate)
	{
		if ((floor(last_actual_temp) != floor(actual)) || dirty)
		{
			sprintf(sActualTemperature, "%.0f", actual);
			ST7735_FillRectangle(21, 22, 7 * Font_11x18.width, 22, BLACK);
			ST7735_WriteString(21, 22, sActualTemperature, Font_11x18, WHITE,
			BLACK);
			last_actual_temp = actual;
		}
		if ((floor(last_target_temp) != floor(target)) || dirty)
		{
			sprintf(sTargetTemperature, "%.0f", target);
			ST7735_FillRectangle(100, 22, 160, 22, BLACK);
			ST7735_WriteString(100, 22, sTargetTemperature, Font_11x18, WHITE,
			BLACK);
			last_target_temp = target;
		}
		lastTemperatureUpdate = now;
	}
}

uint32_t lastTimesUpdate;
void UpdateTimes(uint16_t actual, uint16_t target, bool dirty)
{
	uint32_t now = HAL_GetTick();
	if ((now - SCREEN_UPDATE_PERIOD) > lastTimesUpdate)
	{
		if ((last_actual_time != actual) || dirty)
		{
			/*Update the time structure*/
			intToTime(&actualTimeStruct, actual);
			/*Update the time string*/
			sprintf(sActualTime, "%02d:%02d", actualTimeStruct.hours,
					actualTimeStruct.minutes);
			ST7735_FillRectangle(0, 69, 59, 22, BLACK);
			ST7735_WriteString(5, 69, sActualTime, Font_7x10, WHITE, BLACK);
			last_actual_time = actual;
		}

		if ((last_target_time != target) || dirty)
		{
			/*Update the time structure*/
			intToTime(&targetTimeStruct, target);
			/*Update the time strings*/
			sprintf(sTargetTime, "%02d:%02d", targetTimeStruct.hours,
					targetTimeStruct.minutes);
			ST7735_FillRectangle(60, 69, 60, 22, BLACK);
			ST7735_WriteString(100, 69, sTargetTime, Font_7x10, WHITE, BLACK);
			last_target_time = target;
		}
		lastTimesUpdate = now;
	}
}

void UpdateProgramStep(uint8_t step, bool dirty)
{

}

uint32_t lastUpdateStatus;
void UpdateStatus(Status status, bool dirty)
{
	uint32_t now = HAL_GetTick();
	if ((now - SCREEN_UPDATE_PERIOD) > lastUpdateStatus)
	{
		/*Clear the status line*/
		if ((lastStatus != status) || dirty)
		{
			ST7735_FillRectangle(2, 111, 86, 18, BLACK);
			switch (status)
			{
			case OFF:
				ST7735_WriteString(2, 111, " OFF", Font_7x10, RED,
				BLACK);
				break;
			case ON:
				ST7735_WriteString(2, 111, " ON", Font_7x10, GREEN,
				BLACK);
				break;
			case PRE_HEATING:
				ST7735_WriteString(2, 111, " PRE_HEATING", Font_7x10, GREEN,
				BLACK);
				break;
			case TIMED_OUT:
				ST7735_WriteString(2, 111, " TIMED_OUT", Font_7x10, GREEN,
				BLACK);
				break;
			default:
				ST7735_WriteString(2, 111, " UNKNOWN", Font_7x10, RED,
				BLACK);
				break;
			}
		}
		lastUpdateStatus = now;
	}
}

uint32_t lastUpdateOutput;
void UpdateOutputDisplay(uint16_t value, bool dirty)
{
	uint32_t now = HAL_GetTick();
	if ((now - SCREEN_UPDATE_PERIOD) > lastUpdateOutput)
	{
		if ((last_output_value != value) || dirty)
		{
			//Clear_string(sEncoderValue, sizeof sEncoderValue);
			sprintf(sOutputValue, "%d", value);
			ST7735_FillRectangle(119, 111, 40, 22, BLACK);
			ST7735_WriteString(119, 111, sOutputValue, Font_7x10, RED,
			BLACK);
			ST7735_WriteChar(147, 111, '%', Font_7x10, RED, BLACK);
			last_output_value = value;
		}
		lastUpdateOutput = now;
	}
}

