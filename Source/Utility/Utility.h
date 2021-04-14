/*
 * Utility.h
 *
 *  Created on: June 19, 2020
 *      Author: LongHD
 */

#ifndef _SOURCE_UTILITY_H_
#define _SOURCE_UTILITY_H_

/******************************************************************************/


/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include <stdint.h>
#include <stdio.h>
#include <string.h>

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/

#define GET_BIT_0_7(a)          ((a)&0xFF)
#define GET_BIT_8_15(a)         (((a)>>8)&0xFF)
#define GET_BIT_16_23(a)        (((a)>>16)&0xFF)
#define GET_BIT_24_31(a)        (((a)>>24)&0xFF)

#ifndef HIGH_UINT16
#define HIGH_UINT16(a)          (((a) >> 8) & 0xFF)
#define LOW_UINT16(a)           ((a) & 0xFF)
#define HIGH_UINT8(a)           (((a) >> 4) & 0x0F)
#define LOW_UINT8(a)            ((a) & 0x0F)
#endif /* HI_UINT16 */

#define MERGE16U(a,b)           ((((a) << 8) & 0xFF00) | ((b) & 0x00FF))
#define MERGE32U(a,b,c,d)       ((((a) << 24) & 0xFF000000) | (((b) << 16) & 0x00FF0000) | (((c) << 8) & 0x0000FF00) | ((d) & 0x000000FF))

#define UNUSED(a)               (void)(a)

typedef uint8_t boolean;

#ifndef TRUE
#define TRUE                    1
#define FALSE                   0
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



/******************************************************************************/

#endif /* _SOURCE_UTILITY_H_ */