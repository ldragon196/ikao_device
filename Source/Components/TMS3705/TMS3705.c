/*
 * TMS3705.c
 *
 *  Created on: May 6, 2020
 *      Author: LongHD
 */
/******************************************************************************/

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_drv_gpiote.h"

#include "nrf_sdh.h"

#include "Utility/CRC16/CRC16.h"
#include "Hard/Event/Event.h"
#include "Hard/Timer/Timer.h"
#include "TMS3705.h"

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/

// See http://www.ti.com/lit/ds/scbs881e/scbs881e.pdf?ts=1590631940170
//     http://www.ti.com/lit/an/scba031/scba031.pdf?ts=1590403719044

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

static uint8_t tag_id[RFID_TAG_SIZE];
static uint8_t receive_buffer[RFID_RECEIVE_DATA_SIZE];

static uint8_t reading_start = 0;
static uint8_t tag_id_is_available = 0;

EventData readingTagEventControl;

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/



/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/

static void readingTagEventFunction(void);
static uint8_t TMS3705_Read80Bits(void);
static uint8_t TMS3705_CheckCRC(void);

/******************************************************************************/

/*  FDX
	11 Startbits 00000000001,
	38 ID bits
	10 Country code according to ISO 3166,
	1 Extra application bits,
	14 Reserved bits,
	1 Animal bit,
	16 CCITT CRC over the previous bits
	24 Application bits
*/



/**
* @func   : TMS3705_CheckCRC
* @brief  : Check crc16
* @param  : None
* @retval : Check ok?
*/

static uint8_t TMS3705_CheckCRC(void){
	uint16_t crc_cal = CRC16_Kermit(&receive_buffer[1], RFID_RECEIVE_DATA_SIZE - 3);
	uint16_t crc16_rx = (receive_buffer[RFID_RECEIVE_DATA_SIZE - 1] << 8) | receive_buffer[RFID_RECEIVE_DATA_SIZE - 2];
	
	if(crc_cal == crc16_rx){
		return 1;
	}
	return 0;
}

/**
* @func   : TMS3705_Read80Bits
* @brief  : Read bit, format 1 bit start (high) + 8 bit data + 1 bit stop (low)
            delay next byte < 620us
* @param  : None
* @retval : None
*/

static uint8_t TMS3705_Read80Bits(void){
	uint8_t i, j, temp;
	uint32_t timeout, delay;
	
	nrf_sdh_suspend();
	
	memset(receive_buffer, 0, RFID_RECEIVE_DATA_SIZE);
	timeout = 5000;            // timeout 5ms after txct high
	for(i = 0; i < RFID_RECEIVE_DATA_SIZE; i++){
		delay = TIMER_US_GetCurrentTime();
		while(!SCIO){
			if(TIMER_US_GetElapseTime(delay) > timeout){
				nrf_sdh_resume();
				return 0;
			}
		}
		
		timeout = 1000;                        // timeout 1000us (> 620us)
		nrf_delay_us(DELAY_START_BIT);	   // 64 +32 us start bit + 0.5 bit 1st bit
		
		for(j = 0; j < 8; j++){
			temp = SCIO;
			receive_buffer[i] += temp << j;
			nrf_delay_us(DELAY_ONE_BIT);     // delay for next bit next bit |- 64us-|-64us-|-64us-|..
		}
	}
	
	for(i = 0; i < RFID_RECEIVE_DATA_SIZE; i++){
		receive_buffer[i] = ~receive_buffer[i];
	}
	
	nrf_sdh_resume();
	
	return (receive_buffer[0] == RFID_START_BYTE) && (TMS3705_CheckCRC());
}

/**
* @func   : TMS3705_StartReadingTag
            TMS3705_StopReadingTag
* @brief  : start/stop reading tag id
* @param  : none
* @retval : none
*/

void TMS3705_StartReadingTag(void){
	reading_start = 1;
	
	POWER(HIGH);
	EVENT_SetDelayMS(readingTagEventControl, TIME_DELAY_MS(30));
}

void TMS3705_StopReadingTag(void){
	POWER(LOW);
	tag_id_is_available = 0;
	EVENT_SetInactive(readingTagEventControl);
}


/**
* @func   : reading_tag_event_function
* @brief  : 
* @param  : none
* @retval : none
*/

static void readingTagEventFunction(void){
	EVENT_SetInactive(readingTagEventControl);
	
	if(reading_start){
		TXCT(LOW);
		reading_start = 0;
		EVENT_SetDelayMS(readingTagEventControl, TIME_DELAY_MS(30));
	}
	else{
		TXCT(HIGH);
		reading_start = 1;
		if(TMS3705_Read80Bits()){
			tag_id_is_available = 1;
			memcpy(tag_id, &receive_buffer[1], RFID_TAG_SIZE);
			EVENT_SetDelayMS(readingTagEventControl, TIME_DELAY_MS(100));    // Read OK, TMS3705 go to sleep
		}
		else{
			EVENT_SetDelayMS(readingTagEventControl, TIME_DELAY_MS(30));
		}
	}
}

/**
* @func   : TMS3705_TagIsAvailable
* @brief  : Check tag is available or not
* @param  : None
* @retval : Available?
*/

uint8_t TMS3705_TagIsAvailable(void){
	return tag_id_is_available;
}

/**
* @func   : TMS3705_GetTag
* @brief  : Get tag
* @param  : none
* @retval : 
*/

uint8_t* TMS3705_GetTag(void){
	tag_id_is_available = 0;
	return tag_id;
}

/**
* @func   : TMS3705_Init
* @brief  : Init TMS3705
* @param  : None
* @retval : None
*/
 
void TMS3705_Init(void){
	TIMER_Init();
	
	// Config TXCT (DIN) as output and SCIO (DOUT) as input pulldown
	nrf_gpio_cfg_output(RFID_DIN_PIN);
	
	#ifdef RFID_USE_POWER_PIN
	nrf_gpio_cfg_output(RFID_POWER);
	#endif
	
	nrf_gpio_cfg_input(RFID_DOUT_PIN, NRF_GPIO_PIN_PULLDOWN);
	
	TXCT(LOW);
	POWER(LOW);
	
	EVENT_Creat(&readingTagEventControl, readingTagEventFunction);
}
