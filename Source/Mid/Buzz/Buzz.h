/*
 * Buzz.h
 *
 *  Created on: Jule 1, 2020
 *      Author: LongHD
 */
/******************************************************************************/

#ifndef _SOURCE_MID_BUZZ_BUZZ_H_
#define _SOURCE_MID_BUZZ_BUZZ_H_

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include "Config.h"

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/

#define BUZZ_ON                           HIGH
#define BUZZ_OFF                          LOW

#define BIP_FAST                          100          // 100ms
#define BIP_MEDIUM                        300          // 200ms
#define BIP_LOW                           1000         // 1000ms

#define BIP()                             BUZZ_TurnOn(1, BIP_FAST)
#define BIPBIP()                          BUZZ_TurnOn(2, BIP_FAST)
#define BIPBIPBIP()                       BUZZ_TurnOn(3, BIP_FAST)

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/



/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/



/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/

void BUZZ_Init(void);
void BUZZ_TurnOn(uint32_t times, uint32_t time);

/******************************************************************************/

#endif /* _SOURCE_MID_BUZZ_BUZZ_H_ */
