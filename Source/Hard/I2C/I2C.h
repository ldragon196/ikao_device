/*
 * I2C.h
 *
 *  Created on: July 7, 2020
 *      Author: LongHD
 */
/******************************************************************************/

#ifndef _SOURCE_HARD_I2C_I2C_H_
#define _SOURCE_HARD_I2C_I2C_H_

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include "Config.h"

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

void I2C_Init(void);
uint8_t I2C_Scanner(void);
bool I2C_ReadMulti(uint8_t i2c_id, uint8_t slaveAdd, uint8_t address, uint8_t *data, uint8_t size);
bool I2C_WriteMulti(uint8_t i2c_id, uint8_t slaveAdd, uint8_t address, uint8_t *data, uint8_t size);

/******************************************************************************/

#endif /* _SOURCE_HARD_I2C_I2C_H_ */