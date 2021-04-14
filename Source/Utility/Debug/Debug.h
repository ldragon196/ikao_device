/*
 * debug.h
 *
 *  Created on: May 6, 2020
 *      Author: LongHD
 */

#ifndef SOURCE_UTILITY_DEBUG_DEBUG_H_
#define SOURCE_UTILITY_DEBUG_DEBUG_H_

/******************************************************************************/


/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include <stdint.h>
#include <stdio.h>

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/


#define DISABLE_ALL_DEBUG

#ifdef DISABLE_ALL_DEBUG

#define DEBUG_PRINT(...)                     do {} while(0)
#define DEBUG_PRINTLN(...)                   do {} while(0)
#define DEBUG_PRINT_HEX(a, b)                do {} while(0)
	
#else

#ifdef DEBUG_PRINT_ENABLE

#define DEBUG_PRINT(...)                   do {printf(__VA_ARGS__);} while(0)
#define DEBUG_PRINTLN(...)                   do {printf(__VA_ARGS__); printf("\r\n");} while(0)
#define DEBUG_PRINT_HEX(a, b)                do {DEBUG_PrintHex((a),(b));} while(0)

#else

#define DEBUG_PRINT(...)                     do {} while(0)
#define DEBUG_PRINTLN(...)                   do {} while(0)
#define DEBUG_PRINT_HEX(a, b)                do {} while(0)

#endif
	
#endif



/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/



/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/



/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/

void DEBUG_PrintHex(uint8_t *data, uint16_t length);

/******************************************************************************/

#endif /* SOURCE_UTILITY_DEBUG_DEBUG_H_ */
