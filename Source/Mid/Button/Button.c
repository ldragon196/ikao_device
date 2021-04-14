/*
 * button.c
 *
 *  Created on: May 11, 2020
 *      Author: LongHD
 */

/******************************************************************************/


/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include "app_button.h"

#include "Hard/Event/Event.h"
#include "Button.h"

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/

#define BUTTON_PIN_CONFIG                                                   \
{                                                                           \
	{BUTTON_PIN_0, false, NRF_GPIO_PIN_PULLUP, button_in_handler},          \
	{BUTTON_PIN_1, false, NRF_GPIO_PIN_PULLUP, button_in_handler},          \
	{BUTTON_PIN_2, false, NRF_GPIO_PIN_PULLDOWN, button_in_handler},          \
}

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

// Event data
EventData button_hold_event_control;
EventData button_release_event_control;

// Callback function
ButtonStateHandler handle_button_callback = NULL;
ButtonStateHandler handle_button_holding = NULL;

// To detect button state
ButtonParam_t button_params[NUMBER_OF_BUTTON] = {0};

static void button_in_handler(uint8_t pin, uint8_t action);
static const app_button_cfg_t app_buttons[NUMBER_OF_BUTTON] = BUTTON_PIN_CONFIG;

bool first_active_hold = true;          // First button is pressed
bool first_active_release = true;       // All button is released

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/



/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/

// Event function
static void button_hold_event_function(void);
static void button_release_event_function(void);

static uint8_t button_get_pin_index(uint32_t pin);

/******************************************************************************/

/**
 * @func   button_get_pin_index
 * @brief  get button index
 * @param  pin
 * @retval button instance
 *         INVALID_BUTTON_PIN if not match
 */
 
static uint8_t button_get_pin_index(uint32_t pin){
	uint8_t i;
	for(i = 0; i < NUMBER_OF_BUTTON; i++){
		if(app_buttons[i].pin_no == pin){
			return i;
		}
	}
	
	return INVALID_BUTTON_PIN;
}

/**
 * @func   button_in_handler
 * @brief  handler button interrupt
 * @param  none
 * @retval none
 */

static void button_in_handler(uint8_t pin, uint8_t action){
	uint8_t button_id = button_get_pin_index(pin);
	
	if(button_id == INVALID_BUTTON_PIN){
		return;
	}
	
	if(button_id == BUTTON_ENTER_ID){
		action = !action;
	}
	
	// Button is pressed
	if(action == APP_BUTTON_PUSH){
		button_params[button_id].press_state++;
		if(button_params[button_id].press_state > PRESS_MAX){
			button_params[button_id].press_state = PRESS_ERROR;
		}
		button_params[button_id].trigger_press = 1;
		button_params[button_id].trigger_release = 0;
		button_params[button_id].hold_count = 0;
		
		// If first, active event
		if(first_active_hold){
			first_active_hold = false;
			EVENT_SetActive(button_hold_event_control);
		}
	}
	else{
		button_params[button_id].release_count = 0;
		button_params[button_id].trigger_release = 1;
		button_params[button_id].trigger_press = 0;
		if(first_active_release){
			first_active_release = false;
			EVENT_SetActive(button_release_event_control);
		}
	}
}


/**
 * @func   button_hold_event_function
 * @brief  event for handle button hold
 * @param  none
 * @retval none
 */

static void button_hold_event_function(void){
	EVENT_SetInactive(button_hold_event_control);
	uint8_t still_trigger = 0;

	for(uint8_t i = 0; i < NUMBER_OF_BUTTON; i++){
		if(button_params[i].trigger_press == 1){
			button_params[i].hold_count++;

			if(button_params[i].hold_count == TIMES_TICK_HOLD_1S){
				button_params[i].press_state = HOLD_1S;
				if(handle_button_holding != NULL){
					handle_button_holding(i, HOLD_1S);
				}
			}
			else if(button_params[i].hold_count == TIMES_TICK_HOLD_2S){
				button_params[i].press_state = HOLD_2S;
				if(handle_button_holding != NULL){
					handle_button_holding(i, HOLD_2S);
				}
			}
			else if(button_params[i].hold_count == TIMES_TICK_HOLD_3S){
				button_params[i].press_state = HOLD_3S;
				if(handle_button_holding != NULL){
					handle_button_holding(i, HOLD_3S);
				}
			}
			else if(button_params[i].hold_count == TIMES_TICK_HOLD_5S){
				button_params[i].press_state = HOLD_5S;
				if(handle_button_holding != NULL){
					handle_button_holding(i, HOLD_5S);
				}
			}
			else if(button_params[i].hold_count == TIMES_TICK_HOLD_10S){
				button_params[i].press_state = HOLD_10S;
				if(handle_button_holding != NULL){
					handle_button_holding(i, HOLD_10S);
				}
			}
			still_trigger = 1;
		}
	}
	
	// Loop until no button is pressed
	if(still_trigger == 1){
		EVENT_SetDelayMS(button_hold_event_control, TIME_TICK_HOLD_BUTTON_MS);
	}
	else {
		// Reset trigger
		first_active_hold = true;
	}
}

/**
 * @func   button_release_event_function
 * @brief  event for handle button release
 * @param  none
 * @retval none
 */

static void button_release_event_function(void){
	EVENT_SetInactive(button_release_event_control);
	uint8_t still_trigger = 0;
	
	for(uint8_t i = 0; i < NUMBER_OF_BUTTON; i++){
		if(button_params[i].trigger_release == 1){
			button_params[i].release_count++;

			if(button_params[i].release_count > TIMES_CHECK_RELEASE){
				button_params[i].trigger_release = 0;
				// Must check callback NULL or not
				if(handle_button_callback != NULL){
					handle_button_callback(i, button_params[i].press_state);
				}
				button_params[i].press_state = RELEASE;
			}
			still_trigger = 1;
		}
	}
	
	// Loop until all button are released
	if(still_trigger == 1){
		EVENT_SetDelayMS(button_release_event_control, TIME_TICK_RELEASE_BUTTON_MS);
	}
	else{
		// Reset trigger
		first_active_release = true;
	}
}


/**
 * @func   BUTTON_Init
 * @brief  Initialized button
 * @param  releaseInit: callback handle when button is released
*         holdingInit: callback handle when button is holding
 * @retval None
 */

void BUTTON_Init(ButtonStateHandler releaseInit, ButtonStateHandler holdingInit){
	// Init button app
	uint32_t err_code = NRF_SUCCESS;
	err_code = app_button_init((app_button_cfg_t *)app_buttons,
                                NUMBER_OF_BUTTON,
                                TIME_CHECK_NOISE_MS);
	
	// Enable button gpiote
	if (err_code == NRF_SUCCESS){
		err_code = app_button_enable();
	}
	
	// Init callback func
	if(releaseInit != NULL){
		handle_button_callback = releaseInit;
	}
	
	if(holdingInit != NULL){
		handle_button_holding = holdingInit;
	}
	
	// Creat event
	EVENT_Creat(&button_hold_event_control, button_hold_event_function);
	EVENT_Creat(&button_release_event_control, button_release_event_function);
}








