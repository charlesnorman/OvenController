/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;
SPI_HandleTypeDef hspi2;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim11;

UART_HandleTypeDef huart2;

/* Definitions for MainLoop */
osThreadId_t MainLoopHandle;
const osThreadAttr_t MainLoop_attributes =
		{
				.name = "MainLoop",
				.stack_size = 256 * 4,
				.priority = (osPriority_t) osPriorityNormal,
		};
/* Definitions for ReadTemperature */
osThreadId_t ReadTemperatureHandle;
const osThreadAttr_t ReadTemperature_attributes =
		{
				.name = "ReadTemperature",
				.stack_size = 256 * 4,
				.priority = (osPriority_t) osPriorityNormal,
		};
/* Definitions for PollEncoder */
osThreadId_t PollEncoderHandle;
const osThreadAttr_t PollEncoder_attributes =
		{
				.name = "PollEncoder",
				.stack_size = 256 * 4,
				.priority = (osPriority_t) osPriorityNormal,
		};
/* Definitions for SerialOut */
osThreadId_t SerialOutHandle;
const osThreadAttr_t SerialOut_attributes =
		{
				.name = "SerialOut",
				.stack_size = 256 * 4,
				.priority = (osPriority_t) osPriorityNormal,
		};
/* USER CODE BEGIN PV */

volatile bool wait_for_heatup = true;
int16_t targetTemperature = 78, autoTuneTargetTemperature = 350;
float temperature, lastTemperature;

uint16_t elapsedTime;
uint16_t targetTime;
Status status = OFF;
bool dirty;
bool temperature_dirty;

bool selectionMade = false;
uint32_t lastMenuTriggered;
uint8_t encoderFilter = 50;

Menu *currentMenu = NULL;
MenuItem *currentMenuItem = NULL;
bool navUp = false;
bool navDown = false;
uint8_t lastEncoderValue = 0;

bool preheatedLatch = false;
bool timedOutLatch = false;

float kp, ki, kd;
float *kp_Saved = (float*) 0x8004000;
float *ki_Saved = (float*) 0x8004004;
float *kd_Saved = (float*) 0x8004008;
uint32_t PID_output = 0;
float PID_output_percent = 0;



/******************************Build Main Menu*******************************/
void returnFromMainMenu();
const char mainMenuReturnItemName[] =
		{ "Return" };
const char temperatureMenuItemName[] =
		{ "Temperature" };
const char timeMenuItemName[] =
		{ "Time" };
const char onOffMenuItemName[] =
		{ "ON/OFF" };
const char PIDMenuItemName[] =
		{ "PID" };
const char AutoTuneMenuItemName[] =
		{ "Auto-Tune" };

MenuItem mainMenuReturnItem =
		{
				.selected = false,
				.itemName = mainMenuReturnItemName,
				.DoAction = returnFromMainMenu,

		};
MenuItem mainMenuTemperatureItem =
		{
				.selected = false,
				.itemName = temperatureMenuItemName,
				.DoAction = displayTemperatureMenu

		};
MenuItem mainMenuTimeMenuItem =
		{
				.selected = false,
				.itemName = timeMenuItemName,
				.DoAction = setTime,

		};
MenuItem mainMenuOnOffMenuItem =
		{
				.selected = false,
				.itemName = onOffMenuItemName,
				.DoAction = displayOnOffMenu
		};
MenuItem mainMenuPIDMenuItem =
		{
				.selected = false,
				.itemName = PIDMenuItemName,
				.DoAction = displayPIDMenu
		};
MenuItem mainMenuAutoTuneItem =
		{
				.selected = false,
				.itemName = AutoTuneMenuItemName,
				.DoAction = displayAutoTuneMenu
		};
MenuItem *mainMenuItems[] =
		{
				&mainMenuReturnItem,
				&mainMenuTemperatureItem,
				&mainMenuTimeMenuItem,
				&mainMenuOnOffMenuItem,
				&mainMenuPIDMenuItem,
				&mainMenuAutoTuneItem
		};

Menu mainMenu;

/**************************Build Temperature Menu*************************/

const char setTargetTempMenuRTNName[] =
		{
				"RETURN" };
const char setTargetSetMenuName[] =
		{
				"SET" };

MenuItem temperatureMenuReturnItem =
		{
				.selected = false,
				.itemName = setTargetTempMenuRTNName,
				.DoAction = returnFromTempSetMenu

		};
