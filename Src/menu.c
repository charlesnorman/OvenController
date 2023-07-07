/*
 * menu.c
 *
 *  Created on: Apr 6, 2023
 *      Author: Charles
 */

#include 	"menu.h"

ScreenCell currentCell;
uint8_t selectedIndex;
char saveCharacter = '<';

static void displaySelectedMenu(void)
{
	currentMenuItem->selected = false;
	currentMenuItem = NULL;
	dirty = true;
	MenuItem **firstItem = currentMenu->items;
	displayMenu(*firstItem, currentMenu->lenMenu);
}
static char intToChar(int number)
{
	return '0' + number;
}

static int charToInt(char number)
{
	return number - '0';
}

//static void changeFocus()
//{
//
//}
//
//static void changeDigit()
//{
//
//}
//
//static void saveAndReturn()
//{
//
//}

static void updateEditDisplay(FocusNavList *navList)
{
	/********** Display the digits***************/
	for (int x = 0; x < navList->numberOfCells; x++)
	{
		uint16_t color = navList->cell[x].selected ? RED : BLACK;
		ST7735_Select();
		ST7735_WriteChar(navList->cell[x].x,
				navList->cell[x].y,
				(char) *(navList->cell[x].content), Font_11x18, WHITE,
				color);
		ST7735_Unselect();
		if (navList->cell[x].hasFocus)
		{
			drawRoundRect(navList->cell[x].x,
					navList->cell[x].y, Font_11x18.width,
					Font_11x18.height, 1, WHITE);
		}
	}
}

static void editSelectionChange(FocusNavList *navList)
{
	if (navList->cell[selectedIndex].selected)
	{
		ST7735_Select();
		ST7735_WriteChar(navList->cell[selectedIndex].x,
				navList->cell[selectedIndex].y,
				(char) *(navList->cell[selectedIndex].content),
				Font_11x18, WHITE,
				RED
				);
		ST7735_Unselect();
		drawRoundRect(navList->cell[selectedIndex].x,
				navList->cell[selectedIndex].y, Font_11x18.width,
				Font_11x18.height, 1, WHITE);
	} else
	{
		ST7735_Select();
		ST7735_WriteChar(navList->cell[selectedIndex].x,
				navList->cell[selectedIndex].y,
				(char) *(navList->cell[selectedIndex].content),
				Font_11x18, WHITE,
				BLACK);
		ST7735_Unselect();
		drawRoundRect(navList->cell[selectedIndex].x,
				navList->cell[selectedIndex].y, Font_11x18.width,
				Font_11x18.height, 1, WHITE);
	}
}

void displayMenu(MenuItem *menuItem, uint8_t length)
{
	bool selectionMade = false;
	//ST7735_FillRectangle(0,0,160,128, BLACK);
	MenuItem *pItem = menuItem;
	for (int x = 0; x < length; x++)
	{
		if (pItem->selected)
		{
			selectionMade = true;
			ST7735_Select();
			ST7735_WriteChar(0, Font_11x18.height * x, '>', Font_11x18, WHITE,
			BLACK);
			currentMenuItem = pItem;
			ST7735_Unselect();
		} else
		{
			ST7735_FillRectangle(0, Font_11x18.height * x, Font_11x18.width,
					Font_11x18.height, BLACK);
		}
		ST7735_WriteString(22, Font_11x18.height * x, pItem->itemName,
				Font_11x18, WHITE, BLACK);
		pItem++;
	}
	if (!selectionMade)
	{
		ST7735_Select();
		ST7735_WriteChar(0, 0, '>', Font_11x18, WHITE, BLACK);
		currentMenuItem = menuItem;
		ST7735_Unselect();
	}
}

