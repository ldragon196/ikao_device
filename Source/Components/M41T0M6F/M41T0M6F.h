/*
 * M41T0M6F.h
 *
 *  Created on: July 22, 2020
 *      Author: LongHD
 */
/******************************************************************************/

#ifndef _SOURCE_COMPONENTS_M41T0M6F_M41T0M6F_H_
#define _SOURCE_COMPONENTS_M41T0M6F_M41T0M6F_H_

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include <stdint.h>

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/

// 7 bits address + 1 bit is R/W
#define M41T0M6F_SLAVE_ID                          0xD0
#define M41T0M6F_SECONDS_REG                       0x00
#define M41T0M6F_MINUTES_REG                       0x01
#define M41T0M6F_HOURS_REG                         0x02
#define M41T0M6F_DAY_REG                           0x03
#define M41T0M6F_DATE_REG                          0x04
#define M41T0M6F_MONTH_REG                         0x05
#define M41T0M6F_YEARS_REG                         0x06
#define M41T0M6F_CONTROL_REG                       0x07

#define M41T0M6F_I2C                               0

/**
 * Struct for date/time
 *
 * Parameters:
 *     - uint8_t seconds:
 *         Seconds parameter, from 00 to 59
 *     - uint8_t minutes:
 *         Minutes parameter, from 00 to 59
 *     - uint8_t hours:
 *         Hours parameter, 24Hour mode, 00 to 23
 *     - uint8_t day:
 *         Day in a week, from 1 to 7
 *     - uint8_t date:
 *         Date in a month, 1 to 31
 *     - uint8_t month:
 *         Month in a year, 1 to 12
 *     - uint8_t year:
 *         Year parameter, 00 to 99, 00 is 2000 and 99 is 2099
 */

typedef struct _DATE_TIME_{
	uint8_t Seconds;
	uint8_t Minutes;
	uint8_t Hours;
	uint8_t Day;
	uint8_t Date;
	uint8_t Month;
	uint8_t Years;
} DateTime_t;

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/



/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/



/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/

void M41T0M6F_GetTime(DateTime_t *dateTime);
void M41T0M6F_SetTime(DateTime_t *dateTime);

/******************************************************************************/

#endif /* _SOURCE_COMPONENTS_M41T0M6F_M41T0M6F_H_ */
