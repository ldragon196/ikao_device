/*
 * button.h
 *
 *  Created on: May 11, 2020
 *      Author: LongHD
 */

#ifndef SOURCE_MID_BUTTON_BUTTON_H_
#define SOURCE_MID_BUTTON_BUTTON_H_

/******************************************************************************/


/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include "Config.h"

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/

#define INVALID_BUTTON_PIN                     0xFF

#define TIME_CHECK_NOISE_MS                  TIME_DELAY_MS(100)

#define TIME_TICK_HOLD_BUTTON_MS             TIME_DELAY_MS(200)
#define TIMES_TICK_HOLD_1S                   5               // TIMES_TICK_HOLD_1S x TIME_TICK_HOLD_BUTTON_MS (ms)
#define TIMES_TICK_HOLD_2S                   10
#define TIMES_TICK_HOLD_3S                   15
#define TIMES_TICK_HOLD_5S                   20
#define TIMES_TICK_HOLD_10S                  50

#define TIME_TICK_RELEASE_BUTTON_MS          TIME_DELAY_MS(100)
#define TIMES_CHECK_RELEASE                  3               // TIMES_CHECK_RELEASE x TIME_TICK_RELEASE_BUTTON_MS (ms)


typedef uint8_t ButtonState;
enum {
	RELEASE = 0,
	PRESS = 1,
	PRESS_2 = 2,
	PRESS_3 = 3,
	PRESS_4 = 4,
	PRESS_5 = 5,
	PRESS_MAX = 10,
	PRESS_ERROR,
	HOLD_1S = 11,
	HOLD_2S = 12,
	HOLD_3S = 13,
	HOLD_5S = 14,
	HOLD_7S = 15,
	HOLD_10S = 16,
	HOLD_ERROR,
};

typedef void (*ButtonStateHandler)(uint8_t, ButtonState);
typedef struct {
	uint8_t press_state;
	uint8_t hold_count;
	uint8_t release_count;
	uint8_t trigger_press;
	uint8_t trigger_release;
} ButtonParam_t;


/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/



/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/



/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/

void BUTTON_Init(ButtonStateHandler releaseInit, ButtonStateHandler holdingInit);

/******************************************************************************/

#endif /* SOURCE_MID_BUTTON_BUTTON_H_ */
