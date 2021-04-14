/*
 * BQ27542G1.h
 *
 *  Created on: September 7, 2020
 *      Author: LongHD
 */
/******************************************************************************/

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include "Hard/I2C/I2C.h"

#include "BQ27542G1.h"

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

static bool BQ27542G1_Read(uint8_t command, uint8_t *data, uint8_t length);
static bool BQ27542G1_Write(uint8_t command, uint8_t *data, uint8_t length);

/******************************************************************************/

static bool BQ27542G1_Read(uint8_t command, uint8_t *data, uint8_t length){
	return I2C_ReadMulti(BQ27542G1_I2C, BQ27542G1_SLAVE_ID, command, data, length);
}

static bool BQ27542G1_Write(uint8_t command, uint8_t *data, uint8_t length){
	return I2C_WriteMulti(BQ27542G1_I2C, BQ27542G1_SLAVE_ID, command, data, length);
}

/******************************************************************************/

/**
* @brief  : Read command data. See 9.5.1 Standard Data Commands in datasheet https://www.ti.com/lit/ds/symlink/bq27542-g1.pdf?ts=1607002352329
* @param  : Command
* @retval : Value
*/

uint16_t BQ27542G1_ReadWord(uint8_t command){
	uint16_t retVal;

	if(!BQ27542G1_Read(command, (uint8_t*) &retVal, 2)){
		return 0;
	}
	
	return retVal;
}

/**
* @brief  : Check device type. BQ27542G1 : 0x0542
* @param  : None
* @retval : True, False
*/

bool BQ27542G1_CheckDeviceType(void){
	uint16_t subCmd = BQ27542G1_CNTLDATA_DEVICE_TYPE;
	if( !BQ27542G1_Write(BQ27542G1_CMD_CNTL, (uint8_t*) &subCmd, 2) ){
		return false;
	}
	
	uint16_t deviceType;
	if( !BQ27542G1_Read(BQ27542G1_CMD_CNTL, (uint8_t*) &deviceType, 2) ){
		return false;
	}
	
	return deviceType == BQ27542G1_DEVICE_TYPE;
}

/******************************************************************************/

/**
* @brief  : Check device type. BQ27542G1 : 0x0542
* @param  : None
* @retval : True, False
*/

uint8_t BQ25892_ChargeState(void){
	uint8_t readData = 0;
	uint8_t retVal = CHARGE_STATUS_NONE;
	
	if(I2C_ReadMulti(BQ25892_I2C, BQ25892_SLAVE_ID, BQ25892_CHARGE_STATUS_REG, &readData, 1)){
		/* Bit 3, 4 */
		if(((readData >> 3) & 0x03) == 0x03){
			/* Done */
			retVal = CHARGE_STATUS_DONE;
		}
		else if(((readData >> 3) & 0x03) == 0x00){
			retVal = CHARGE_STATUS_NONE;
		}
		else{
			retVal = CHARGE_STATUS_CHARGING;
		}
	}
	return retVal;
}









