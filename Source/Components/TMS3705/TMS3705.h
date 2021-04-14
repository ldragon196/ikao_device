/*
 * TMS3705.h
 *
 *  Created on: June 9, 2020
 *      Author: LongHD
 */
/******************************************************************************/

#ifndef _SOURCE_COMPONENTS_TMS3705_TMS3705_H_
#define _SOURCE_COMPONENTS_TMS3705_TMS3705_H_

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include "Config.h"

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/

#define TIMEOUT_READING_COUNT                           1000

#define RFID_RECEIVE_DATA_SIZE                          11
#define RFID_TAG_SIZE                                   8

#define DELAY_ONE_BIT                                   62
#define DELAY_START_BIT                                 85

#define RFID_START_BYTE                                 0x7E

#define SCIO                                            nrf_gpio_pin_read(RFID_DOUT_PIN)
#define TXCT(a)                                         nrf_gpio_pin_write(RFID_DIN_PIN, (a));
#ifdef RFID_USE_POWER_PIN
#define POWER(a)                                        nrf_gpio_pin_write(RFID_POWER, (a));
#else
#define POWER(a)                                        
#endif

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/



/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/



/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/

void TMS3705_Init(void);
uint8_t* TMS3705_GetTag(void);
uint8_t TMS3705_TagIsAvailable(void);
void TMS3705_StartReadingTag(void);
void TMS3705_StopReadingTag(void);

/******************************************************************************/

#endif /* _SOURCE_COMPONENTS_TMS3705_TMS3705_H_ */
