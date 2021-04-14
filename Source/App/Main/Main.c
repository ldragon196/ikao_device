/*
 * Main.c
 *
 *  Created on: June 29, 2020
 *      Author: LongHD
 */
/******************************************************************************/


/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include "nrf_sdh.h"

#define DEBUG_PRINT_ENABLE
#include "Utility/Debug/Debug.h"

#include "Hard/Event/Event.h"
#include "Hard/Uart/Uart.h"
#include "Hard/I2C/I2C.h"
#include "Hard/WDT/WDT.h"

#include "Mid/Buzz/Buzz.h"
#include "Mid/MyBLE/MyBLE.h"
#include "Mid/Button/Button.h"
#include "Mid/LCD/LCD.h"

#include "App/Manage/Manage.h"
#include "Components/TMS3705/TMS3705.h"
#include "Components/BQ27542G1/BQ27542G1.h"

#include "Config.h"

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/



/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

EventData mainProcessEventControl;
EventData readBatteryEventControl;

const uint8_t modePage[NUMBER_MODE] = {LCD_PAGE_MAIN, LCD_PAGE_SCAN, MODE_BLE};
const uint8_t scanModePage[NUMBER_SUBMODE] = {LCD_PAGE_INFO1, LCD_PAGE_INFO2};

static uint8_t mainMode = LCD_PAGE_MAIN;
static uint8_t subMode = SUB_MODE_IDLE;

volatile boolean bleConnected = FALSE;
volatile boolean enterMode = FALSE;

static uint32_t readBatteryCount = 0;
static uint32_t timeoutResetMode = 0;
static uint32_t timeoutPowerSave = 0;
static uint8_t currentBatteryRemaining = 0;
static uint8_t chargeStatus = CHARGE_STATUS_NONE;
static uint8_t batteryShow = 0;
static uint8_t forceShowTimestamp = false;

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/



/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/

static void MAIN_Init(void);
void mainProcessEventFunction(void);
void readBatteryEventFunction(void);

void MAIN_HandleBLEState(uint8_t type, const uint8_t *data, uint16_t size);
void MAIN_HandleModbusReceive(uint8_t* data, uint8_t size);
void MAIN_HandleButtonHolding(uint8_t button, ButtonState state);
void MAIN_HandleButtonReleased(uint8_t button, ButtonState state);

static void MAIN_ResetMode(void);
static void MAIN_EnterMode(void);

static void MAIN_EnterPowerSaveMode(void);
static void MAIN_ExitPowerSaveMode(void);

/******************************************************************************/


/**
* @func   : MAIN_Init
* @brief  : 
* @param  : none
* @retval : none
*/

static void MAIN_Init(void){
	// Initialize timer module.
	ret_code_t err_code = app_timer_init();
	APP_ERROR_CHECK(err_code);
	
	// Hard init
	UART_Init();
	EVENT_Init();
	I2C_Init();
	WDT_Init();
	
	// Mid init
	BUZZ_Init();
	BUTTON_Init(MAIN_HandleButtonReleased, MAIN_HandleButtonHolding);
	BLE_Init(MAIN_HandleBLEState);       // Initializes the SoftDevice and BLE (Require)
	
	TMS3705_Init();
	MANAGE_Init();
	
	/* Enable LCD */
	nrf_gpio_cfg_output(POWER_ENABLE_PIN);
	nrf_gpio_pin_write(POWER_ENABLE_PIN, HIGH);
	
	timeoutPowerSave = 0;
	readBatteryCount = TIME_READ_BATTERY_S; // read battery
	DEBUG_PRINTLN("Power up");
}

/**
* @func   : main
* @brief  : 
* @param  : none
* @retval : none
*/

int main(void){
	MAIN_Init();
	
	BLE_AdvertisingStart();
	MAIN_ResetMode();
	
	// Creat main process event and active
	EVENT_Creat(&mainProcessEventControl, mainProcessEventFunction);
	EVENT_SetDelayMS(mainProcessEventControl, TIME_DELAY_MS(1000));
	
	EVENT_Creat(&readBatteryEventControl, readBatteryEventFunction);
	EVENT_SetDelayMS(readBatteryEventControl, TIME_DELAY_MS(1000));
	
	BIPBIP();
	
    while(1){
		EVENT_Task();
		BLE_Task();
		WDT_Feed();
	}
}