void displayPIDMenu(void)
{
	currentMenu = &PIDMenu;
	displaySelectedMenu();
}
void displayTemperatureMenu(void)
{
	currentMenu = &temperatureMenu;
	displaySelectedMenu();
}
void displayOnOffMenu(void)
{
	currentMenu = &onOffMenu;
	displaySelectedMenu();
}
void displayAutoTuneMenu(void)
{
	currentMenu = &autoTuneMenu;
	displaySelectedMenu();
}
void returnFromMainMenu(void)
{
	currentMenu = NULL;
	currentMenuItem->selected = false;
	currentMenuItem = NULL;
	dirty = true;
	DisplayHomeScreen();
}

void returnFromTempSetMenu(void)
{
	currentMenu = &mainMenu;
	currentMenuItem->selected = false;
	currentMenuItem = NULL;
	dirty = true;
	MenuItem **firstItem = currentMenu->items;
	displayMenu(*firstItem, currentMenu->lenMenu);
}
void setOn(void)
{
	if (status == OFF)
	{
		status = ON;
	}
	currentMenu = NULL;
	selectedIndex = 0;
	DisplayHomeScreen();
	dirty = true;
}
void setOff(void)
{
	if ((status == ON) || (status == PRE_HEATING) || (status == TIMED_OUT))
	{
		status = OFF;
	}
	currentMenu = NULL;
	selectedIndex = 0;
	DisplayHomeScreen();
	dirty = true;
}
void setPID(void)
{
	selectionMade = false;
	ST7735_FillRectangle(0, 0, 160, 128, BLACK);

	/******************* convert parameters to digits ****************/

	/*******  Kp ******/
	uint32_t temp = kp * 1000;

	uint8_t workingDigit = temp / 1000000;
	char kp1000sDigit = intToChar(
			workingDigit > 9 ? (workingDigit % 10) : workingDigit);

	temp -= (workingDigit * 1000000);
	workingDigit = temp / 100000;
	char kp100sDigit = intToChar(workingDigit);

	temp -= (workingDigit * 100000);
	workingDigit = temp / 10000;
	char kp10sDigit = intToChar(workingDigit);

	temp -= (workingDigit * 10000);
	workingDigit = temp / 1000;
	char kpUnitsDigit = intToChar(workingDigit);

	temp -= (workingDigit * 1000);
	workingDigit = temp / 100;
	char kpTenthsDigit = intToChar(workingDigit);

	temp -= (workingDigit * 100);
	workingDigit = temp / 10;
	char kpHundrethsDigit = intToChar(workingDigit);

	char kpThousantsDigit = intToChar(temp % 10);

	/********* Ki *****/
	temp = ki * 1000;
	workingDigit = temp / 1000000;
	char ki1000sDigit = intToChar(
			workingDigit > 9 ? (workingDigit % 10) : workingDigit);

	temp -= (workingDigit * 1000000);
	workingDigit = temp / 100000;
	char ki100sDigit = intToChar(workingDigit);

	temp -= (workingDigit * 100000);
	workingDigit = temp / 10000;
	char ki10sDigit = intToChar(workingDigit);

	temp -= (workingDigit * 10000);
	workingDigit = temp / 1000;
	char kiUnitsDigit = intToChar(workingDigit);

	temp -= (workingDigit * 1000);
	workingDigit = temp / 100;
	char kiTenthsDigit = intToChar(workingDigit);

	temp -= (workingDigit * 100);
	workingDigit = temp / 10;
	char kiHundrethsDigit = intToChar(workingDigit);

	char kiThousantsDigit = intToChar(temp % 10);

	/********** Kd ******/
	temp = kd * 1000;
	workingDigit = temp / 1000000;
	char kd1000sDigit = intToChar(
			workingDigit > 9 ? (workingDigit % 10) : workingDigit);

	temp -= (workingDigit * 1000000);
	workingDigit = temp / 100000;
	char kd100sDigit = intToChar(workingDigit);

	temp -= (workingDigit * 100000);
	workingDigit = temp / 10000;
	char kd10sDigit = intToChar(workingDigit);

	temp -= (workingDigit * 10000);
	workingDigit = temp / 1000;
	char kdUnitsDigit = intToChar(workingDigit);

	temp -= (workingDigit * 1000);
	workingDigit = temp / 100;
	char kdTenthsDigit = intToChar(workingDigit);

	temp -= (workingDigit * 100);
	workingDigit = temp / 10;
	char kdHundrethsDigit = intToChar(workingDigit);

	char kdThousantsDigit = intToChar(temp % 10);

	/********** Set the screen cell contents *****/
	/*Kp*/
	PIDNavList.cell[0].content = &kp1000sDigit;
	PIDNavList.cell[1].content = &kp100sDigit;
	PIDNavList.cell[2].content = &kp10sDigit;
	PIDNavList.cell[3].content = &kpUnitsDigit;
	PIDNavList.cell[4].content = &kpTenthsDigit;
	PIDNavList.cell[5].content = &kpHundrethsDigit;
	PIDNavList.cell[6].content = &kpThousantsDigit;
	/*Ki*/
	PIDNavList.cell[7].content = &ki1000sDigit;
	PIDNavList.cell[8].content = &ki100sDigit;
	PIDNavList.cell[9].content = &ki10sDigit;
	PIDNavList.cell[10].content = &kiUnitsDigit;
	PIDNavList.cell[11].content = &kiTenthsDigit;
	PIDNavList.cell[12].content = &kiHundrethsDigit;
	PIDNavList.cell[13].content = &kiThousantsDigit;

	/*Kd*/
	PIDNavList.cell[14].content = &kd1000sDigit;
	PIDNavList.cell[15].content = &kd100sDigit;
	PIDNavList.cell[16].content = &kd10sDigit;
	PIDNavList.cell[17].content = &kdUnitsDigit;
	PIDNavList.cell[18].content = &kdTenthsDigit;
	PIDNavList.cell[19].content = &kdHundrethsDigit;
	PIDNavList.cell[20].content = &kdThousantsDigit;

	char leftArrow = '<';

	PIDNavList.cell[21].content = &leftArrow;

	/*********** Display the labels***************/
	ST7735_WriteString(0 * Font_11x18.width, 1 * Font_11x18.height, "Kp",
			Font_11x18, WHITE,
			BLACK);
	ST7735_WriteString(0 * Font_11x18.width, 2 * Font_11x18.height, "Ki",
			Font_11x18, WHITE,
			BLACK);
	ST7735_WriteString(0 * Font_11x18.width, 3 * Font_11x18.height, "Kd",
			Font_11x18, WHITE,
			BLACK);

	ST7735_Select();
	ST7735_WriteChar(9 * Font_11x18.width, 1 * Font_11x18.height, ',',
			Font_11x18, WHITE, BLACK);
	ST7735_WriteChar(9 * Font_11x18.width, 2 * Font_11x18.height, ',',
			Font_11x18, WHITE, BLACK);
	ST7735_WriteChar(9 * Font_11x18.width, 3 * Font_11x18.height, ',',
			Font_11x18, WHITE, BLACK);
	ST7735_Unselect();

	selectedIndex = 0;
	PIDNavList.cell[0].hasFocus = true;
	updateEditDisplay(&PIDNavList);

	while (1)
	{
		if (navUp && !selectionMade)
		{

			/***************  Modify the Digit ************************/
			if (PIDNavList.cell[selectedIndex].selected
					&& PIDNavList.cell[selectedIndex].numberCell)
			{
				uint8_t number = charToInt(
						*PIDNavList.cell[selectedIndex].content);
				if (number < 9)
				{
					number += 1;
				} else
				{
					number = 0;
				}
				*PIDNavList.cell[selectedIndex].content = intToChar(
						number);
				updateEditDisplay(&PIDNavList);

			}
			/*************** Move the Selection************************/
			if (!PIDNavList.cell[selectedIndex].selected)
			{
				PIDNavList.cell[selectedIndex].hasFocus = false;
				if (selectedIndex < (PIDNavList.numberOfCells - 1))
				{
					selectedIndex += 1;
				} else
				{
					selectedIndex = 0;
				}
				PIDNavList.cell[selectedIndex].hasFocus = true;
				updateEditDisplay(&PIDNavList);
			}
			navUp = false;
		}
		if (navDown && !selectionMade)
		{
			/***************  Modify the Digit ************************/
			if (PIDNavList.cell[selectedIndex].selected
					&& PIDNavList.cell[selectedIndex].numberCell)
			{
				uint8_t number = charToInt(
						*PIDNavList.cell[selectedIndex].content);
				if (number > 0)
				{
					number -= 1;
				} else
				{
					number = 9;
				}
				*PIDNavList.cell[selectedIndex].content = intToChar(
						number);
				updateEditDisplay(&PIDNavList);

			}
			/*************** Move the Selection************************/
			if (!PIDNavList.cell[selectedIndex].selected)
			{
				PIDNavList.cell[selectedIndex].hasFocus = false;
				if (selectedIndex > 0)
				{
					selectedIndex -= 1;
				} else
				{
					selectedIndex = PIDNavList.numberOfCells - 1;
				}
				PIDNavList.cell[selectedIndex].hasFocus = true;
				updateEditDisplay(&PIDNavList);
			}
			navDown = false;
		}

		/*************** Save and return*************************/
		if ((selectedIndex == (PIDNavList.numberOfCells - 1))
				&& (PIDNavList.cell[selectedIndex].selected)
				&& selectionMade)
		{
			/************Convert chars to numbers*/
			float workingVar = 0;
			workingVar += (charToInt(*PIDNavList.cell[0].content) * 1000);
			workingVar += (charToInt(*PIDNavList.cell[1].content) * 100);
			workingVar += (charToInt(*PIDNavList.cell[2].content) * 10);
			workingVar += (charToInt(*PIDNavList.cell[3].content));
			workingVar += (charToInt(*PIDNavList.cell[4].content) * 0.1);
			workingVar += (charToInt(*PIDNavList.cell[5].content) * 0.01);
			workingVar += (charToInt(*PIDNavList.cell[6].content) * 0.001);
			kp = workingVar;

			workingVar = 0;
			workingVar += (charToInt(*PIDNavList.cell[7].content) * 1000);
			workingVar += (charToInt(*PIDNavList.cell[8].content) * 100);
			workingVar += (charToInt(*PIDNavList.cell[9].content) * 10);
			workingVar += (charToInt(*PIDNavList.cell[10].content));
			workingVar += (charToInt(*PIDNavList.cell[11].content) * 0.1);
			workingVar += (charToInt(*PIDNavList.cell[12].content) * 0.01);
			workingVar += (charToInt(*PIDNavList.cell[13].content) * 0.001);
			ki = workingVar;

			workingVar = 0;
			workingVar += (charToInt(*PIDNavList.cell[14].content) * 1000);
			workingVar += (charToInt(*PIDNavList.cell[15].content) * 100);
			workingVar += (charToInt(*PIDNavList.cell[16].content) * 10);
			workingVar += (charToInt(*PIDNavList.cell[17].content));
			workingVar += (charToInt(*PIDNavList.cell[18].content) * 0.1);
			workingVar += (charToInt(*PIDNavList.cell[19].content) * 0.01);
			workingVar += (charToInt(*PIDNavList.cell[20].content) * 0.001);
			kd = workingVar;

			currentMenu = NULL;
			selectionMade = false;
			PIDNavList.cell[selectedIndex].selected = false;
			PIDNavList.cell[selectedIndex].hasFocus = false;
			selectedIndex = 0;
			/*Persist the data to flash*/
			float dataToPersist[3];
			dataToPersist[0] = kp;
			dataToPersist[1] = ki;
			dataToPersist[2] = kd;
			eraseSector(1);
			writeSector(1, 0, (uint8_t*) dataToPersist, 12);
			set_PID_constants(kp, ki, kd);
			DisplayHomeScreen();
			dirty = true;
			break;
		} else if (selectionMade)
		{
			PIDNavList.cell[selectedIndex].selected =
					PIDNavList.cell[selectedIndex].selected ?
					false :
																true;
			editSelectionChange(&PIDNavList);
			selectionMade = false;
		}

	}

}