MenuItem setTemperatureItem =
		{
				.selected = false,
				.itemName = setTargetSetMenuName,
				.DoAction = setTemperature
		};

MenuItem *temperatureMenuItems[] =
		{
				&temperatureMenuReturnItem,
				&setTemperatureItem
		};

Menu temperatureMenu;

/**************************Build ON/OFF Menu*************************/
const char setOnMenuItemName[] =
		{
				"ON" };
const char setOffMenuItemName[] =
		{
				"OFF" };

MenuItem setOnMenuItem =
		{
				.itemName = setOnMenuItemName,
				.selected = false,
				.DoAction = setOn
		};

MenuItem setOffMenuItem =
		{
				.itemName = setOffMenuItemName,
				.selected = false,
				.DoAction = setOff
		};

MenuItem *onOffMenuItems[] =
		{
				&setOnMenuItem,
				&setOffMenuItem
		};

Menu onOffMenu;

/**************************Build PID Menu*************************/
const char setPIDReturnName[] =
		{
				"RETURN" };
const char setPIDName[] =
		{
				"SET" };

MenuItem PIDMenuReturnItem =
		{
				.itemName = setPIDReturnName,
				.selected = false,
				.DoAction = returnFromMainMenu
		};
MenuItem PIDMenuSetItem =
		{
				.itemName = setPIDName,
				.selected = false,
				.DoAction = setPID
		};

MenuItem *PIDMenuItems[] =
		{
				&PIDMenuReturnItem,
				&PIDMenuSetItem
		};

Menu PIDMenu;

/**************************Build Auto tune Menu*************************/
const char setAutoTuneTemperatureName[] =
		{ "Temperature" };

const char startAutoTuneName[] =
		{ "Start" };

const char returnToMainMenuName[] =
		{ "Return" };

MenuItem AutoTuneSetTempItem =
		{
				.itemName = setAutoTuneTemperatureName,
				.selected = false,
				.DoAction = setAutoTuneTemp
		};
MenuItem AutoTuneStartItem =
		{
				.itemName = startAutoTuneName,
				.selected = false,
				.DoAction = runPIDAutoTune
		};

MenuItem AutoTuneReturnItem =
		{
				.itemName = returnToMainMenuName,
				.selected = false,
				.DoAction = returnFromMainMenu
		};

MenuItem *AutoTuneMenuItems[] =
		{
				&AutoTuneSetTempItem,
				&AutoTuneStartItem,
				&AutoTuneReturnItem
		};

Menu autoTuneMenu;
/**************************Temperature entry screen*************************/
const uint8_t fontWidth = 11;
const uint8_t fontHeight = 18;

ScreenCell temperatureEntryCells[] =
		{
				{ .numberCell = true, .x = 4 * fontWidth, .y = 2 * fontHeight },
				{ .numberCell = true, .x = 5 * fontWidth, .y = 2 * fontHeight },
				{ .numberCell = true, .x = 6 * fontWidth, .y = 2 * fontHeight },
				{ .numberCell = true, .x = 7 * fontWidth, .y = 2 * fontHeight },
				{ .numberCell = false, .x = 5 * fontWidth, .y = 3 * fontHeight }

		};
FocusNavList temperatureNavList;

/**************************Time entry screen ******************************/

ScreenCell timeEntryCells[] =
		{
				{ .numberCell = true, .x = 9 * fontWidth, 1 * fontHeight },
				{ .numberCell = true, .x = 10 * fontWidth, 1 * fontHeight },
				{ .numberCell = true, .x = 9 * fontWidth, 2 * fontHeight },
				{ .numberCell = true, .x = 10 * fontWidth, 2 * fontHeight },
				{ .numberCell = false, .x = 5 * fontWidth, 3 * fontHeight }
		};

FocusNavList timeNavList;