/**
* @func   : mainProcessEventFunction
* @brief  : Event function
* @param  : None
* @retval : None
*/

void mainProcessEventFunction(void){
	EVENT_SetInactive(mainProcessEventControl);
	
	if( (mainMode == MODE_SCAN) && (subMode != SUB_MODE_IDLE) ){
		if(TMS3705_TagIsAvailable()){
			BIP();
			TMS3705_StopReadingTag();
			MANAGE_HandleTagId(TMS3705_GetTag());
			return;
		}
	}
	
	EVENT_SetDelayMS(mainProcessEventControl, TIME_DELAY_MS(200));
}

/**
* @func   : readBatteryEventFunction
* @brief  : Event function
* @param  : None
* @retval : None
*/

void readBatteryEventFunction(void){
	EVENT_SetInactive(readBatteryEventControl);
	
	if(mainMode == MODE_MAIN){
		if(readBatteryCount >= TIME_READ_BATTERY_S){
			readBatteryCount = 0;
			currentBatteryRemaining = MANAGE_ReadBattery() + 5;
			currentBatteryRemaining = (currentBatteryRemaining < MIN_BATTERY_PERCENTAGE) ? MIN_BATTERY_PERCENTAGE : currentBatteryRemaining;
			currentBatteryRemaining = (currentBatteryRemaining > MAX_BATTERY_PERCENTAGE) ? MAX_BATTERY_PERCENTAGE : currentBatteryRemaining;
			
			if(chargeStatus == CHARGE_STATUS_NONE){
				batteryShow = currentBatteryRemaining;
				LCD_ShowBattery(currentBatteryRemaining);
			}
		}
		timeoutResetMode = 0;
		MANAGE_HandleEventTime(forceShowTimestamp);
		forceShowTimestamp = false;
	}
	else{
		if(timeoutResetMode >= TIMEOUT_RESET_MODE_S){
			BIP();
			TMS3705_StopReadingTag();
			MAIN_ResetMode();
		}
	}
	
	chargeStatus = MANAGE_BatteryCharging();
	if(chargeStatus != CHARGE_STATUS_NONE){
		batteryShow = batteryShow + 20;
		if(batteryShow >= 120){
			batteryShow = currentBatteryRemaining;
		}
		else if(batteryShow >= 100){
			batteryShow = 100;
		}
		LCD_CharginBattery(batteryShow, currentBatteryRemaining);
	}
	
	if(timeoutPowerSave > TIMEOUT_POWER_SAVE_S){
		MAIN_EnterPowerSaveMode();
	}
	else{
		timeoutPowerSave++;
	}
	
	readBatteryCount++;
	timeoutResetMode++;  // Timeout to back to main
	EVENT_SetDelayMS(readBatteryEventControl, TIME_DELAY_MS(1000));
}

/**
* @func   : MAIN_HandleBLEState
* @brief  : Hanle BLE state
* @param  : None
* @retval : None
*/

void MAIN_HandleBLEState(uint8_t type, const uint8_t *data, uint16_t size){
	switch(type){
		case BLE_IDLE:
			BLE_AdvertisingStart();
			break;
		case BLE_CONNECTED:
			bleConnected = TRUE;
			MAIN_ResetMode();
			break;
		case BLE_DISCONNECTED:
			bleConnected = FALSE;
			MAIN_ResetMode();
			break;
		case BLE_ALL_PEERS_DELETED:
			DEBUG_PRINTLN("Peers deleted");
			break;
		case BLE_SHOW_PASSKEY:
			LCD_SelectPage(LCD_PAGE_PASS);
			LCD_ShowPasskey((char*) data);
			break;
		case BLE_UART_SERVICE_RECEIVED:
			MANAGE_ParseBleReceive((const char*) data, size);
			break;
		default:
			break;
	}
}

/**
* @func   : MAIN_HandleButtonHolding
* @brief  : Invoked when button holding 1s, 2s, 3s,...(still hold)
* @param  : Button and state
* @retval : None
*/

