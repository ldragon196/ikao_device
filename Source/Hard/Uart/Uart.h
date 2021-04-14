/*
 * Uart.h
 *
 *  Created on: June 29, 2020
 *      Author: LongHD
 */
/******************************************************************************/

#ifndef _SOURCE_HARD_UART_UART_H_
#define _SOURCE_HARD_UART_UART_H_

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include "Config.h"

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/

// Use rx buffer and not tx buffer
#define UART_TX_BUF_SIZE                 256                           /**< UART TX buffer size */
#define UART_RX_BUF_SIZE                 256                           /**< UART RX buffer size */

#define UART_PARAMS_CONFIG               \
{                                        \
	.rx_pin_no = UART_RX_PIN,            \
	.tx_pin_no = UART_TX_PIN,            \
	.rts_pin_no = RTS_PIN_NUMBER,        \
	.cts_pin_no = CTS_PIN_NUMBER,        \
	.flow_control = UART_HWFC,           \
	.use_parity = false,                 \
	.baud_rate = UART_BAUDRATE           \
}

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/



/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/



/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/

void UART_Init(void);
uint8_t UART_RX_GetByte(uint8_t* read_byte);
void UART_TX_SendData(uint8_t *data, uint8_t length);

/******************************************************************************/

#endif /* _SOURCE_HARD_UART_UART_H_ */
