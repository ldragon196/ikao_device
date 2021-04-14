/*
 * Timer.c
 *
 *  Created on: July 1, 2020
 *      Author: LongHD
 */
/******************************************************************************/

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include "nrf_drv_timer.h"

#include "Timer.h"

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/



/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

const nrf_drv_timer_t TIMER_RFID = NRF_DRV_TIMER_INSTANCE(2);

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/



/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/

static void TIMER_EventHandler(nrf_timer_event_t event_type, void* p_context);

/******************************************************************************/

/**
* @func   TIMER_EventHandler
* @brief  Must != NULL to init timer
* @param  None
* @retval None
*/

static void TIMER_EventHandler(nrf_timer_event_t event_type, void* p_context){
	// Do nothing
}

/**
* @func   TIMER_Init
* @brief  Init timer to calculator pulse width
* @param  None
* @retval None
*/

void TIMER_Init(void){
	uint32_t err_code = NRF_SUCCESS;
	
	nrf_drv_timer_config_t timer_cfg = NRF_DRV_TIMER_DEFAULT_CONFIG;
	timer_cfg.frequency = NRF_TIMER_FREQ_1MHz;        // <4=> 1 MHz 
	timer_cfg.bit_width = NRF_TIMER_BIT_WIDTH_32;     // < Timer bit width 32 bit
	
	err_code = nrf_drv_timer_init(&TIMER_RFID, &timer_cfg, TIMER_EventHandler);
    APP_ERROR_CHECK(err_code);
	
	nrf_drv_timer_enable(&TIMER_RFID);
}

/**
* @func   TIMER_US_GetCurrentTime
* @brief  Get timer count. Frequence = 1M => 1count = 1us
* @param  None
* @retval Counter
*/

uint32_t TIMER_US_GetCurrentTime(void){
    NRF_TIMER2->TASKS_CAPTURE[0] = 1;
	return NRF_TIMER2->CC[0];
}

/**
* @func   TIMER_US_GetElapseTime
* @brief  Time from time_start to now
* @param  Time_start
* @retval Elapse time
*/

uint32_t TIMER_US_GetElapseTime(uint32_t time_start){
    return ((TIMER_US_GetCurrentTime() - time_start) & TIMER_2_MAX_VALUE);
}