/**************************PID entry screen ******************************/
ScreenCell PIDEntryCells[] =
		{
				/*Kp cells*/
				{ .numberCell = true, .x = 3 * fontWidth, 1 * fontHeight },
				{ .numberCell = true, .x = 4 * fontWidth, 1 * fontHeight },
				{ .numberCell = true, .x = 5 * fontWidth, 1 * fontHeight },
				{ .numberCell = true, .x = 6 * fontWidth, 1 * fontHeight },
				{ .numberCell = true, .x = 8 * fontWidth, 1 * fontHeight },
				{ .numberCell = true, .x = 9 * fontWidth, 1 * fontHeight },
				{ .numberCell = true, .x = 10 * fontWidth, 1 * fontHeight },

				/*Ki cells*/
				{ .numberCell = true, .x = 3 * fontWidth, 2 * fontHeight },
				{ .numberCell = true, .x = 4 * fontWidth, 2 * fontHeight },
				{ .numberCell = true, .x = 5 * fontWidth, 2 * fontHeight },
				{ .numberCell = true, .x = 6 * fontWidth, 2 * fontHeight },
				{ .numberCell = true, .x = 8 * fontWidth, 2 * fontHeight },
				{ .numberCell = true, .x = 9 * fontWidth, 2 * fontHeight },
				{ .numberCell = true, .x = 10 * fontWidth, 2 * fontHeight },

				/*Kd cells*/
				{ .numberCell = true, .x = 3 * fontWidth, 3 * fontHeight },
				{ .numberCell = true, .x = 4 * fontWidth, 3 * fontHeight },
				{ .numberCell = true, .x = 5 * fontWidth, 3 * fontHeight },
				{ .numberCell = true, .x = 6 * fontWidth, 3 * fontHeight },
				{ .numberCell = true, .x = 8 * fontWidth, 3 * fontHeight },
				{ .numberCell = true, .x = 9 * fontWidth, 3 * fontHeight },
				{ .numberCell = true, .x = 10 * fontWidth, 3 * fontHeight },

				/*Entry Cell*/
				{ .numberCell = false, .x = 4 * fontWidth, 4 * fontHeight }
		};

FocusNavList PIDNavList;

/**************************Auto tune Temperature entry screen *********************/
ScreenCell AutoTuneTemperatureEntryCells[] =
		{
				{ .numberCell = true, .x = 4 * fontWidth, .y = 2 * fontHeight },
				{ .numberCell = true, .x = 5 * fontWidth, .y = 2 * fontHeight },
				{ .numberCell = true, .x = 6 * fontWidth, .y = 2 * fontHeight },
				{ .numberCell = true, .x = 7 * fontWidth, .y = 2 * fontHeight },
				{ .numberCell = false, .x = 5 * fontWidth, .y = 3 * fontHeight }
		};