void setTime(void)
{
	selectionMade = false;
	ST7735_FillRectangle(0, 0, 160, 128, BLACK);

	/****************** Convert target time to digits ****************/
	struct sTimeStruct workingTime;
	intToTime(&workingTime, targetTime);

	char hoursTensDigit = intToChar(workingTime.hours / 10);
	char hoursUnitsDigit = intToChar(workingTime.hours % 10);
	char minutesTensDigit = intToChar(workingTime.minutes / 10);
	char minutesUnitsDigit = intToChar(workingTime.minutes % 10);
	timeNavList.cell[0].content = &hoursTensDigit;
	timeNavList.cell[1].content = &hoursUnitsDigit;
	timeNavList.cell[2].content = &minutesTensDigit;
	timeNavList.cell[3].content = &minutesUnitsDigit;
	timeNavList.cell[4].content = &saveCharacter;

	/**************Display the labels ********************************/
	ST7735_WriteString(14, 20, "HOURS", Font_11x18, WHITE, BLACK);
	ST7735_WriteString(14, 40, "MINUTES", Font_11x18, WHITE, BLACK);

	/*************Set focus on hours tens digit *********************/
	selectedIndex = 0;
	timeNavList.cell[0].hasFocus = true;
	updateEditDisplay(&timeNavList);

	while (1)
	{
		if (navUp && !selectionMade)
		{

			/***************  Modify the Digit ************************/
			if (timeNavList.cell[selectedIndex].selected
					&& timeNavList.cell[selectedIndex].numberCell)
			{
				uint8_t number = charToInt(
						*timeNavList.cell[selectedIndex].content);
				if (number < 9)
				{
					number += 1;
				} else
				{
					number = 0;
				}
				*timeNavList.cell[selectedIndex].content = intToChar(
						number);
				updateEditDisplay(&timeNavList);

			}
			/*************** Move the Selection************************/
			if (!timeNavList.cell[selectedIndex].selected)
			{
				timeNavList.cell[selectedIndex].hasFocus = false;
				if (selectedIndex < (timeNavList.numberOfCells - 1))
				{
					selectedIndex += 1;
				} else
				{
					selectedIndex = 0;
				}
				timeNavList.cell[selectedIndex].hasFocus = true;
				updateEditDisplay(&timeNavList);
			}
			navUp = false;
		}
		if (navDown && !selectionMade)
		{
			/***************  Modify the Digit ************************/
			if (timeNavList.cell[selectedIndex].selected
					&& timeNavList.cell[selectedIndex].numberCell)
			{
				uint8_t number = charToInt(
						*timeNavList.cell[selectedIndex].content);
				if (number > 0)
				{
					number -= 1;
				} else
				{
					number = 9;
				}
				*timeNavList.cell[selectedIndex].content = intToChar(
						number);
				updateEditDisplay(&timeNavList);

			}
			/*************** Move the Selection************************/
			if (!timeNavList.cell[selectedIndex].selected)
			{
				timeNavList.cell[selectedIndex].hasFocus = false;
				if (selectedIndex > 0)
				{
					selectedIndex -= 1;
				} else
				{
					selectedIndex = timeNavList.numberOfCells - 1;
				}
				timeNavList.cell[selectedIndex].hasFocus = true;
				updateEditDisplay(&timeNavList);
			}
			navDown = false;
		}

		/*************** Save and return*************************/
		if ((selectedIndex == (timeNavList.numberOfCells - 1))
				&& (timeNavList.cell[selectedIndex].selected)
				&& selectionMade)
		{
			/************Convert digits to time*/
			uint16_t workingVar = 0;
			workingVar += charToInt(*timeNavList.cell[0].content) * 10 * 3600;
			workingVar += charToInt(*timeNavList.cell[1].content) * 3600;
			workingVar += charToInt(*timeNavList.cell[2].content) * 10 * 60;
			workingVar += charToInt(*timeNavList.cell[3].content) * 60;
			targetTime = workingVar;
			currentMenu = NULL;
			selectionMade = false;
			timeNavList.cell[selectedIndex].selected = false;
			timeNavList.cell[selectedIndex].hasFocus = false;
			selectedIndex = 0;
			DisplayHomeScreen();
			dirty = true;
			temperature_dirty = true;
			break;
		} else if (selectionMade)
		{
			timeNavList.cell[selectedIndex].selected =
					timeNavList.cell[selectedIndex].selected ?
					false :
																true;
			editSelectionChange(&timeNavList);
			selectionMade = false;
		}

	}
}

