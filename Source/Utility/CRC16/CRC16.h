/*
 *  crc16.h
 *
 *  Created on: March 22, 2020
 *      Author: LongHD
 */

#ifndef UTILITY_CRC16_CRC16_H
#define UTILITY_CRC16_CRC16_H

/******************************************************************************/

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include <stdint.h>

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

uint16_t CRC16_Modbus(uint8_t* data, uint8_t length);
uint16_t CRC16_Kermit(uint8_t *data, uint8_t length);

/******************************************************************************/

#endif /* UTILITY_CRC16_CRC16_H */

