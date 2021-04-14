/*
 * Config.h
 *
 *  Created on: June 29, 2020
 *      Author: LongHD
 */

#ifndef SOURCE_CONFIG_H_
#define SOURCE_CONFIG_H_

/******************************************************************************/


/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include "nrf_gpio.h"
#include "app_uart.h"
#include "nrf_uart.h"
#include "app_timer.h"
#include "nrf_drv_spi.h"
#include "nrf_drv_twi.h"

#include "Utility/Utility.h"

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/

/* ---------- SYSTEM ---------- */

#define MAX_TICK_COUNTER
#define CURRENT_TIME_MS                  app_timer_cnt_get()
#define TIME_ELAPSE_MS(start)            ( (CURRENT_TIME_MS() - (start)) & MAX_TICK_COUNTER)

/* ---------- UART ---------- */

#define UART_TX_PIN                      7
#define UART_RX_PIN                      6
#define RTS_PIN_NUMBER                   UART_PIN_DISCONNECTED
#define CTS_PIN_NUMBER                   UART_PIN_DISCONNECTED

#define UART_BAUDRATE                    NRF_UART_BAUDRATE_9600
#define UART_HWFC                        APP_UART_FLOW_CONTROL_DISABLED

/* ---------- RFID TMS3705 ---------- */

#define RFID_DIN_PIN                     8
#define RFID_DOUT_PIN                    9

#ifndef LOW
#define LOW                              0
#define HIGH                             1
#endif

/* ---------- I2C ---------- */

#define I2C_SCL_PIN                      4         // SCL signal pin
#define I2C_SDA_PIN                      3         // SDA signal pin


#define BQ27542G1_I2C                    0
#define BQ25892_I2C                      0
#define M41T0M6F_I2C                     0

/* ---------- BUZZER ---------- */

#define BUZZER_PIN                       20

/* ---------- BLE ---------- */

#define MAX_BONDS_ALLOWED                10

/* ---------- BUTTON ---------- */

#define NUMBER_OF_BUTTON                 3
#define BUTTON_PIN_0                     27                         
#define BUTTON_PIN_1                     26
#define BUTTON_PIN_2                     25

#define BUTTON_LEFT_ID                   0
#define BUTTON_RIGHT_ID                  1
#define BUTTON_ENTER_ID                  2

/* ---------- LCD ---------- */

#define NUM_INFO_PER_PAGE                4

enum{
	MODE_MAIN = 0,
	MODE_SCAN,
	MODE_BLE,
	NUMBER_MODE
};

enum {
	SUBMODE_INFO_1 = 0,
	SUBMODE_INFO_2,
	NUMBER_SUBMODE
};

#define LOW_BATTERY_PERCENTAGE           20
#define MIN_BATTERY_PERCENTAGE           5
#define MAX_BATTERY_PERCENTAGE           100

/* ---------- MAIN ---------- */

#define SUB_MODE_IDLE                    0xFF
#define POWER_ON_PIN                     5

#define TIME_READ_BATTERY_S              60
#define TIMEOUT_RESET_MODE_S             180
#define TIMEOUT_POWER_SAVE_S             300

#define POWER_ENABLE_PIN                 11

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

#endif /* SOURCE_CONFIG_H_ */
