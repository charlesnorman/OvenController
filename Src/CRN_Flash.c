/*
 * CRN_Flash.c
 *
 *  Created on: May 16, 2023
 *      Author: Charles
 */

#include "CRN_Flash.h"

CRN_Flash_SectorData sectors[CRN_FLASH_SECTORCOUNT];

CRN_Flash_SectorData* initCRNFlash()
{
	//---------------------------
	//Setup Sector Base Addresses
	sectors[0].uBase = 0x08000000;
	sectors[1].uBase = 0x08004000;
	sectors[2].uBase = 0x08008000;
	sectors[3].uBase = 0x0800C000;
	sectors[4].uBase = 0x08010000;
	sectors[5].uBase = 0x08020000;
	sectors[6].uBase = 0x08040000;
	sectors[7].uBase = 0x08060000;

	//------------------
	//Setup Sector Sizes
	sectors[0].uSize = 16384;
	sectors[1].uSize = 16384;
	sectors[2].uSize = 16384;
	sectors[3].uSize = 16384;
	sectors[4].uSize = 65536;
	sectors[5].uSize = 131072;
	sectors[6].uSize = 131072;
	sectors[7].uSize = 131072;

	//---------------------
	//Setup Sector Pointers
	for (uint8_t i = 0; i < CRN_FLASH_SECTORCOUNT; i++)
		sectors[i].pData = (void*) sectors[i].uBase;
	//------------------------
	//Setup Sector Lock Values
	sectors[0].bLocked = true;
	sectors[1].bLocked = false;
	sectors[2].bLocked = true;
	sectors[3].bLocked = true;
	sectors[4].bLocked = true;
	sectors[5].bLocked = true;
	sectors[6].bLocked = true;
	sectors[7].bLocked = true;

	return sectors;

}

FlashResults eraseSector(uint8_t uSector)
{
	//Check sector index
	if (uSector >= CRN_FLASH_SECTORCOUNT)
		return FLASH_Fail;

	//Check if sector is locked
	if (sectors[uSector].bLocked)
		return FLASH_Fail;

	//Erase Sector
	if (HAL_FLASH_Unlock() != HAL_OK)
		return FLASH_Fail;
	__HAL_FLASH_CLEAR_FLAG(
			FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGSERR);
	FLASH_Erase_Sector(uSector, FLASH_VOLTAGE_RANGE_3);
	HAL_FLASH_Lock();

	//Return
	return FLASH_OK;
}

FlashResults eraseAndWriteSector(uint8_t uSector, uint8_t *pData,
		uint32_t uSize)
{
	//Check sector index
	if (uSector >= CRN_FLASH_SECTORCOUNT)
		return FLASH_Fail;

	//Check if sector is locked
	if (sectors[uSector].bLocked)
		return FLASH_Fail;

	//Erase Sector
	if (HAL_FLASH_Unlock() != HAL_OK)
		return FLASH_Fail;
	__HAL_FLASH_CLEAR_FLAG(
			FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGSERR);
	FLASH_Erase_Sector(uSector, FLASH_VOLTAGE_RANGE_3);

	for (uint32_t i = 0; i < uSize; i++)
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, sectors[uSector].uBase + i,
				(uint8_t) pData[i]);

	HAL_FLASH_Lock();

	//Return
	return FLASH_OK;
}

FlashResults writeSector(uint8_t uSector, uint32_t uOffset, uint8_t *pData,
		uint32_t uSize)
{
	//Check sector index
	if (uSector >= CRN_FLASH_SECTORCOUNT)
		return FLASH_Fail;

	//Check if sector is locked
	if (sectors[uSector].bLocked)
		return FLASH_Fail;

	//Check write will not exceed sector size
	if ((uOffset + uSize) >= sectors[uSector].uSize)
		return FLASH_Fail;

	//Erase Sector
	if (HAL_FLASH_Unlock() != HAL_OK)
		return FLASH_Fail;
	__HAL_FLASH_CLEAR_FLAG(
			FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR | FLASH_FLAG_PGSERR);

	uint32_t uBase = sectors[uSector].uBase + uOffset;
	for (uint32_t i = 0; i < uSize; i++)
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, uBase + i,
				(uint8_t) pData[i]);

	HAL_FLASH_Lock();

	//Return
	return FLASH_OK;
}
