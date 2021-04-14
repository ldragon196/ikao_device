/*
 * LCD.h
 *
 *  Created on: July 3, 2020
 *      Author: LongHD
 */
/******************************************************************************/

#ifndef _SOURCE_APP_LCD_LCD_H_
#define _SOURCE_APP_LCD_LCD_H_

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include "Config.h"

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/

#define LCD_NUM_OF_BYTE_END                      3
#define LCD_BYTE_END                             0xFF

#define LCD_TURN_ON()                            nrf_gpio_pin_write(LCD_POWER_PIN, HIGH)
#define LCD_TURN_OFF()                           nrf_gpio_pin_write(LCD_POWER_PIN, LOW)

#define LCD_PAGE_MAIN                            0
#define LCD_PAGE_SCAN                            1
#define LCD_PAGE_BLE                             2

#define LCD_PAGE_PASS                            3
#define LCD_PAGE_INFO1                           4
#define LCD_PAGE_INFO2                           5
#define LCD_PAGE_BONDS                           6

#define LCD_PIC_BLE_CONNECTED                    1
#define LCD_PIC_BLE_DISCONNECTED                 2

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/



/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/



/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/

void LCD_SendCommand(uint8_t *data, uint8_t length);;
void LCD_ShowTag(char* tag);
void LCD_SelectPage(uint8_t page);
void LCD_ShowBLEConnection(boolean connected);
void LCD_ShowBonds(uint32_t bonds);
void LCD_ShowDateTime(uint32_t timestamp);
void LCD_ShowPasskey(char* pass);
void LCD_ShowBattery(uint8_t percent);
void LCD_CharginBattery(uint8_t percent, uint8_t percentText);
void LCD_ForceShowDateTime(uint32_t timestamp);

/******************************************************************************/
#endif /* _SOURCE_APP_LCD_LCD_H_ */