void intToTime(struct sTimeStruct *pTime, uint16_t time)
{
	pTime->hours = (time / 3600);
	pTime->minutes = (time - (pTime->hours * 3600)) / 60;
	pTime->seconds = (time - (pTime->hours * 3600) - (pTime->minutes * 60));
}

uint16_t timeToInt(struct sTimeStruct *pTime)
{
	uint16_t time = 0;
	time += pTime->hours * 3600;
	time += pTime->minutes * 60;
	time += pTime->seconds;
	return time;
}

void runPIDAutoTune()
{
	status = AUTO_TUNE;
	PID_autotune(autoTuneTargetTemperature, 10);
	status = OFF;
	currentMenu = NULL;
	DisplayHomeScreen();
}

void setAutoTuneTemp()
{
	selectionMade = false;
	ST7735_FillRectangle(0, 0, 160, 128, BLACK);
	uint16_t temp = autoTuneTargetTemperature;
	uint8_t workingDigit;

	/****** Convert target temperature to digits******/
	workingDigit = temp / 1000;
	char digit1000 = intToChar(workingDigit);
	temp -= (workingDigit * 1000);

	workingDigit = temp / 100;
	char digit100 = intToChar(workingDigit);
	temp -= (workingDigit * 100);

	workingDigit = temp / 10;
	char digit10 = intToChar(workingDigit);
	temp -= (workingDigit * 10);

	workingDigit = temp / 1;
	char digit1 = intToChar(workingDigit);

	AutoTuneTemperatureNavList.cell[0].content = &digit1000;
	AutoTuneTemperatureNavList.cell[1].content = &digit100;
	AutoTuneTemperatureNavList.cell[2].content = &digit10;
	AutoTuneTemperatureNavList.cell[3].content = &digit1;
	AutoTuneTemperatureNavList.cell[4].content = &saveCharacter;

	/*************  Set focus on 1000's digit ****************/
	selectedIndex = 0;
	//currentCell = temperatureNavList.cell[0];
	AutoTuneTemperatureNavList.cell[0].hasFocus = true;
	updateEditDisplay(&AutoTuneTemperatureNavList);
	while (1)
		{
			if (navUp && !selectionMade)
			{

				/***************  Modify the Digit ************************/
				if (AutoTuneTemperatureNavList.cell[selectedIndex].selected
						&& AutoTuneTemperatureNavList.cell[selectedIndex].numberCell)
				{
					uint8_t number = charToInt(
							*AutoTuneTemperatureNavList.cell[selectedIndex].content);
					if (number < 9)
					{
						number += 1;
					} else
					{
						number = 0;
					}
					*AutoTuneTemperatureNavList.cell[selectedIndex].content = intToChar(
							number);
					updateEditDisplay(&AutoTuneTemperatureNavList);

				}
				/*************** Move the Selection************************/
				if (!AutoTuneTemperatureNavList.cell[selectedIndex].selected)
				{
					AutoTuneTemperatureNavList.cell[selectedIndex].hasFocus = false;
					if (selectedIndex < (AutoTuneTemperatureNavList.numberOfCells - 1))
					{
						selectedIndex += 1;
					} else
					{
						selectedIndex = 0;
					}
					AutoTuneTemperatureNavList.cell[selectedIndex].hasFocus = true;
					updateEditDisplay(&AutoTuneTemperatureNavList);
				}
				navUp = false;
			}
			if (navDown && !selectionMade)
			{
				/***************  Modify the Digit ************************/
				if (AutoTuneTemperatureNavList.cell[selectedIndex].selected
						&& AutoTuneTemperatureNavList.cell[selectedIndex].numberCell)
				{
					uint8_t number = charToInt(
							*AutoTuneTemperatureNavList.cell[selectedIndex].content);
					if (number > 0)
					{
						number -= 1;
					} else
					{
						number = 9;
					}
					*AutoTuneTemperatureNavList.cell[selectedIndex].content = intToChar(
							number);
					updateEditDisplay(&AutoTuneTemperatureNavList);

				}
				/*************** Move the Selection************************/
				if (!AutoTuneTemperatureNavList.cell[selectedIndex].selected)
				{
					AutoTuneTemperatureNavList.cell[selectedIndex].hasFocus = false;
					if (selectedIndex > 0)
					{
						selectedIndex -= 1;
					} else
					{
						selectedIndex = AutoTuneTemperatureNavList.numberOfCells - 1;
					}
					AutoTuneTemperatureNavList.cell[selectedIndex].hasFocus = true;
					updateEditDisplay(&AutoTuneTemperatureNavList);
				}
				navDown = false;
			}

			/*************** Save and return*************************/
			if ((selectedIndex == (AutoTuneTemperatureNavList.numberOfCells - 1))
					&& (AutoTuneTemperatureNavList.cell[selectedIndex].selected)
					&& selectionMade)
			{
				/************Convert digits to temperature*/
				uint16_t workingVar = 0;
				workingVar += (charToInt(
						*AutoTuneTemperatureNavList.cell[0].content) * 1000);
				workingVar += (charToInt(
						*AutoTuneTemperatureNavList.cell[1].content) * 100);
				workingVar += (charToInt(
						*AutoTuneTemperatureNavList.cell[2].content) * 10);
				workingVar += (charToInt(
						*AutoTuneTemperatureNavList.cell[3].content));
				autoTuneTargetTemperature  = workingVar;
				selectionMade = false;
				AutoTuneTemperatureNavList.cell[selectedIndex].selected = false;
				AutoTuneTemperatureNavList.cell[selectedIndex].hasFocus = false;
				currentMenu = NULL;
				selectedIndex = 0;
				temperature_dirty = true;
				DisplayHomeScreen();
				break;
			} else if (selectionMade)
			{
				AutoTuneTemperatureNavList.cell[selectedIndex].selected =
						AutoTuneTemperatureNavList.cell[selectedIndex].selected ?
						false :
																			true;
				editSelectionChange(&AutoTuneTemperatureNavList);
				selectionMade = false;
			}

		}



}

