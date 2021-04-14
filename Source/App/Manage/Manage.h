/*
 * Manage.h
 *
 *  Created on: July 3, 2020
 *      Author: LongHD
 */

#ifndef SOURCE_APP_MANAGE_MANAGE_H_
#define SOURCE_APP_MANAGE_MANAGE_H_

/******************************************************************************/


/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include "Config.h"

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/

#define BLE_START_MESSAGE_BYTE                       '{'
#define BLE_END_MESSAGE_BYTE                         '}'

#define BLE_TAG_OK_STRING                            "tag:0"
#define BLE_TAG_OK_LENGTH                            5
#define BLE_INFO_RES_STRING                          ".txt="
#define BLE_INFO_RES_LENGTH                          5
#define BLE_TIMESTAMP_STRING                         "timestamp:"
#define BLE_TIMESTAMP_LENGTH                         10

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/



/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/



/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/

void MANAGE_Init(void);

void MANAGE_HandleTagId(uint8_t* tag);
void MANAGE_RequestInfoApp(uint8_t pageInfo);
void MANAGE_ParseBleReceive(const char* data, uint16_t size);
void MANAGE_HandleEventTime(uint8_t forceShow);
void MANAGE_ShowTag(void);

uint8_t MANAGE_ReadBattery(void);
uint8_t MANAGE_BatteryCharging(void);

/******************************************************************************/

#endif /* SOURCE_APP_MANAGE_MANAGE_H_ */
