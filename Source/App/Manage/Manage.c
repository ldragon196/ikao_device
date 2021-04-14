/*
 * Manage.c
 *
 *  Created on: July 3, 2020
 *      Author: LongHD
 */

/******************************************************************************/


/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include <stdlib.h>

#include "Components/BQ27542G1/BQ27542G1.h"
#include "Mid/LCD/LCD.h"
#include "Mid/MyBLE/MyBLE.h"
#include "Mid/RTime/RTime.h"
#include "Mid/Buzz/Buzz.h"
#include "Manage.h"

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/



/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

static char sendDataBuffer[32];
static uint64_t tagConvert = 0;            // Tag id
static uint32_t manufacture = 0;           // Manufacture code

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/



/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/



/******************************************************************************/

/**
* @func   : MANAGE_Init
* @brief  : 
* @param  : None
* @retval : None
*/

void MANAGE_Init(void){
	TIME_Init();
}

/**
* @func   : MANAGE_HandleEventTime
* @brief  : Sync and show time
* @param  : None
* @retval : None
*/

void MANAGE_HandleEventTime(uint8_t forceShow){
	uint32_t timestamp = TIME_GetTimestamp();
	if(forceShow){
		LCD_ForceShowDateTime(timestamp);
	}
	else{
		LCD_ShowDateTime(timestamp);
	}
}

/**
* @func   : MANAGE_HandleTagId
* @brief  : Save tag id to flash, show LCD and request info via bluetooth
* @param  : Tag id
* @retval : None
*/

void MANAGE_HandleTagId(uint8_t* tag){
	uint8_t* ptag;
	
	// 38 ID bits
	ptag = (uint8_t*) &tagConvert;
	ptag[0] = tag[0];
	ptag[1] = tag[1];
	ptag[2] = tag[2];
	ptag[3] = tag[3];
	ptag[4] = tag[4] & 0x3F;
	
	// 10 Country bits
	manufacture = ((tag[4] & 0xC0) >> 6) + (tag[5] << 2);
	if( (manufacture == 0) && (tagConvert == 0) ){
		return;
	}
	
	MANAGE_ShowTag();
	
	sprintf(sendDataBuffer, "{tag:%03u%012ju}", manufacture, tagConvert);
	BLE_SendData((uint8_t*) sendDataBuffer, strlen(sendDataBuffer));
}

/**
* @func   : MANAGE_ShowTag
* @brief  : Show tag id
* @param  : None
* @retval : None
*/

void MANAGE_ShowTag(void){
	if((manufacture != 0) && (tagConvert != 0)){
	sprintf(sendDataBuffer, "%03u%012ju", manufacture, tagConvert);
	LCD_ShowTag(sendDataBuffer);
	}
	sprintf(sendDataBuffer, "%03u%012ju", manufacture, tagConvert);
	LCD_ShowTag(sendDataBuffer);
}

/**
* @func   : MANAGE_ReadBattery
* @brief  : 
* @param  : None
* @retval : None
*/

uint8_t MANAGE_ReadBattery(void){
	uint8_t percent = (uint8_t) BQ27542G1_ReadWord(BQ27542G1_CMD_SOC);
	return percent;
}

/**
* @func   : MANAGE_RequestInfoApp
* @brief  : Send ble request info
* @param  : Page Info
* @retval : None
*/

void MANAGE_RequestInfoApp(uint8_t pageInfo){
	switch(pageInfo){
		case LCD_PAGE_INFO1:
			sprintf(sendDataBuffer, "{info:3,1,2,3}");            // {info:N,X1,X2...}
			BLE_SendData((uint8_t*) sendDataBuffer, strlen(sendDataBuffer));
			break;
		case LCD_PAGE_INFO2:
			sprintf(sendDataBuffer, "{info:4,4,5,6,7}");            // {info:N,X1,X2...}
			BLE_SendData((uint8_t*) sendDataBuffer, strlen(sendDataBuffer));
			break;
		default:
			break;
	}
}

/**
* @func   : MANAGE_ParseBleReceive
* @brief  : Parse data received from BLE
*           {abc:def}
* @param  : Data and size
* @retval : None
*/
#define VALUE_PARSE_SIZE      24

void MANAGE_ParseBleReceive(const char* data, uint16_t size){
	uint8_t valueParse[VALUE_PARSE_SIZE];
	
	if( (data[0] == BLE_START_MESSAGE_BYTE) && (data[size - 1] == BLE_END_MESSAGE_BYTE) ){
		// Send tag id and receive response ok
		if(strncmp(&data[1], BLE_TAG_OK_STRING, BLE_TAG_OK_LENGTH) == 0){
			MANAGE_RequestInfoApp(LCD_PAGE_INFO1);
		}
		// Info {tN.txt="..."}
		else if(strncmp(&data[3], BLE_INFO_RES_STRING, BLE_INFO_RES_LENGTH) == 0){
			// Fix bug app (template)
			if( (data[2] == '2') && ((data[9] == 'f') || (data[9] == 'F')) ){    // false == Female
				uint8_t text[13] = {0x74, 0x32, 0x2e, 0x74, 0x78, 0x74, 0x3d, 0x22, 0x43, 0xc3, 0xa1, 0x69, 0x22};
				LCD_SendCommand(text, 13);
			}
			else if( (data[1] == '2') && ((data[9] == 't') || (data[9] == 'T')) ){ // true == male
				uint8_t text[15] = {0x74, 0x32, 0x2e, 0x74, 0x78, 0x74, 0x3d, 0x22, 0xc4, 0x90, 0xe1, 0xbb, 0xb1, 0x63, 0x22};
				LCD_SendCommand(text, 15);
			}
			else{
				LCD_SendCommand((uint8_t*) &data[1], size - 2);
			}
		}
		// Timestamp {timestamp:123..}
		else if(strncmp(&data[1], BLE_TIMESTAMP_STRING, BLE_TIMESTAMP_LENGTH) == 0){
			uint32_t timestamp;
			
			memset(valueParse, 0, VALUE_PARSE_SIZE);
			memcpy(valueParse, &data[BLE_TIMESTAMP_LENGTH + 1], size - 2 - BLE_TIMESTAMP_LENGTH);
			timestamp = atoi(&data[BLE_TIMESTAMP_LENGTH + 1]);
			
			if(timestamp > 1577836800){ // 00:00:00 1/1/2020
				TIME_SetTimestamp(timestamp + 25200);      // GMT +7
				sprintf(sendDataBuffer, "{timestamp:0}");
			}
			else{
				// Error timestamp
				sprintf(sendDataBuffer, "{timestamp:1}");
			}
			
			BLE_SendData((uint8_t*) sendDataBuffer, strlen(sendDataBuffer));
		}
		else{
			sprintf(sendDataBuffer, "Not supported");
			BLE_SendData((uint8_t*) sendDataBuffer, strlen(sendDataBuffer));
		}
	}
}

uint8_t MANAGE_BatteryCharging(void){
	return BQ25892_ChargeState();
}


















