/*
 * CRN_Flash.c
 *
 *  Created on: May 16, 2023
 *      Author: Charles
 */

#ifndef INC_CRN_FLASH_C_
#define INC_CRN_FLASH_C_

#define CRN_FLASH_SECTORCOUNT  8
#include <stdbool.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"

//--------------------
//CRN_Flash_SectorData
typedef struct
{

	uint32_t uSize;
	uint32_t uBase;

	void *pData;

	bool bLocked;

} CRN_Flash_SectorData;

typedef enum uint8_t
{
	FLASH_OK = 0,                      //Function has succeeded
	FLASH_Fail,                 //Function has failed, with a non-specific error
	FLASH_Error_PeriphBusy, //Function has not been able to initialize a particular peripheral as the peripheral is busy
	FLASH_Error_PeriphNotSupported //Function has not been able to initialize a particular peripheral as the peripheral doesn't support the required functionality
} FlashResults;

CRN_Flash_SectorData* initCRNFlash();
FlashResults eraseSector(uint8_t uSector);
FlashResults eraseAndWriteSector(uint8_t uSector, uint8_t *pData,
		uint32_t uSize);
FlashResults writeSector(uint8_t uSector, uint32_t uOffset, uint8_t *pData,
		uint32_t uSize);

#endif /* INC_CRN_FLASH_C_ */
