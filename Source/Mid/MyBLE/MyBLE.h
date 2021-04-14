/*
 * MyBLE.h
 *
 *  Created on: Jule 1, 2020
 *      Author: LongHD
 */
/******************************************************************************/

#ifndef _SOURCE_MID_MYBLE_MYBLE_H_
#define _SOURCE_MID_MYBLE_MYBLE_H_

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include "Config.h"

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/

#define DEVICE_NAME                     "NTQ iDe"                                  /**< Name of device. Will be included in the advertising data. */
#define MANUFACTURER_NAME               "NTQ Solution"                             /**< Manufacturer. Will be passed to Device Information Service. */
#define MODEL_NUMBER                    "nRF52"                                    /**< Model Number string. Will be passed to Device Information Service. */
#define MANUFACTURER_ID                 0x12345678                                 /**< DUMMY Manufacturer ID. Will be passed to Device Information Service. You shall use the ID for your Company*/
#define ORG_UNIQUE_ID                   0xAABBCC                                   /**< DUMMY Organisation Unique ID. Will be passed to Device Information Service. You shall use the Organisation Unique ID relevant for your Company */



#define PASSKEY_TXT                     "Passkey:"                                  /**< Message to be displayed together with the pass-key. */
#define PASSKEY_TXT_LENGTH              8                                           /**< Length of message to be displayed together with the pass-key. */
#define PASSKEY_LENGTH                  6                                           /**< Length of pass-key received by the stack for display. */

#define APP_ADV_INTERVAL                40                                          /**< The advertising interval (in units of 0.625 ms. This value corresponds to 25 ms). */
#define APP_ADV_DURATION                18000                                       /**< The advertising duration (180 seconds) in units of 10 milliseconds. */

#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(10, UNIT_1_25_MS)             /**< Minimum acceptable connection interval (10 ms). */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(100, UNIT_1_25_MS)            /**< Maximum acceptable connection interval (100 ms) */
#define SLAVE_LATENCY                   0                                           /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)             /**< Connection supervisory timeout (4 seconds). */
#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000)                       /**< Time from initiating event (connect or start of notification) to first time sd_ble_gap_conn_param_update is called (5 seconds). */
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000)                      /**< Time between each call to sd_ble_gap_conn_param_update after the first call (30 seconds). */
#define MAX_CONN_PARAM_UPDATE_COUNT     3                                           /**< Number of attempts before giving up the connection parameter negotiation. */


#define APP_BLE_OBSERVER_PRIO           3                                           /**< Application's BLE observer priority. You shouldn't need to modify this value. */
#define APP_BLE_CONN_CFG_TAG            1                                           /**< A tag identifying the SoftDevice BLE configuration. */



#define SEC_PARAM_BOND                  1                                           /**< Perform bonding. */
#define SEC_PARAM_MITM                  1                                           /**< Man In The Middle protection required (applicable when display module is detected). */
#define SEC_PARAM_LESC                  1                                           /**< LE Secure Connections enabled. */
#define SEC_PARAM_KEYPRESS              0                                           /**< Keypress notifications not enabled. */
#define SEC_PARAM_IO_CAPABILITIES       BLE_GAP_IO_CAPS_DISPLAY_ONLY                /**< Display I/O capabilities. */
#define SEC_PARAM_OOB                   0                                           /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE          7                                           /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE          16                                          /**< Maximum encryption key size. */

#define SEC_KDIST_OWN_ENC               1
#define SEC_KDIST_OWN_ID                1
#define SEC_KDIST_PEER_ENC              1
#define SEC_KDIST_PEER_ID               1


// Advertising 
#define ADV_UUIDS_CONFIG                                           \
{                                                                  \
    {BLE_UUID_BATTERY_SERVICE, BLE_UUID_TYPE_BLE},                 \
    {BLE_UUID_DEVICE_INFORMATION_SERVICE, BLE_UUID_TYPE_BLE}       \
}