FocusNavList AutoTuneTemperatureNavList;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM3_Init(void);
static void MX_SPI1_Init(void);
static void MX_SPI2_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM11_Init(void);
void StartMainLoop(void *argument);
void StartReadTemperatuer(void *argument);
void StartPollEncoder(void *argument);
void StartSerialOut(void *argument);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	/* USER CODE BEGIN 1 */
	/**************************** Main Menu **************************/
	mainMenu.items = (MenuItem*) mainMenuItems;
	mainMenu.lenMenu = sizeof(mainMenuItems) / 4;
	mainMenuReturnItem.prev = NULL;
	mainMenuReturnItem.next = &mainMenuTemperatureItem;
	mainMenuTemperatureItem.prev = &mainMenuReturnItem;
	mainMenuTemperatureItem.next = &mainMenuTimeMenuItem;
	mainMenuTimeMenuItem.prev = &mainMenuTemperatureItem;
	mainMenuTimeMenuItem.next = &mainMenuOnOffMenuItem;
	mainMenuOnOffMenuItem.prev = &mainMenuTimeMenuItem;
	mainMenuOnOffMenuItem.next = &mainMenuPIDMenuItem;
	mainMenuPIDMenuItem.prev = &mainMenuOnOffMenuItem;
	mainMenuPIDMenuItem.next = &mainMenuAutoTuneItem;
	mainMenuAutoTuneItem.prev = &mainMenuPIDMenuItem;
	mainMenuAutoTuneItem.next = NULL;

	/**************************** Temperature Menu *********************/
	temperatureMenu.items = (MenuItem*) temperatureMenuItems;
	temperatureMenu.lenMenu = sizeof(temperatureMenuItems) / 4;
	temperatureMenuReturnItem.prev = NULL;
	temperatureMenuReturnItem.next = &setTemperatureItem;
	setTemperatureItem.prev = &temperatureMenuReturnItem;
	setTemperatureItem.next = NULL;

	/**************************** ON OFF Menu *********************/
	onOffMenu.items = (MenuItem*) onOffMenuItems;
	onOffMenu.lenMenu = sizeof(onOffMenuItems) / 4;
	setOnMenuItem.prev = NULL;
	setOnMenuItem.next = &setOffMenuItem;
	setOffMenuItem.prev = &setOnMenuItem;
	setOffMenuItem.next = NULL;

	/**************************** PID Menu *********************/
	PIDMenu.items = (MenuItem*) PIDMenuItems;
	PIDMenu.lenMenu = sizeof(PIDMenuItems) / 4;
	PIDMenuReturnItem.prev = NULL;
	PIDMenuReturnItem.next = &PIDMenuSetItem;
	PIDMenuSetItem.prev = &PIDMenuReturnItem;
	PIDMenuSetItem.next = NULL;

	/**************************** Auto Tune Menu *********************/
	autoTuneMenu.items = (MenuItem*) AutoTuneMenuItems;
	autoTuneMenu.lenMenu = sizeof(AutoTuneMenuItems) / 4;
	AutoTuneSetTempItem.prev = NULL;
	AutoTuneSetTempItem.next = &AutoTuneStartItem;
	AutoTuneStartItem.prev = &AutoTuneSetTempItem;
	AutoTuneStartItem.next = &AutoTuneReturnItem;
	AutoTuneReturnItem.prev = &AutoTuneStartItem;
	AutoTuneReturnItem.next = NULL;

	/**************************** Temperature Entry Screen*****************/
	temperatureNavList.cell = temperatureEntryCells;
	temperatureNavList.numberOfCells = sizeof(temperatureEntryCells)
			/ sizeof(ScreenCell);

	/*************************time entry screen ****************************/
	timeNavList.cell = timeEntryCells;
	timeNavList.numberOfCells = sizeof(timeEntryCells) / sizeof(ScreenCell);

	/*************************PID entry screen ****************************/
	PIDNavList.cell = PIDEntryCells;
	PIDNavList.numberOfCells = sizeof(PIDEntryCells) / sizeof(ScreenCell);

	/***************Auto tune temperature entry screen ****************************/
	AutoTuneTemperatureNavList.cell = AutoTuneTemperatureEntryCells;
	AutoTuneTemperatureNavList.numberOfCells =
			sizeof(AutoTuneTemperatureEntryCells) / sizeof(ScreenCell);

	/************* Load Stored parameters *********************************/
	initCRNFlash();
