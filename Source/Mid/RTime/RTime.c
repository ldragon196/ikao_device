/*
 * RTime.c
 *
 *  Created on: July 8, 2020
 *      Author: LongHD
 */
/******************************************************************************/

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include <Time.h>
#include "Hard/Event/Event.h"
#include "Mid/LCD/LCD.h"

#include "Components/M41T0M6F/M41T0M6F.h"
#include "RTime.h"

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/



/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

static volatile uint32_t eventTimestamp = 1594184400;
static uint32_t updateTimeCount = 0;
EventData realTimeEventControl;

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/

#include "Mid/Buzz/Buzz.h"

/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/

static void realTimeEventFunction(void);
static void CalibCurrentTime(void);

/******************************************************************************/

/**
* @func   realTimeEventFunction
* @brief  Loop for timestamp
* @param  
* @retval none
*/

static void realTimeEventFunction(void){
	EVENT_SetInactive(realTimeEventControl);
	
	eventTimestamp++;
	updateTimeCount++;
	if(updateTimeCount >= 30){
		updateTimeCount = 0;
		CalibCurrentTime();
	}
	
	EVENT_SetDelayMS(realTimeEventControl, TIME_TICK_1S);
}

/**
* @func   TIME_Init
* @brief  Init event for increasing timestamp
* @param  None
* @retval None
*/

void TIME_Init(void){
	CalibCurrentTime();
	
	EVENT_Creat(&realTimeEventControl, realTimeEventFunction);
	EVENT_SetDelayMS(realTimeEventControl, TIME_TICK_1S);
}

/**
 * @func   CalibCurrentTime
 * @brief  Read time from RTC chip and calib current timestamp
 * @param  None
 * @retval None
 */

static void CalibCurrentTime(void){
	DateTime_t dateTime = {0};
	time_t epoch;
	
	M41T0M6F_GetTime(&dateTime);
	
	struct tm tm;
	tm.tm_sec = dateTime.Seconds;
	tm.tm_min = dateTime.Minutes;
	tm.tm_hour = dateTime.Hours;
	tm.tm_mday = dateTime.Date;
	tm.tm_mon = dateTime.Month - 1;
	tm.tm_year = dateTime.Years + 2000 - 1900;
	
	epoch = mktime(&tm);
	if(epoch != 0xFFFFFFFF){
		eventTimestamp = epoch;
	}
}

/**
 * @func   TIME_GetTimestamp/TIME_SetTimestamp
 * @brief  Get/set timestamp
 * @param  None
 * @retval None
 */

uint32_t TIME_GetTimestamp(void){
	return eventTimestamp;
}

void TIME_SetTimestamp(uint32_t setTime){
	DateTime_t dateTime;
	struct tm* localtm = localtime(&setTime);
	
	dateTime.Years = localtm->tm_year - (2000 - 1900);
	dateTime.Month = localtm->tm_mon + 1;
	dateTime.Date = localtm->tm_mday;
	dateTime.Day = localtm->tm_wday + 1;
	dateTime.Hours = localtm->tm_hour;
	dateTime.Minutes = localtm->tm_min;
	dateTime.Seconds = localtm->tm_sec;
	M41T0M6F_SetTime(&dateTime);
	
	eventTimestamp = setTime;
}