void setTemperature(void)
{

	selectionMade = false;
	ST7735_FillRectangle(0, 0, 160, 128, BLACK);
	uint16_t temp = targetTemperature;
	uint8_t workingDigit;

	/****** Convert target temperature to digits******/
	workingDigit = temp / 1000;
	char digit1000 = intToChar(workingDigit);
	temp -= (workingDigit * 1000);

	workingDigit = temp / 100;
	char digit100 = intToChar(workingDigit);
	temp -= (workingDigit * 100);

	workingDigit = temp / 10;
	char digit10 = intToChar(workingDigit);
	temp -= (workingDigit * 10);

	workingDigit = temp / 1;
	char digit1 = intToChar(workingDigit);

	temperatureNavList.cell[0].content = &digit1000;
	temperatureNavList.cell[1].content = &digit100;
	temperatureNavList.cell[2].content = &digit10;
	temperatureNavList.cell[3].content = &digit1;
	temperatureNavList.cell[4].content = &saveCharacter;

	/*************  Set focus on 1000's digit ****************/
	selectedIndex = 0;
	//currentCell = temperatureNavList.cell[0];
	temperatureNavList.cell[0].hasFocus = true;
	updateEditDisplay(&temperatureNavList);

	while (1)
	{
		if (navUp && !selectionMade)
		{

			/***************  Modify the Digit ************************/
			if (temperatureNavList.cell[selectedIndex].selected
					&& temperatureNavList.cell[selectedIndex].numberCell)
			{
				uint8_t number = charToInt(
						*temperatureNavList.cell[selectedIndex].content);
				if (number < 9)
				{
					number += 1;
				} else
				{
					number = 0;
				}
				*temperatureNavList.cell[selectedIndex].content = intToChar(
						number);
				updateEditDisplay(&temperatureNavList);

			}
			/*************** Move the Selection************************/
			if (!temperatureNavList.cell[selectedIndex].selected)
			{
				temperatureNavList.cell[selectedIndex].hasFocus = false;
				if (selectedIndex < (temperatureNavList.numberOfCells - 1))
				{
					selectedIndex += 1;
				} else
				{
					selectedIndex = 0;
				}
				temperatureNavList.cell[selectedIndex].hasFocus = true;
				updateEditDisplay(&temperatureNavList);
			}
			navUp = false;
		}
		if (navDown && !selectionMade)
		{
			/***************  Modify the Digit ************************/
			if (temperatureNavList.cell[selectedIndex].selected
					&& temperatureNavList.cell[selectedIndex].numberCell)
			{
				uint8_t number = charToInt(
						*temperatureNavList.cell[selectedIndex].content);
				if (number > 0)
				{
					number -= 1;
				} else
				{
					number = 9;
				}
				*temperatureNavList.cell[selectedIndex].content = intToChar(
						number);
				updateEditDisplay(&temperatureNavList);

			}
			/*************** Move the Selection************************/
			if (!temperatureNavList.cell[selectedIndex].selected)
			{
				temperatureNavList.cell[selectedIndex].hasFocus = false;
				if (selectedIndex > 0)
				{
					selectedIndex -= 1;
				} else
				{
					selectedIndex = temperatureNavList.numberOfCells - 1;
				}
				temperatureNavList.cell[selectedIndex].hasFocus = true;
				updateEditDisplay(&temperatureNavList);
			}
			navDown = false;
		}

		/*************** Save and return*************************/
		if ((selectedIndex == (temperatureNavList.numberOfCells - 1))
				&& (temperatureNavList.cell[selectedIndex].selected)
				&& selectionMade)
		{
			/************Convert digits to temperature*/
			uint16_t workingVar = 0;
			workingVar += (charToInt(
					*temperatureNavList.cell[0].content) * 1000);
			workingVar += (charToInt(
					*temperatureNavList.cell[1].content) * 100);
			workingVar += (charToInt(
					*temperatureNavList.cell[2].content) * 10);
			workingVar += (charToInt(
					*temperatureNavList.cell[3].content));
			targetTemperature = workingVar;
			set_target_temperature(targetTemperature);
			selectionMade = false;
			temperatureNavList.cell[selectedIndex].selected = false;
			temperatureNavList.cell[selectedIndex].hasFocus = false;
			currentMenu = NULL;
			selectedIndex = 0;
			temperature_dirty = true;
			DisplayHomeScreen();
			break;
		} else if (selectionMade)
		{
			temperatureNavList.cell[selectedIndex].selected =
					temperatureNavList.cell[selectedIndex].selected ?
					false :
																		true;
			editSelectionChange(&temperatureNavList);
			selectionMade = false;
		}

	}

}

