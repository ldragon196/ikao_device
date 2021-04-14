/*
 * I2C.c
 *
 *  Created on: July 6, 2020
 *      Author: LongHD
 */
/******************************************************************************/

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include "I2C.h"

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/

/**< TWI (I2C) instance index */
#define TWI_INSTANCE_ID                          1

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/


static const nrf_drv_twi_t i2c_ins = NRF_DRV_TWI_INSTANCE(TWI_INSTANCE_ID);

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/



/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/



/******************************************************************************/

/**
 * @func   I2C_Init
 * @brief  Init I2C
 * @param  None
 * @retval None
 */

void I2C_Init(void){
	// Config spi
	const nrf_drv_twi_config_t i2c_config = {
		.scl                = I2C_SCL_PIN,
		.sda                = I2C_SDA_PIN,
		.frequency          = NRF_DRV_TWI_FREQ_100K,
		.interrupt_priority = APP_IRQ_PRIORITY_LOW,
		.clear_bus_init     = false
	};
	APP_ERROR_CHECK(nrf_drv_twi_init(&i2c_ins, &i2c_config, NULL, NULL));
	
	nrf_drv_twi_enable(&i2c_ins);
}

/**
 * @func   I2C_Scanner
 * @brief  I2C scan
 * @param  None
 * @retval Slave ID
 */

uint8_t I2C_Scanner(void){
	ret_code_t err_code;
	uint8_t address;
	uint8_t readData;
	
	for (address = 1; address <= 0x7F; address++)
	{
		err_code = nrf_drv_twi_rx(&i2c_ins, address, &readData, sizeof(readData));
		if (err_code == NRF_SUCCESS){
			return address;
		}
	}
	return 0;
}	

/**
 * @func   I2C_I2C_ReadMulti
 * @brief  Read multiple registers
 * @param  Slave id, start register address, data and size
 * @retval None
 */

bool I2C_ReadMulti(uint8_t i2c_id, uint8_t slaveAdd, uint8_t address, uint8_t *data, uint8_t size){
	ret_code_t err_code;
	err_code = nrf_drv_twi_tx(&i2c_ins, slaveAdd >> 1, &address, 1, true);
	if (err_code != NRF_SUCCESS){
		return false;
	}
	
	err_code = nrf_drv_twi_rx(&i2c_ins, slaveAdd >> 1, data, size);
	if (err_code != NRF_SUCCESS){
		return false;
	}
	return true;
}

/**
 * @func   I2C_WriteMulti
 * @brief  Write multiple registers
 * @param  Slave id, start register address, data and size
 * @retval None
 */

bool I2C_WriteMulti(uint8_t i2c_id, uint8_t slaveAdd, uint8_t regAddress, uint8_t *data, uint8_t size){
	ret_code_t err_code;
	uint8_t txBuf[16]; 
	
	txBuf[0] = regAddress;
	memcpy(&txBuf[1], data, size);
	
	err_code = nrf_drv_twi_tx(&i2c_ins, slaveAdd >> 1, txBuf, size + 1, false);
	if (err_code != NRF_SUCCESS){
		return false;
	}
	return true;
}








