/*
 * Event.h
 *
 *  Created on: June 29, 2020
 *      Author: LongHD
 */
/******************************************************************************/

#ifndef _SOURCE_HARD_EVENT_EVENT_H_
#define _SOURCE_HARD_EVENT_EVENT_H_

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include "Config.h"

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/

#define MAX_EVENT_CONTROL                              (8u)
#define EVENT_TIMER_MAX_VALUE                          0xFFFFFFFF

#define EVENT_LOOP_TICK_MS                             APP_TIMER_TICKS(10)
#define TIME_DELAY_MS(a)                               ((a) / 10)

typedef uint8_t EventStatus;
enum{
    EVENT_UNUSED = 0x00,
    EVENT_INACTIVE = 0x01,
    EVENT_ACTIVE = 0x02,
};

enum{
    EVENT_SUCCESS = 0,
    EVENT_FAILURE,
    EVENT_INVALID_INIT,
};

enum{
	EVENT_ACTIVE_IDLE = 0,
	EVENT_ACTIVE_RUNNING,
};



typedef uint8_t EventData;
typedef void (*EventHandler)(void);


typedef struct{
    EventStatus status;
    uint32_t timeStart;
    uint32_t timeExcute;
    EventHandler function;
} EventParam_t;

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/



/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/



/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/

void EVENT_Init(void);
void EVENT_Task(void);
uint8_t EVENT_Creat(EventData *event_id, EventHandler func);
void EVENT_SetDelayMS(EventData event_id, uint32_t delay);
void EVENT_SetActive(EventData event_id);
void EVENT_SetInactive(EventData event_id);
uint32_t EVENT_GetTimeRemain(EventData event_id);


/******************************************************************************/

#endif /* _SOURCE_HARD_EVENT_EVENT_H_ */
