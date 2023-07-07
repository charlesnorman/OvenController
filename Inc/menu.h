/*
 * menu.h
 *
 *  Created on: Apr 5, 2023
 *      Author: Charles
 */

#ifndef INC_MENU_H_
#define INC_MENU_H_

#include <stdbool.h>
#include "stdio.h"
#include "main.h"

#include 	"home_screen.h"
#include	"stdbool.h"
#include 	"ST7735.h"

typedef struct MenuItem
{
	bool selected;
	void (*DoAction)();
	const char *itemName;
	struct MenuItem *prev;
	struct MenuItem *next;
} MenuItem;

struct sTimeStruct
{
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;
};
typedef struct Menu
{
	uint8_t lenMenu;
	MenuItem *items;
} Menu;

typedef struct ScreenCell
{
	uint8_t x;
	uint8_t y;
	bool numberCell;
	bool hasFocus;
	bool selected;
	char *content;
} ScreenCell;

typedef struct FocusList
{
	ScreenCell *cell;
	uint8_t numberOfCells;
} FocusNavList;

void returnFromMainMenu(void);
void displayTemperatureMenu(void);
void displayOnOffMenu(void);
void displayPIDMenu(void);
void displayAutoTuneMenu(void);
void setTime(void);
void setTemperature(void);
void setOn(void);
void setOff(void);
void setPID(void);
void runPIDAutoTune();
void setAutoTuneTemp();
void displayMenu(MenuItem *menuItem, uint8_t length);
void returnFromTempSetMenu(void);
void intToTime(struct sTimeStruct *pTime, uint16_t time);
uint16_t timeToInt(struct sTimeStruct *pTime);

extern MenuItem *currentMenuItem;
extern Menu *currentMenu;
extern bool dirty, temperature_dirty;
extern Menu mainMenu;
extern Menu temperatureMenu;
extern Menu onOffMenu;
extern Menu PIDMenu;
extern Menu autoTuneMenu;
extern float kp, ki, kd;
extern int16_t targetTemperature, autoTuneTargetTemperature;
extern uint16_t targetTime;
extern Status status;

extern FocusNavList temperatureNavList;
extern FocusNavList timeNavList;
extern FocusNavList PIDNavList;
extern FocusNavList AutoTuneTemperatureNavList;

extern bool navUp, navDown, selectionMade;

#endif /* INC_MENU_H_ */