//	kp = 123.456;
//	ki = 789.123;
//	kd = 456.789;
	kp = *kp_Saved;
	ki = *ki_Saved;
	kd = *kd_Saved;

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_USART2_UART_Init();
	MX_TIM3_Init();
	MX_SPI1_Init();
	MX_SPI2_Init();
	MX_TIM2_Init();
	MX_TIM11_Init();
	/* USER CODE BEGIN 2 */

	HAL_TIM_Base_Start_IT(&htim11);
	HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
	HAL_TIM_Encoder_Start_IT(&htim2, TIM_CHANNEL_ALL);

	Max6675_Init(hspi1, MAX6675_CS_GPIO_Port, MAX6675_CS_Pin, FAHRENHEIT);
	ST7735_Init(1);
	/* USER CODE END 2 */

	/* Init scheduler */
	osKernelInitialize();

	/* USER CODE BEGIN RTOS_MUTEX */
	/* add mutexes, ... */
	/* USER CODE END RTOS_MUTEX */

	/* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */
	/* USER CODE END RTOS_SEMAPHORES */

	/* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
	/* USER CODE END RTOS_TIMERS */

	/* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
	/* USER CODE END RTOS_QUEUES */

	/* Create the thread(s) */
	/* creation of MainLoop */
	MainLoopHandle = osThreadNew(StartMainLoop, NULL, &MainLoop_attributes);

	/* creation of ReadTemperature */
	ReadTemperatureHandle = osThreadNew(StartReadTemperatuer, NULL,
			&ReadTemperature_attributes);

	/* creation of PollEncoder */
	PollEncoderHandle = osThreadNew(StartPollEncoder, NULL,
			&PollEncoder_attributes);

	/* creation of SerialOut */
	SerialOutHandle = osThreadNew(StartSerialOut, NULL, &SerialOut_attributes);

	/* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */
	/* USER CODE END RTOS_THREADS */

	/* USER CODE BEGIN RTOS_EVENTS */
	/* add events, ... */
	/* USER CODE END RTOS_EVENTS */

	/* Start scheduler */
	osKernelStart();
	/* We should never get here as control is now taken by the scheduler */
	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct =
			{ 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct =
			{ 0 };

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 100;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 4;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
 * @brief SPI1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_SPI1_Init(void)
{

	/* USER CODE BEGIN SPI1_Init 0 */

	/* USER CODE END SPI1_Init 0 */

	/* USER CODE BEGIN SPI1_Init 1 */

	/* USER CODE END SPI1_Init 1 */
	/* SPI1 parameter configuration*/
	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI_MODE_MASTER;
	hspi1.Init.Direction = SPI_DIRECTION_2LINES_RXONLY;
	hspi1.Init.DataSize = SPI_DATASIZE_16BIT;
	hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi1.Init.NSS = SPI_NSS_SOFT;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial = 10;
	if (HAL_SPI_Init(&hspi1) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN SPI1_Init 2 */

	/* USER CODE END SPI1_Init 2 */

}

/**
 * @brief SPI2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_SPI2_Init(void)
{

	/* USER CODE BEGIN SPI2_Init 0 */

	/* USER CODE END SPI2_Init 0 */

	/* USER CODE BEGIN SPI2_Init 1 */

	/* USER CODE END SPI2_Init 1 */
	/* SPI2 parameter configuration*/
	hspi2.Instance = SPI2;
	hspi2.Init.Mode = SPI_MODE_MASTER;
	hspi2.Init.Direction = SPI_DIRECTION_2LINES;
	hspi2.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi2.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi2.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi2.Init.NSS = SPI_NSS_SOFT;
	hspi2.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
	hspi2.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi2.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi2.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi2.Init.CRCPolynomial = 10;
	if (HAL_SPI_Init(&hspi2) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN SPI2_Init 2 */

	/* USER CODE END SPI2_Init 2 */

}

/**
 * @brief TIM2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM2_Init(void)
{

	/* USER CODE BEGIN TIM2_Init 0 */

	/* USER CODE END TIM2_Init 0 */

	TIM_Encoder_InitTypeDef sConfig =
			{ 0 };
	TIM_MasterConfigTypeDef sMasterConfig =
			{ 0 };

	/* USER CODE BEGIN TIM2_Init 1 */

	/* USER CODE END TIM2_Init 1 */
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 0;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 40 - 1;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	sConfig.EncoderMode = TIM_ENCODERMODE_TI1;
	sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
	sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
	sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
	sConfig.IC1Filter = 3;
	sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
	sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
	sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
	sConfig.IC2Filter = 3;
	if (HAL_TIM_Encoder_Init(&htim2, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN TIM2_Init 2 */

	/* USER CODE END TIM2_Init 2 */

}

/**
 * @brief TIM3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM3_Init(void)
{

	/* USER CODE BEGIN TIM3_Init 0 */

	/* USER CODE END TIM3_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig =
			{ 0 };
	TIM_MasterConfigTypeDef sMasterConfig =
			{ 0 };
	TIM_OC_InitTypeDef sConfigOC =
			{ 0 };

	/* USER CODE BEGIN TIM3_Init 1 */

	/* USER CODE END TIM3_Init 1 */
	htim3.Instance = TIM3;
	htim3.Init.Prescaler = 10000;
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.Period = 1000 - 1;
	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
	{
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
	{
		Error_Handler();
	}
	if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
	{
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 0;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN TIM3_Init 2 */

	/* USER CODE END TIM3_Init 2 */
	HAL_TIM_MspPostInit(&htim3);

}

/**
 * @brief TIM11 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM11_Init(void)
{

	/* USER CODE BEGIN TIM11_Init 0 */

	/* USER CODE END TIM11_Init 0 */

	/* USER CODE BEGIN TIM11_Init 1 */

	/* USER CODE END TIM11_Init 1 */
	htim11.Instance = TIM11;
	htim11.Init.Prescaler = 10000 - 1;
	htim11.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim11.Init.Period = 10000 - 1;
	htim11.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim11.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim11) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN TIM11_Init 2 */

	/* USER CODE END TIM11_Init 2 */

}

/**
 * @brief USART2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART2_UART_Init(void)
{

	/* USER CODE BEGIN USART2_Init 0 */

	/* USER CODE END USART2_Init 0 */

	/* USER CODE BEGIN USART2_Init 1 */

	/* USER CODE END USART2_Init 1 */
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart2) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN USART2_Init 2 */

	/* USER CODE END USART2_Init 2 */

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct =
			{ 0 };
	/* USER CODE BEGIN MX_GPIO_Init_1 */
	/* USER CODE END MX_GPIO_Init_1 */

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, HeartBeat_Pin | MAX6675_CS_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(DISPL_CS_GPIO_Port, DISPL_CS_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOC, DISPL_RST_Pin | DISPL_DC_Pin | DISPL_LED_Pin,
			GPIO_PIN_RESET);

	/*Configure GPIO pin : B1_Pin */
	GPIO_InitStruct.Pin = B1_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : HeartBeat_Pin MAX6675_CS_Pin */
	GPIO_InitStruct.Pin = HeartBeat_Pin | MAX6675_CS_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pin : DISPL_CS_Pin */
	GPIO_InitStruct.Pin = DISPL_CS_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(DISPL_CS_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : DISPL_RST_Pin DISPL_DC_Pin DISPL_LED_Pin */
	GPIO_InitStruct.Pin = DISPL_RST_Pin | DISPL_DC_Pin | DISPL_LED_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pin : Enc_SW_Pin */
	GPIO_InitStruct.Pin = Enc_SW_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(Enc_SW_GPIO_Port, &GPIO_InitStruct);

	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

	/* USER CODE BEGIN MX_GPIO_Init_2 */
	/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	uint32_t now = HAL_GetTick();
	if ((GPIO_Pin == Enc_SW_Pin) && ((now - lastMenuTriggered)) > encoderFilter)
	{
		selectionMade = true;
		lastMenuTriggered = now;
	} else
	{
		__NOP();
	}
}

void UpdatePWM(uint32_t pwm_amount)
{
	TIM3->CCR1 = pwm_amount;
}
/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartMainLoop */
/**
 * @brief  Function implementing the MainLoop thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartMainLoop */
void StartMainLoop(void *argument)
{
	/* USER CODE BEGIN 5 */

	/* Init menu item nav links*/

	DisplayHomeScreen();
	targetTime =
			1200;
	status = OFF;
	dirty = true;

	/* Infinite loop */
	for (;;)
	{
		/***************** Manage the machine states ************************/

		switch (status)
		{
			case OFF:
				preheatedLatch = false;
				timedOutLatch = false;
				elapsedTime = 0;

				break;
			case ON:
				if (!preheatedLatch)
				{
					status = PRE_HEATING;
				}
				if (elapsedTime >= targetTime)
				{
					status = TIMED_OUT;
				}

				break;
			case PRE_HEATING:
				if (temperature >= targetTemperature)
				{
					preheatedLatch = true;
					status = ON;
				}

				break;
			case TIMED_OUT:
				if (!timedOutLatch)
				{
					timedOutLatch = true;
					elapsedTime = 0;
				}

				break;
			default:
				break;
		}

		/*------------------------------------------------------------------*/

		/*Change to the requested menu or home screen*/
		if (selectionMade)
		{
			if (currentMenu != NULL)	//Not on home screen, do the menu action
			{
				ST7735_FillRectangle(0, 0, 160, 128, BLACK);
				currentMenuItem->DoAction();
				selectionMade = false;
			} else						//On home screen, change to main menu
			{
				ST7735_FillRectangle(0, 0, 160, 128, BLACK);
				currentMenu = &mainMenu;
				MenuItem **firstItem = (MenuItem**) currentMenu->items;
				displayMenu(*firstItem, currentMenu->lenMenu);
				selectionMade = false;
			}
		}

		if (navUp)
		{
			if (currentMenu != NULL && (currentMenuItem->next != NULL))
			{
				currentMenuItem->selected = false;
				currentMenuItem = currentMenuItem->next;
				currentMenuItem->selected = true;
				MenuItem **firstItem = (MenuItem**) currentMenu->items;
				displayMenu(*firstItem, currentMenu->lenMenu);
			}
			navUp = false;
		}
		if (navDown)
		{
			if ((currentMenu != NULL) && (currentMenuItem->prev != NULL))
			{
				currentMenuItem->selected = false;
				currentMenuItem = currentMenuItem->prev;
				currentMenuItem->selected = true;
				MenuItem **firstItem = (MenuItem**) currentMenu->items;
				displayMenu(*firstItem, currentMenu->lenMenu);
			}
			navDown = false;
		}
		osDelay(100);						//Play nice and share
		if (currentMenu == NULL)
		{
			UpdateOutputDisplay(PID_output_percent, dirty);
			UpdateStatus(status, dirty);
			UpdateTemperatures(temperature, targetTemperature,
					temperature_dirty);
			UpdateTimes(elapsedTime, targetTime, dirty);
			dirty = false;
		}
	}
	/* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartReadTemperatuer */
/**
 * @brief Function implementing the ReadTemperature thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartReadTemperatuer */
void StartReadTemperatuer(void *argument)
{
	/* USER CODE BEGIN StartReadTemperatuer */
	/* Infinite loop */
	float sum_of_temperatures = 0;
	uint8_t nReads = 0;

	set_PID_constants(kp, ki, kd);
	set_target_temperature(targetTemperature);
	bool init = true;

	for (;;)
	{

		osDelay(500);
		temperature = Max6675_Read_Temp();
		if (init)
		{
			lastTemperature = temperature;
			init = false;
		}

		/*If temperature reasonable add to sum and increment nReads*/
		if ((temperature < (lastTemperature + 50)) &&
				(temperature > (lastTemperature - 50)))
		{
			sum_of_temperatures += temperature;
			nReads++;
		}
		if (nReads == 10)
		{

			temperature = sum_of_temperatures / nReads;
			set_current_temperature(temperature);
			if (temperature != lastTemperature)
			{
				temperature_dirty = true;
				lastTemperature = temperature;
			}
			nReads = 0;
			sum_of_temperatures = 0;
			if (!(status == AUTO_TUNE))
			{
				/*Update the PID output*/
				if ((status == ON) || (status == PRE_HEATING))
				{
					PID_output = get_pid_output();
				} else
				{
					PID_output = 0;
				}
				UpdatePWM(PID_output);
				PID_output_percent = PID_output / 10;
				if (currentMenu == NULL)
				{
					UpdateOutputDisplay(PID_output_percent, true);
					UpdateTemperatures(temperature, targetTemperature, true);
				}
			}
		}
	}
	/* USER CODE END StartReadTemperatuer */
}

/* USER CODE BEGIN Header_StartPollEncoder */
/**
 * @brief Function implementing the PollEncoder thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartPollEncoder */
void StartPollEncoder(void *argument)
{
	/* USER CODE BEGIN StartPollEncoder */
	/* Infinite loop */
	int8_t direction = 0;
//	lastEncoderValue = ENCODER_VALUE;
	for (;;)
	{
		direction = PollEncoder();
		if (direction == ENCODER_DOWN)
			navDown = true;
		if (direction == ENCODER_UP)
			navUp = true;
		osDelay(50);
	}
	/* USER CODE END StartPollEncoder */
}

/* USER CODE BEGIN Header_StartSerialOut */
/**
 * @brief Function implementing the SerialOut thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartSerialOut */
void StartSerialOut(void *argument)
{
	/* USER CODE BEGIN StartSerialOut */
	/* Infinite loop */
	for (;;)
	{
		if (!(status == AUTO_TUNE))
		{
//			printf("Temperature : %f\n\r", temperature);
//			printf("PID_output : %lu\n\r", PID_output);
//			printf("ENCODER_VALUE = %d\n\r", (int) ENCODER_VALUE);
//			printf("lastEncoderValue = %d\n\r", lastEncoderValue);
			osDelay(1000);
		}
	}
	/* USER CODE END StartSerialOut */
}

/**
 * @brief  Period elapsed callback in non blocking mode
 * @note   This function is called  when TIM10 interrupt took place, inside
 * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
 * a global variable "uwTick" used as application time base.
 * @param  htim : TIM handle
 * @retval None
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	/* USER CODE BEGIN Callback 0 */

	/* USER CODE END Callback 0 */
	if (htim->Instance == TIM10)
	{
		HAL_IncTick();
	}
	/* USER CODE BEGIN Callback 1 */

	/* USER CODE END Callback 1 */
}

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
