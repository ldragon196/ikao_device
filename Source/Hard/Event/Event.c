/*
 * Event.c
 *
 *  Created on: June 29, 2020
 *      Author: LongHD
 */
/******************************************************************************/

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include "Event.h"

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/



/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

_APP_TIMER_DEF(eventLoopId);

static uint8_t eventActiveState = EVENT_ACTIVE_IDLE;
static volatile uint32_t eventTick = 0;

EventParam_t eventControls[MAX_EVENT_CONTROL];

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/



/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/

static void EVENT_TickLoop(void* arg);
static uint32_t EVENT_GetElapseTime(uint32_t start);
static uint8_t EVENT_IsExpiredTime(uint8_t event_id);

/******************************************************************************/

/**
* @func   EVENT_TickLoop
* @brief  Loop for timer tick
* @param  
* @retval None
*/

static void EVENT_TickLoop(void* arg){
	eventTick ++;
	
	// If has pending, run app timer
	if(eventActiveState == EVENT_ACTIVE_RUNNING){
		// Start application timers.
		ret_code_t err_code = app_timer_start(eventLoopId, EVENT_LOOP_TICK_MS, NULL);
		APP_ERROR_CHECK(err_code);
	}
	
	UNUSED(arg);
}

/**
* @func   EVENT_GetElapseTime
* @brief  Time from time_start to now
* @param  Time start
* @retval Elapse time
*/

static uint32_t EVENT_GetElapseTime(uint32_t start){
    return ((eventTick - start) & EVENT_TIMER_MAX_VALUE);
}

/**
 * @func   EVENT_IsExpiredTime
 * @brief  Check time expired of event
 * @param  Event id to check
 * @retval True if time is expired
 */

static uint8_t EVENT_IsExpiredTime(uint8_t event_id){
    uint32_t timeElapse, timeStart;

    // Get time elapse
    timeStart = eventControls[event_id].timeStart;
    timeElapse = EVENT_GetElapseTime(timeStart);
	
    if(timeElapse >= eventControls[event_id].timeExcute){
        return true;
    }
    return false;
}

/**
 * @func   EVENT_Init
 * @brief  Init timer and event. Must init first after power up
 * @param  none
 * @retval none
 */

void EVENT_Init(void){
	ret_code_t err_code;
	
	// Set event control default value
	memset((uint8_t*) eventControls, 0, sizeof(eventControls));
	
	/***************************************************************/
	
	// Create timers.
    err_code = app_timer_create(&eventLoopId,
                                APP_TIMER_MODE_SINGLE_SHOT,
                                EVENT_TickLoop);
    APP_ERROR_CHECK(err_code);
}

/**
 * @func   EVENT_Creat
 * @brief  Register event to schedule
 * @param  event_id output to store event id
 *         init_func function handler when event excute
 * @retval EVENT_SUCCESS if register success
 *         error code in otherwise
 */

uint8_t EVENT_Creat(EventData *event_id, EventHandler func){
    if((func == NULL) || (event_id == NULL) ){
        return EVENT_INVALID_INIT;
    }

    // Check event control available
    for(uint8_t i = 0; i < MAX_EVENT_CONTROL; i++){
        if(eventControls[i].status == EVENT_UNUSED){
            eventControls[i].status = EVENT_INACTIVE;
            eventControls[i].function = func;
            *event_id = i;
            return EVENT_SUCCESS;
        }
    }
    return EVENT_FAILURE;
}

/**
 * @func   EVENT_SetDelayMS
 * @brief  Schedule active timer after delay ms
 * @param  event_id
 *         delay time
 * @retval None
 */

void EVENT_SetDelayMS(EventData event_id, uint32_t delay){
    uint8_t eventId;

    eventId = event_id;
    if(eventId < MAX_EVENT_CONTROL){
        eventControls[eventId].timeStart = eventTick;
        eventControls[eventId].timeExcute = delay;
        eventControls[eventId].status = EVENT_ACTIVE;
		
		if(eventActiveState == EVENT_ACTIVE_IDLE){
			eventActiveState = EVENT_ACTIVE_RUNNING;
			// Start application timers.
			ret_code_t err_code = app_timer_start(eventLoopId, EVENT_LOOP_TICK_MS, NULL);
			APP_ERROR_CHECK(err_code);
		}
    }
}

/**
 * @func   EVENT_SetActive
 * @brief  Active event now
 * @param  event_id
 * @retval None
 */

void EVENT_SetActive(EventData event_id){
	EVENT_SetDelayMS(event_id, 0);
}

/**
 * @func   EVENT_SetInactive
 * @brief  Inactive event now
 * @param  event_id
 * @retval None
 */

void EVENT_SetInactive(EventData event_id){
    uint8_t eventId;

    eventId = event_id;
    if(eventId < MAX_EVENT_CONTROL){
        eventControls[eventId].status = EVENT_INACTIVE;
    }
}

/**
 * @func   EVENT_GetTimeRemain
 * @brief  Get time remaining before event is excuted
 * @param  Event index
 * @retval Time remaining
 *         If event is not active, return 0xFFFFFFFF
 */

uint32_t EVENT_GetTimeRemain(EventData event_id){
    uint8_t eventId;
    uint32_t start, timeElapse;

    eventId = event_id;
    if(eventControls[eventId].status != EVENT_ACTIVE){
        return 0xFFFFFFFF;
    }

    // Get time remaining
    start = eventControls[eventId].timeStart;
    timeElapse = EVENT_GetElapseTime(start);

    if(eventControls[eventId].timeExcute > timeElapse){
        return eventControls[eventId].timeExcute - timeElapse;
    }
    return EVENT_TIMER_MAX_VALUE;
}

/**
 * @func   EVENT_Task
 * @brief  Task to schedule event
 * @param  None
 * @retval None
 */

void EVENT_Task(void){
    uint8_t i, running;
	
	running = false;
    for(i = 0; i < MAX_EVENT_CONTROL; i++){
        if(eventControls[i].status == EVENT_ACTIVE){
			running = true;
            if(EVENT_IsExpiredTime(i) == true){
				if(eventControls[i].function != NULL){
					eventControls[i].function();
				}
            }
        }
    }
	
	// If no event is active, stop timer
	if( (eventActiveState == EVENT_ACTIVE_RUNNING) && (!running) ){
		// Stop application timers.
		ret_code_t err_code = app_timer_stop(eventLoopId);
		APP_ERROR_CHECK(err_code);
		
		eventActiveState = EVENT_ACTIVE_IDLE;
	}
}
