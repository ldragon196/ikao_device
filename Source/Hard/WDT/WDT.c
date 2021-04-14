/*
 * WDT.c
 *
 *  Created on: July 6, 2020
 *      Author: LongHD
 */
/******************************************************************************/

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include "WDT.h"

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



/******************************************************************************/

/**
 * @func   WDT_Init
 * @brief  Init watchdog
 * @param  None
 * @retval None
 */

void WDT_Init(void){
	NRF_WDT->CONFIG = (WDT_CONFIG_HALT_Pause << WDT_CONFIG_HALT_Pos) | ( WDT_CONFIG_SLEEP_Run << WDT_CONFIG_SLEEP_Pos);   //Configure Watchdog. a) Pause watchdog while the CPU is halted by the debugger.  b) Keep the watchdog running while the CPU is sleeping.
	NRF_WDT->CRV = 3 * 32768;             // timeout [s] = ( CRV + 1 ) / 32768
	NRF_WDT->RREN |= WDT_RREN_RR0_Msk;  // Enable reload register 0
	NRF_WDT->TASKS_START = 1;           // Start the Watchdog timer
}

void WDT_Feed(void){
	NRF_WDT->RR[0] = WDT_RR_RR_Reload;  //Reload watchdog register 0
}














