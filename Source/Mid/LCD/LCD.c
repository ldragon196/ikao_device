/*
 * lcd.c
 *
 *  Created on: May 12, 2020
 *      Author: LongHD
 */
/******************************************************************************/


/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include <time.h>

#include "Hard/Uart/Uart.h"
#include "LCD.h"

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/



/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

static char send_data[32];
static uint32_t last_timestamp = 0;
static uint8_t last_battery_percent = 100;

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/



/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/



/******************************************************************************/

/**
* @func   : LCD_SendCommand
* @brief  : Send data and 3 bytes 0xFF
* @param  : data
*           length of data
* @retval : None
*/

void LCD_SendCommand(uint8_t *data, uint8_t length){
	uint8_t end_data[LCD_NUM_OF_BYTE_END];
	memset(end_data, LCD_BYTE_END, LCD_NUM_OF_BYTE_END);
	
	UART_TX_SendData(end_data, LCD_NUM_OF_BYTE_END);
	UART_TX_SendData(data, length);
	UART_TX_SendData(end_data, LCD_NUM_OF_BYTE_END);
}

/**
* @func   : LCD_SelectPage
* @brief  : Select page
* @param  : Page
* @retval : None
*/

void LCD_SelectPage(uint8_t page){
	sprintf(send_data, "page %d", page);
	LCD_SendCommand((uint8_t*) send_data, strlen(send_data));
}

/**
* @func   : LCD_ShowTag
* @brief  : Show tag id
* @param  : Tag
* @retval : None
*/

void LCD_ShowTag(char* tag){
	sprintf(send_data, "t0.txt=\"%s\"", tag);
	LCD_SendCommand((uint8_t*) send_data, strlen(send_data));
}

/**
* @func   : LCD_ShowBattery
* @brief  : Show battery percentage
* @param  : Percent
* @retval : None
*/

void LCD_ShowBattery(uint8_t percent){	
	sprintf(send_data, "tBat.txt=\"%d%%\"", percent);
	LCD_SendCommand((uint8_t*) send_data, strlen(send_data));
	sprintf(send_data, "jBat.val=%d", percent);
	LCD_SendCommand((uint8_t*) send_data, strlen(send_data));
	
	if( (last_battery_percent > LOW_BATTERY_PERCENTAGE) && (percent <= LOW_BATTERY_PERCENTAGE) ){
		sprintf(send_data, "tBat.pco=63488");
		LCD_SendCommand((uint8_t*) send_data, strlen(send_data));
		sprintf(send_data, "jBat.pco=63488");
		LCD_SendCommand((uint8_t*) send_data, strlen(send_data));
	}
	else if( (last_battery_percent <= LOW_BATTERY_PERCENTAGE) && (percent > LOW_BATTERY_PERCENTAGE) ){
		sprintf(send_data, "tBat.pco=65535");
		LCD_SendCommand((uint8_t*) send_data, strlen(send_data));
		sprintf(send_data, "jBat.pco=31");
		LCD_SendCommand((uint8_t*) send_data, strlen(send_data));
	}
	last_battery_percent = percent;
}

/**
* @func   : LCD_CharginBattery
* @brief  : Show battery percentage
* @param  : Percent
* @retval : None
*/

void LCD_CharginBattery(uint8_t percent, uint8_t percentText){
	sprintf(send_data, "tBat.txt=\"%d%%\"", percentText);
	LCD_SendCommand((uint8_t*) send_data, strlen(send_data));
	
	sprintf(send_data, "jBat.val=%d", percent);
	LCD_SendCommand((uint8_t*) send_data, strlen(send_data));
	
	sprintf(send_data, "tBat.pco=65535");
	LCD_SendCommand((uint8_t*) send_data, strlen(send_data));
	sprintf(send_data, "jBat.pco=31");
	LCD_SendCommand((uint8_t*) send_data, strlen(send_data));
}

/**
* @func   : LCD_ShowBonds
* @brief  : Show number bond device
* @param  : Number
* @retval : None
*/

void LCD_ShowBonds(uint32_t bonds){
	sprintf(send_data, "n0.val=%d", bonds);
	LCD_SendCommand((uint8_t*) send_data, strlen(send_data));
}

/**
* @func   : LCD_ShowBLEConnection
* @brief  : Show BLE state
* @param  : Connected/ Disconnected
* @retval : None
*/

void LCD_ShowBLEConnection(boolean connected){
	if(connected){
		sprintf(send_data, "pBLE.pic=%01d", LCD_PIC_BLE_CONNECTED);
	}
	else{
		sprintf(send_data, "pBLE.pic=%01d", LCD_PIC_BLE_DISCONNECTED);
	}
	LCD_SendCommand((uint8_t*) send_data, strlen(send_data));
}

/**
* @func   : LCD_ShowPasskey
* @brief  : Show passkey
* @param  : Key
* @retval : None
*/

void LCD_ShowPasskey(char* pass){
	sprintf(send_data, "tPass.txt=\"%s\"", pass);
	LCD_SendCommand((uint8_t*) send_data, strlen(send_data));
}


/**
* @func   : LCD_ShowHistory
* @brief  : Show number of history max/ current
* @param  : Number
* @retval : None
*/

void LCD_ShowHistory(uint32_t max, uint32_t current){
	sprintf(send_data, "n0.val=%u", current);
	LCD_SendCommand((uint8_t*) send_data, strlen(send_data));
	sprintf(send_data, "n1.val=%u", max);
	LCD_SendCommand((uint8_t*) send_data, strlen(send_data));
}

// format Www Mmm dd hh:mm:ss yyyy\n
/**
* @func   : LCD_ShowDateTime
* @brief  : Send datetime string to lcd
* @param  : Timestamp
* @retval : None
*/

void LCD_ShowDateTime(uint32_t timestamp){
	struct tm* localtm = localtime(&timestamp);    
	
	// > 1 day
	if( ((timestamp - last_timestamp) & 0xFFFFFFFF) >= 86400) {
		sprintf(send_data, "tDay.txt=\"%02d/%02d/%04d\"", localtm->tm_mday, localtm->tm_mon + 1, localtm->tm_year + 1900);
		LCD_SendCommand((uint8_t*) send_data, strlen(send_data));
	}
	
	// > 1 minute
	if( ((timestamp - last_timestamp) & 0xFFFFFFFF) >= 60){
		last_timestamp = timestamp;
		sprintf(send_data, "tHour.txt=\"%02d:%02d\"", localtm->tm_hour, localtm->tm_min);
		LCD_SendCommand((uint8_t*) send_data, strlen(send_data));
	}
}

void LCD_ForceShowDateTime(uint32_t timestamp){
	struct tm* localtm = localtime(&timestamp);
	
	sprintf(send_data, "tDay.txt=\"%02d/%02d/%04d\"", localtm->tm_mday, localtm->tm_mon + 1, localtm->tm_year + 1900);
	LCD_SendCommand((uint8_t*) send_data, strlen(send_data));
	sprintf(send_data, "tHour.txt=\"%02d:%02d\"", localtm->tm_hour, localtm->tm_min);
	LCD_SendCommand((uint8_t*) send_data, strlen(send_data));
}