void MAIN_HandleButtonHolding(uint8_t button, ButtonState state){
	if(button == BUTTON_ENTER_ID){
		if(state == HOLD_2S){
			BIPBIPBIP();
		}
		else if(state == HOLD_5S){
			MAIN_EnterPowerSaveMode();
		}
	}
}

/**
* @func   : MAIN_HandleButtonReleased
* @brief  : Handle button function
* @param  : Button and state
* @retval : None
*/

void MAIN_HandleButtonReleased(uint8_t button, ButtonState state){
	if(state == PRESS){
		timeoutPowerSave = 0;
		timeoutResetMode = 0; // reset timeout
		switch(button){
			case BUTTON_LEFT_ID:
				// In Scan mode
				if( (mainMode == MODE_SCAN) && (subMode < NUMBER_SUBMODE) ){
					subMode = (NUMBER_SUBMODE + subMode - 1) % NUMBER_SUBMODE;
					LCD_SelectPage(scanModePage[subMode]);
					if(subMode == SUBMODE_INFO_1){
						MANAGE_ShowTag();
					}
					MANAGE_RequestInfoApp(scanModePage[subMode]);
				}
				else{
					mainMode = (NUMBER_MODE + mainMode - 1) % NUMBER_MODE;
					LCD_SelectPage(modePage[mainMode]);
					enterMode = FALSE;
				}
				break;
			case BUTTON_RIGHT_ID:
				// In Scan mode
				if( (mainMode == MODE_SCAN) && (subMode < NUMBER_SUBMODE) ){
					subMode = (subMode + 1) % NUMBER_SUBMODE;
					LCD_SelectPage(scanModePage[subMode]);
					if(subMode == SUBMODE_INFO_1){
						MANAGE_ShowTag();
					}
					MANAGE_RequestInfoApp(scanModePage[subMode]);
				}
				else{
					mainMode = (mainMode + 1) % NUMBER_MODE;
					LCD_SelectPage(modePage[mainMode]);
					enterMode = FALSE;
				}
				break;
			case BUTTON_ENTER_ID:
				if( (!enterMode) || (mainMode == MODE_SCAN) ){
					MAIN_EnterMode();
				}
				enterMode = TRUE;
				BIP();
				MAIN_ExitPowerSaveMode();
				break;
			default:
				break;
		}
	}
	else if ( (button == BUTTON_ENTER_ID) && (state == PRESS_2) ){
		MAIN_ResetMode();
	}
	else if ( (button == BUTTON_ENTER_ID) && (state == HOLD_2S) ){
		if(enterMode){
			if(mainMode == MODE_BLE){
				BLE_DeleteBonds();
			}
		}
		enterMode = FALSE;
		LCD_SelectPage(modePage[mainMode]);
	}
}

/**
* @func   : MAIN_ResetMode
* @brief  : Reset to main mode
* @param  : None
* @retval : None
*/

static void MAIN_ResetMode(void){
	mainMode = MODE_MAIN;
	subMode = SUB_MODE_IDLE;
	
	LCD_SelectPage(LCD_PAGE_MAIN);
	LCD_ShowBLEConnection(bleConnected);
}

/**
* @func   : MAIN_EnterMode
* @brief  : Show and handler mode selected
* @param  : None
* @retval : None
*/

static void MAIN_EnterMode(){
	switch(mainMode){
		case MODE_SCAN:
			subMode = SUBMODE_INFO_1;
			BIP();
			TMS3705_StartReadingTag();
			LCD_SelectPage(scanModePage[subMode]);
			EVENT_SetActive(mainProcessEventControl);
			break;
		case MODE_BLE:
			LCD_SelectPage(LCD_PAGE_BONDS);
			LCD_ShowBonds(BLE_GetNumBonds());
			break;
		default:
			break;
	}
}

static void MAIN_EnterPowerSaveMode(void){
	TMS3705_StopReadingTag();
	MAIN_ResetMode();
	
	nrf_gpio_pin_write(POWER_ENABLE_PIN, LOW);
	nrf_sdh_suspend();
}

static void MAIN_ExitPowerSaveMode(void){
	nrf_gpio_pin_write(POWER_ENABLE_PIN, HIGH);
	nrf_sdh_resume();
	
	forceShowTimestamp = true;
	readBatteryCount = TIME_READ_BATTERY_S - 2; // read battery after 1-2s
}






