/*
 * M41T0M6F.h
 *
 *  Created on: July 22, 2020
 *      Author: LongHD
 */
/******************************************************************************/

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include "Hard/I2C/I2C.h"

#include "M41T0M6F.h"

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/



/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/



/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/



/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/

static uint8_t M41T0M6F_BCDToBin(uint8_t bcd);
static uint8_t M41T0M6F_BinToBCD(uint8_t bin);
static uint8_t M41T0M6F_ValidRange(uint8_t val, uint8_t min, uint8_t max);

/******************************************************************************/

/**
 * @func   M41T0M6F_BCDToBin
 * @brief  Convert BCD format to Binary format
 * @param  BCD value
 * @retval None
 */

static uint8_t M41T0M6F_BCDToBin(uint8_t bcd){
	uint8_t retVal = 10 * (bcd >> 4);
	retVal += bcd & 0x0F;
	return retVal;
}

/**
 * @func   M41T0M6F_BcdToBin
 * @brief  Convert Binary format to BCD format 
 * @param  Bin value
 * @retval None
 */

static uint8_t M41T0M6F_BinToBCD(uint8_t bin){
	uint8_t low = 0;
	uint8_t high = 0;
	
	/* High nibble */
	high = bin / 10;
	/* Low nibble */
	low = bin - (high * 10);
	
	/* Return */
	return high << 4 | low;
}

/**
 * @func   M41T0M6F_ValidRange
 * @brief  Valid value in range min -> max
 * @param  Max and min value
 * @retval Value
 */

static uint8_t M41T0M6F_ValidRange(uint8_t val, uint8_t min, uint8_t max){
	if (val < min) {
		return min;
	} else if (val > max) {
		return max;
	}
	return val;
}

/**
 * @func   M41T0M6F_GetTime
 * @brief  Get date time
 * @param  Date Time output
 * @retval None
 */
 
void M41T0M6F_GetTime(DateTime_t *dateTime){
	uint8_t data[7] = {0};

	/* Read multi bytes */
	I2C_ReadMulti(M41T0M6F_I2C, M41T0M6F_SLAVE_ID, M41T0M6F_SECONDS_REG, data, 7);
	
	/* Fill data */
	dateTime->Seconds = M41T0M6F_BCDToBin(data[M41T0M6F_SECONDS_REG] & 0x7F);  // 7 bits
	dateTime->Minutes = M41T0M6F_BCDToBin(data[M41T0M6F_MINUTES_REG] & 0x7F);  // 7 bits
	dateTime->Hours = M41T0M6F_BCDToBin(data[M41T0M6F_HOURS_REG] & 0x3F);      // 6 bits
	dateTime->Day = M41T0M6F_BCDToBin(data[M41T0M6F_DAY_REG] & 0x03);          // 3 bits
	dateTime->Date = M41T0M6F_BCDToBin(data[M41T0M6F_DATE_REG] & 0x3F);        // 6 bits
	dateTime->Month = M41T0M6F_BCDToBin(data[M41T0M6F_MONTH_REG] & 0x1F);      // 5 bits
	dateTime->Years = M41T0M6F_BCDToBin(data[M41T0M6F_YEARS_REG] & 0xFF);      // 8 bits
}

/**
 * @func   M41T0M6F_SetTime
 * @brief  Set date time
 * @param  Date Time output
 * @retval None
 */
 
void M41T0M6F_SetTime(DateTime_t *dateTime){
	uint8_t data[7] = {0};

	/* Format data */
	data[M41T0M6F_SECONDS_REG] = M41T0M6F_BinToBCD(M41T0M6F_ValidRange(dateTime->Seconds, 0, 59));
	data[M41T0M6F_MINUTES_REG] = M41T0M6F_BinToBCD(M41T0M6F_ValidRange(dateTime->Minutes, 0, 59));
	data[M41T0M6F_HOURS_REG] = M41T0M6F_BinToBCD(M41T0M6F_ValidRange(dateTime->Hours, 0, 23));
	data[M41T0M6F_DAY_REG] = M41T0M6F_BinToBCD(M41T0M6F_ValidRange(dateTime->Day, 1, 7));
	data[M41T0M6F_DATE_REG] = M41T0M6F_BinToBCD(M41T0M6F_ValidRange(dateTime->Date, 1, 31));
	data[M41T0M6F_MONTH_REG] = M41T0M6F_BinToBCD(M41T0M6F_ValidRange(dateTime->Month, 1, 12));
	data[M41T0M6F_YEARS_REG] = M41T0M6F_BinToBCD(M41T0M6F_ValidRange(dateTime->Years, 0, 99));

	/* Write multi bytes */
	I2C_WriteMulti(M41T0M6F_I2C, M41T0M6F_SLAVE_ID, M41T0M6F_SECONDS_REG, data, 7);
}