#define ADV_INIT_CONFIG                                            \
{                                                                  \
	.advdata =                                                     \
	{                                                              \
		.name_type = BLE_ADVDATA_FULL_NAME,                        \
		.include_appearance = true,                                \
		.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_LIMITED_DISC_MODE,      \
	},                                                             \
	.config =                                                      \
	{                                                              \
		.ble_adv_fast_enabled = true,                              \
		.ble_adv_fast_interval = APP_ADV_INTERVAL,                 \
		.ble_adv_fast_timeout = APP_ADV_DURATION                   \
	},                                                             \
}


// Connect params

#define GAP_CONN_PARAMS_CONFIG                                     \
{                                                                  \
	.min_conn_interval = MIN_CONN_INTERVAL,                        \
	.max_conn_interval = MAX_CONN_INTERVAL,                        \
	.slave_latency = SLAVE_LATENCY,                                \
	.conn_sup_timeout = CONN_SUP_TIMEOUT                           \
}

#define CONN_PARAMS_CONFIG                                             \
{                                                                      \
	.p_conn_params = NULL,                                             \
	.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY,  \
	.next_conn_params_update_delay = NEXT_CONN_PARAMS_UPDATE_DELAY,    \
	.max_conn_params_update_count = MAX_CONN_PARAM_UPDATE_COUNT,       \
	.start_on_notify_cccd_handle = BLE_GATT_HANDLE_INVALID,            \
	.disconnect_on_fail = false,                                       \
	.evt_handler = NULL,                                               \
	.error_handler = NULL,                                             \
}

// Peer manager
#define PEER_MANAGER_CONFIG                                        \
{                                                                  \
	.bond = SEC_PARAM_BOND,                                        \
	.mitm = SEC_PARAM_MITM,                                        \
	.lesc = SEC_PARAM_LESC,                                        \
	.keypress = SEC_PARAM_KEYPRESS,                                \
	.io_caps = SEC_PARAM_IO_CAPABILITIES,                          \
	.oob = SEC_PARAM_OOB,                                          \
	.min_key_size = SEC_PARAM_MIN_KEY_SIZE,                        \
	.max_key_size = SEC_PARAM_MAX_KEY_SIZE,                        \
	.kdist_own =                                                   \
		{                                                          \
			.enc = SEC_KDIST_OWN_ENC,                              \
			.id = SEC_KDIST_OWN_ID                                 \
		},                                                         \
	.kdist_peer =                                                  \
		{                                                          \
			.enc = SEC_KDIST_PEER_ENC,                             \
			.id = SEC_KDIST_PEER_ID                                \
		}                                                          \
}

// Services
#define SERVICE_BATTERY_INIT                                       \
{                                                                  \
	.bl_rd_sec = SEC_OPEN,                                         \
	.bl_cccd_wr_sec = SEC_OPEN,                                    \
	.bl_report_rd_sec = SEC_OPEN,                                  \
	.evt_handler = NULL,                                           \
	.support_notification = true,                                  \
	.p_report_ref = NULL,                                          \
	.initial_batt_level = 100,                                     \
}


// Callback handle ble state
typedef void (*BleStateHandler)(uint8_t, const uint8_t*, uint16_t);
enum{
	BLE_IDLE = 0,
	BLE_CONNECTED,
	BLE_DISCONNECTED,
	BLE_ALL_PEERS_DELETED,
	BLE_SHOW_PASSKEY,
	BLE_UART_SERVICE_RECEIVED,
};

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/



/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/



/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/

void BLE_Init(BleStateHandler initFunc);
void BLE_Task(void);

void BLE_DeleteBonds(void);
uint32_t BLE_GetNumBonds(void);
	
void BLE_AdvertisingStart(void);
void BLE_AdvertisingStop(void);

void BLE_SendData(uint8_t* data, uint16_t length);

/******************************************************************************/

#endif /* _SOURCE_MID_MYBLE_MYBLE_H_ */
