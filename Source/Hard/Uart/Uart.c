/*
 * Uart.c
 *
 *  Created on: June 29, 2020
 *      Author: LongHD
 */
/******************************************************************************/

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include "Uart.h"

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/



/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/



/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/



/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/

static void UART_ErrorHandler(app_uart_evt_t * p_event);

/******************************************************************************/

/**
 * @func   uart_error_handle
 * @brief  handle rx error
 * @param  p_event
 * @retval None
 */
 
static void UART_ErrorHandler(app_uart_evt_t * p_event)
{
//    if (p_event->evt_type == APP_UART_COMMUNICATION_ERROR)
//    {
//        APP_ERROR_HANDLER(p_event->data.error_communication);
//    }
//    else if (p_event->evt_type == APP_UART_FIFO_ERROR)
//    {
//        APP_ERROR_HANDLER(p_event->data.error_code);
//    }
}

/**
 * @func   UART_Init
 * @brief  Initialized uart
 * @param  None
 * @retval None
 */
 
void UART_Init(void){
	uint32_t err_code;
	const app_uart_comm_params_t comm_params = UART_PARAMS_CONFIG;
	
	APP_UART_FIFO_INIT(&comm_params,
					 UART_RX_BUF_SIZE,
					 UART_TX_BUF_SIZE,
					 UART_ErrorHandler,
					 APP_IRQ_PRIORITY_LOWEST,
					 err_code);
    APP_ERROR_CHECK(err_code);
}

/**
 * @func   UART_RX_GetByte
 * @brief  Eead byte uart
 * @param  Result
 * @retval True if success, otherwise, return false
 */

uint8_t UART_RX_GetByte(uint8_t* read_byte){
	if(app_uart_get(read_byte) == NRF_SUCCESS){
		return true;
	}
	return false;
}

/**
 * @func   UART_TX_SendData
 * @brief  Send data via uart
 * @param  data
 *         length of data
 * @retval None
 */

void UART_TX_SendData(uint8_t *data, uint8_t length){
	for(uint8_t i = 0; i < length; i++){
		app_uart_put(data[i]);
	}
}


