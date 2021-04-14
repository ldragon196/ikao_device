/*
 * Buzz.c
 *
 *  Created on: July 1, 2020
 *      Author: LongHD
 */
/******************************************************************************/

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include "Hard/Event/Event.h"

#include "Buzz.h"

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/

#define BUZZER(a)                     nrf_gpio_pin_write(BUZZER_PIN, (a));

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

EventData buzzEventControl;

static uint32_t buzzCount = 0, timeBip = BIP_FAST;

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/



/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/

void buzzEventFunction(void);

/******************************************************************************/

/**
* @func   BUZZ_Init
* @brief  Init buzzer
* @param  None
* @retval None
*/

void BUZZ_Init(void){
	nrf_gpio_cfg_output(BUZZER_PIN);
	BUZZER(BUZZ_OFF);
	
	EVENT_Creat(&buzzEventControl, buzzEventFunction);
}

/**
* @func   buzzEventFunction
* @brief  Event for turn on buzz
* @param  None
* @retval None
*/

void buzzEventFunction(void){
	EVENT_SetInactive(buzzEventControl);
	
	if((buzzCount % 2) == 0){
		BUZZER(BUZZ_OFF);
	}
	else{
		BUZZER(BUZZ_ON);
	}
	
	if(buzzCount > 0){
		buzzCount--;
		EVENT_SetDelayMS(buzzEventControl, TIME_DELAY_MS(timeBip));
	}
}

/**
* @func   BUZZ_TurnOn
* @brief  Turn on buzzer n bips
* @param  Times bip
* @retval None
*/

void BUZZ_TurnOn(uint32_t times, uint32_t time){
	buzzCount = (times << 1) - 1;
	timeBip = time;
	
	EVENT_SetDelayMS(buzzEventControl, TIME_DELAY_MS(100));
}

