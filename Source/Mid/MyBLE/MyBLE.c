/*
 * MyBLE.c
 *
 *  Created on: July 1, 2020
 *      Author: LongHD
 */
/******************************************************************************/

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include "ble.h"
#include "app_error.h"
#include "nrf_sdh.h"
#include "nrf_sdh_soc.h"
#include "nrf_sdh_ble.h"
#include "nrf_ble_lesc.h"

// GATT
#include "nrf_ble_gatt.h"

// Advertising
#include "ble_advdata.h"
#include "ble_advertising.h"
#include "ble_srv_common.h"

// Connect parameters
#include "ble_conn_params.h"
#include "ble_conn_state.h"

// Peer manager
#include "peer_manager.h"
#include "peer_manager_handler.h"


// Service
#include "ble_bas.h"
#include "ble_dis.h"
#include "nrf_ble_qwr.h"
#include "ble_nus.h"

//#define DEBUG_PRINT_ENABLE
#include "Utility/Debug/Debug.h"

#include "MyBLE.h"

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/



/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

NRF_BLE_GATT_DEF(m_gatt);                                                           /**< GATT module instance. */
BLE_ADVERTISING_DEF(m_advertising);                                                 /**< Advertising module instance. */
NRF_BLE_QWR_DEF(m_qwr);                                                             /**< Context for the Queued Write module.*/

// Services
BLE_BAS_DEF(m_bas);                                                                 /**< Structure used to identify the battery service. */
BLE_NUS_DEF(m_nus, NRF_SDH_BLE_TOTAL_LINK_COUNT);                                   /**< BLE NUS service instance. */

static pm_peer_id_t m_peer_to_be_deleted = PM_PEER_ID_INVALID;                      /**< Peer id */
static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID;                            /**< Handle of the current connection */
static uint16_t m_ble_nus_max_data_len = BLE_GATT_ATT_MTU_DEFAULT - 3;            /**< Maximum length of data (in bytes) that can be transmitted
                                                                                         to the peer by the Nordic UART service module */

// Advertising
static ble_uuid_t m_adv_uuids[] = ADV_UUIDS_CONFIG;                                 /**< Universally unique service identifiers */

// Callback
BleStateHandler handleBleState = NULL;

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/



/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/

static void BLE_StackInit(void);
static void BLE_EventHandler(ble_evt_t const *p_ble_evt, void *p_context);

static void BLE_InitGAP(void);
static void BLE_GATTInit(void);
static void BLE_GATTEvents(nrf_ble_gatt_t * p_gatt, nrf_ble_gatt_evt_t const * p_evt);

static void BLE_AdvEvents(ble_adv_evt_t ble_adv_evt);
static void BLE_AdvertisingInit(void);

static void BLE_QWRErrorHandler(uint32_t nrf_error);
static void BLE_NUSReceiveHandler(ble_nus_evt_t * p_evt);
static void BLE_ServicesInit(void);

static void BLE_ConnectParamEvents(ble_conn_params_evt_t * p_evt);
static void BLE_ConnectParamErrorHandler(uint32_t nrf_error);
static void BLE_ConnectParamInit(void);

static void BLE_PeerManagerEvents(pm_evt_t const * p_evt);
static void BLE_PeerManagerInit(void);

/*******************************************************************************/


/************************************* BLE *************************************/

/**
* @func   BLE_EventHandler
* @brief  Function for handling BLE events
* @param  Bluetooth stack event
* @retval None
*/

static void BLE_EventHandler(ble_evt_t const *p_ble_evt, void *p_context){
	ret_code_t err_code;
	pm_handler_secure_on_connection(p_ble_evt);
	
	switch (p_ble_evt->header.evt_id){
		case BLE_GAP_EVT_DISCONNECTED:
			DEBUG_PRINTLN("Disconnected");
			
			m_conn_handle = BLE_CONN_HANDLE_INVALID;
			if(handleBleState != NULL){
				handleBleState(BLE_DISCONNECTED, NULL, 0);
			}
			
			// Check if the last connected peer had not used MITM, if so, delete its bond information.
			if (m_peer_to_be_deleted != PM_PEER_ID_INVALID){
				err_code = pm_peer_delete(m_peer_to_be_deleted);
				APP_ERROR_CHECK(err_code);
				DEBUG_PRINTLN("Collector's bond deleted");
				m_peer_to_be_deleted = PM_PEER_ID_INVALID;
			}
			break;
		
		case BLE_GAP_EVT_CONNECTED:
			DEBUG_PRINTLN("Connected");
			
			m_peer_to_be_deleted = PM_PEER_ID_INVALID;
			m_conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
			err_code = nrf_ble_qwr_conn_handle_assign(&m_qwr, m_conn_handle);
			APP_ERROR_CHECK(err_code);
			break;
		
		case BLE_GAP_EVT_PHY_UPDATE_REQUEST:
		{
			DEBUG_PRINTLN("PHY update request.");
			
			ble_gap_phys_t const phys = {
				.rx_phys = BLE_GAP_PHY_AUTO,
				.tx_phys = BLE_GAP_PHY_AUTO,
			};
			err_code = sd_ble_gap_phy_update(p_ble_evt->evt.gap_evt.conn_handle, &phys);
			APP_ERROR_CHECK(err_code);
		}
			break;

		case BLE_GATTC_EVT_TIMEOUT:
			// Disconnect on GATT Client timeout event.
			DEBUG_PRINTLN("GATT Client Timeout.");
			
			err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gattc_evt.conn_handle,
											 BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
			APP_ERROR_CHECK(err_code);
			break;

		case BLE_GATTS_EVT_TIMEOUT:
			// Disconnect on GATT Server timeout event.
			DEBUG_PRINTLN("GATT Server Timeout.");
			
			err_code = sd_ble_gap_disconnect(p_ble_evt->evt.gatts_evt.conn_handle,
										 BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
			APP_ERROR_CHECK(err_code);
			break;

		case BLE_GAP_EVT_PASSKEY_DISPLAY:
		{
			char passkey[PASSKEY_LENGTH + 1];
			memcpy(passkey, p_ble_evt->evt.gap_evt.params.passkey_display.passkey, PASSKEY_LENGTH);
			passkey[PASSKEY_LENGTH] = 0;
				
			if(handleBleState != NULL){
				handleBleState(BLE_SHOW_PASSKEY, (const uint8_t*) passkey, PASSKEY_LENGTH);
			}
			
			DEBUG_PRINTLN("Passkey %s", passkey);
		}
			break;
		default:
			DEBUG_PRINTLN("BLE event %d", p_ble_evt->header.evt_id);
			break;
	}
}

/**
* @func   BLE_StackInit
* @brief  Initializes the BLE event interrupt
* @param  None
* @retval None
*/

static void BLE_StackInit(void){
	ret_code_t err_code;

	err_code = nrf_sdh_enable_request();
	APP_ERROR_CHECK(err_code);

	// Configure the BLE stack using the default settings.
	// Fetch the start address of the application RAM.
	uint32_t ram_start = 0;
	err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG, &ram_start);
	APP_ERROR_CHECK(err_code);
	
	// Enable BLE stack.
	err_code = nrf_sdh_ble_enable(&ram_start);
	APP_ERROR_CHECK(err_code);

	// Register a handler for BLE events.
	NRF_SDH_BLE_OBSERVER(m_ble_observer, APP_BLE_OBSERVER_PRIO, BLE_EventHandler, NULL);
}

/***************************************************************************************/



/********************************** GAP, GATT *****************************************/


/**@brief Function for the GAP initialization.
 *
 * @details This function sets up all the necessary GAP (Generic Access Profile) parameters of the
 *          device including the device name, appearance, and the preferred connection parameters.
 */

static void BLE_InitGAP(void){
	ret_code_t err_code;
    ble_gap_conn_sec_mode_t sec_mode;
	ble_gap_conn_params_t gap_conn_params = GAP_CONN_PARAMS_CONFIG;
	
	// Set GAP mode is OPEN
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&sec_mode);
    err_code = sd_ble_gap_device_name_set(&sec_mode,
                                          (const uint8_t *)DEVICE_NAME,
                                          strlen(DEVICE_NAME));
    APP_ERROR_CHECK(err_code);

	// Set appearance
    err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_GENERIC_TAG);
    APP_ERROR_CHECK(err_code);

	// Config params
    err_code = sd_ble_gap_ppcp_set(&gap_conn_params);
    APP_ERROR_CHECK(err_code);
}

/**@brief Function for handling events from the GATT library. */
static void BLE_GATTEvents(nrf_ble_gatt_t * p_gatt, nrf_ble_gatt_evt_t const * p_evt){
	if ((m_conn_handle == p_evt->conn_handle) && (p_evt->evt_id == NRF_BLE_GATT_EVT_ATT_MTU_UPDATED)){
		m_ble_nus_max_data_len = p_evt->params.att_mtu_effective - OPCODE_LENGTH - HANDLE_LENGTH;
		DEBUG_PRINTLN("Data length is set to 0x%X(%d)", m_ble_nus_max_data_len, m_ble_nus_max_data_len);
	}
}

/**@brief Function for initializing the GATT module.
 */
static void BLE_GATTInit(void){
    ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, BLE_GATTEvents);
    APP_ERROR_CHECK(err_code);
	
	err_code = nrf_ble_gatt_att_mtu_periph_set(&m_gatt, NRF_SDH_BLE_GATT_MAX_MTU_SIZE);
	APP_ERROR_CHECK(err_code);
}

/***********************************************************************************/



/********************************** Advertising ************************************/

/**@brief Function for handling advertising events.
 *
 * @param[in] ble_adv_evt  Advertising event.
 */

static void BLE_AdvEvents(ble_adv_evt_t ble_adv_evt){
    switch (ble_adv_evt){
	case BLE_ADV_EVT_FAST:
		DEBUG_PRINTLN("Advertising fast");
		
		break; // BLE_ADV_EVT_FAST

	case BLE_ADV_EVT_IDLE:
		DEBUG_PRINTLN("Advertising idle");
		
		if(handleBleState != NULL){
			handleBleState(BLE_IDLE, NULL, 0);
		}
		break; // BLE_ADV_EVT_IDLE

	default:
		break;
    }
}

/**@brief Function for initializing the Advertising functionality.
 *
 * @details Encodes the required advertising data and passes it to the stack.
 *          Also builds a structure to be passed to the stack when starting advertising.
 */

static void BLE_AdvertisingInit(void){
    uint32_t err_code;
    ble_advertising_init_t init = ADV_INIT_CONFIG;
	
    init.advdata.uuids_complete.uuid_cnt = sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]);
    init.advdata.uuids_complete.p_uuids  = m_adv_uuids;
	
	// Set handler
    init.evt_handler = BLE_AdvEvents;

    err_code = ble_advertising_init(&m_advertising, &init);
    APP_ERROR_CHECK(err_code);

    ble_advertising_conn_cfg_tag_set(&m_advertising, APP_BLE_CONN_CFG_TAG);
}

/***********************************************************************************/



/********************************* Service init ************************************/

/**@brief Function for handling Queued Write Module errors.
 *
 * @details A pointer to this function will be passed to each service which may need to inform the
 *          application about an error.
 *
 * @param[in]   nrf_error   Error code containing information about what went wrong.
 */
static void BLE_QWRErrorHandler(uint32_t nrf_error){
    APP_ERROR_HANDLER(nrf_error);
}

/**@brief Function for handling the data from the Nordic UART Service.
 *
 * @details This function will process the data received from the Nordic UART BLE Service and send
 *          it to the UART module.
 *
 * @param[in] p_evt       Nordic UART Service event.
 */
/**@snippet [Handling the data received over BLE] */
static void BLE_NUSReceiveHandler(ble_nus_evt_t * p_evt){
    if (p_evt->type == BLE_NUS_EVT_RX_DATA){
        DEBUG_PRINTLN("NUS receive data");
		
		if(handleBleState != NULL){
			handleBleState(BLE_UART_SERVICE_RECEIVED, p_evt->params.rx_data.p_data, p_evt->params.rx_data.length);
		}
    }
}

/**
* @func   BLE_ServicesInit
* @brief  Initializes the BLE services: Device info, battery, NUS
* @param  None
* @retval None
*/

static void BLE_ServicesInit(void){
	ret_code_t err_code;
	ble_dis_init_t dis_init;
	nrf_ble_qwr_init_t qwr_init = {0};
	
	// Initialize Queued Write Module.
    qwr_init.error_handler = BLE_QWRErrorHandler;

    err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);
    APP_ERROR_CHECK(err_code);
	
	// Initialize Battery Service.
	ble_bas_init_t bas_init = SERVICE_BATTERY_INIT;
    err_code = ble_bas_init(&m_bas, &bas_init);
    APP_ERROR_CHECK(err_code);
	
	// Initialize Device Information Service.
    memset(&dis_init, 0, sizeof(dis_init));

    ble_srv_ascii_to_utf8(&dis_init.manufact_name_str, MANUFACTURER_NAME);
    ble_srv_ascii_to_utf8(&dis_init.serial_num_str, MODEL_NUMBER);

    ble_dis_sys_id_t system_id;
    system_id.manufacturer_id = MANUFACTURER_ID;
    system_id.organizationally_unique_id = ORG_UNIQUE_ID;
    dis_init.p_sys_id = &system_id;

    dis_init.dis_char_rd_sec = SEC_OPEN;

    err_code = ble_dis_init(&dis_init);
    APP_ERROR_CHECK(err_code);
	
	
	// Initialize NUS.
	ble_nus_init_t nus_init;
	memset(&nus_init, 0, sizeof(nus_init));
	nus_init.data_handler = BLE_NUSReceiveHandler;

	err_code = ble_nus_init(&m_nus, &nus_init);
	APP_ERROR_CHECK(err_code);
}

/***********************************************************************************/



/********************************* Connect Params **********************************/

/**@brief Function for handling the Connection Parameter events.
 *
 * @details This function will be called for all events in the Connection Parameters Module which
 *          are passed to the application.
 *          @note All this function does is to disconnect. This could have been done by simply
 *                setting the disconnect_on_fail configuration parameter, but instead we use the
 *                event handler mechanism to demonstrate its use.
 *
 * @param[in] p_evt  Event received from the Connection Parameters Module.
 */
static void BLE_ConnectParamEvents(ble_conn_params_evt_t * p_evt){
    ret_code_t err_code;
	
    if(p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED){
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}


/**@brief Function for handling a Connection Parameters error.
 *
 * @param[in] nrf_error  Error code containing information about what went wrong.
 */
static void BLE_ConnectParamErrorHandler(uint32_t nrf_error){
    APP_ERROR_HANDLER(nrf_error);
}

/**@brief Function for initializing the Connection Parameters module.
 */
static void BLE_ConnectParamInit(void){
    ret_code_t err_code;
    ble_conn_params_init_t cp_init = CONN_PARAMS_CONFIG;
	
	// Set callbacks
    cp_init.evt_handler = BLE_ConnectParamEvents;
    cp_init.error_handler = BLE_ConnectParamErrorHandler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}

/***********************************************************************************/



/****************************** Peer manager ****************************************/

/**@brief Function for handling Peer Manager events.
 *
 * @param[in] p_evt  Peer Manager event.
 */
static void BLE_PeerManagerEvents(pm_evt_t const * p_evt){
    ret_code_t err_code;

    pm_handler_on_pm_evt(p_evt);
    pm_handler_disconnect_on_sec_failure(p_evt);
    pm_handler_flash_clean(p_evt);
	
    switch (p_evt->evt_id){
	case PM_EVT_CONN_SEC_SUCCEEDED:
	{
		pm_conn_sec_status_t conn_sec_status;

		// Check if the link is authenticated (meaning at least MITM).
		err_code = pm_conn_sec_status_get(p_evt->conn_handle, &conn_sec_status);
		APP_ERROR_CHECK(err_code);

		if (conn_sec_status.mitm_protected){
			uint32_t numBonds = pm_peer_count();
			DEBUG_PRINTLN("Number of bonds %d", numBonds);
			
			// If full, delete head id
			if(numBonds > MAX_BONDS_ALLOWED){
				pm_peer_id_t delete_id = pm_next_peer_id_get(PM_PEER_ID_INVALID);
				err_code = pm_peer_delete(delete_id);
				APP_ERROR_CHECK(err_code);
			}
			if(handleBleState != NULL){
				handleBleState(BLE_CONNECTED, NULL, 0);
			}
		}
		else{
			// The peer did not use MITM, disconnect.
			DEBUG_PRINTLN("Collector did not use MITM, disconnecting");
			err_code = pm_peer_id_get(m_conn_handle, &m_peer_to_be_deleted);
			APP_ERROR_CHECK(err_code);
			err_code = sd_ble_gap_disconnect(m_conn_handle,
											 BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
			APP_ERROR_CHECK(err_code);
		}
	}
		break;

	case PM_EVT_CONN_SEC_FAILED:
		err_code = pm_peer_id_get(m_conn_handle, &m_peer_to_be_deleted);
		APP_ERROR_CHECK(err_code);
		err_code = pm_peer_delete(m_peer_to_be_deleted);
		APP_ERROR_CHECK(err_code);
		m_conn_handle = BLE_CONN_HANDLE_INVALID;
		break;

	case PM_EVT_PEERS_DELETE_SUCCEEDED:
		DEBUG_PRINTLN("Peers deleted");
		if(handleBleState != NULL){
			handleBleState(BLE_ALL_PEERS_DELETED, NULL, 0);
		}
		break;

	default:
		break;
    }
}

/**@brief Function for the Peer Manager initialization.
 */
static void BLE_PeerManagerInit(void){
    ble_gap_sec_params_t sec_param = PEER_MANAGER_CONFIG;
    ret_code_t err_code;

    err_code = pm_init();
    APP_ERROR_CHECK(err_code);
	
	// Set params
    err_code = pm_sec_params_set(&sec_param);
    APP_ERROR_CHECK(err_code);
	
	// Register callback handler
    err_code = pm_register(BLE_PeerManagerEvents);
    APP_ERROR_CHECK(err_code);
}

/***********************************************************************************/


/**@brief Function for starting advertising.
 */
void BLE_AdvertisingStart(void){
	ret_code_t err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);
	APP_ERROR_CHECK(err_code);
}

/**@brief Function for stopping advertising.
 */
void BLE_AdvertisingStop(void){
	(void) sd_ble_gap_adv_stop(m_advertising.adv_handle);
}

/**@brief Clear bond information from persistent storage.
 */
void BLE_DeleteBonds(void){
    ret_code_t err_code;

    DEBUG_PRINTLN("Erase bonds");

    err_code = pm_peers_delete();
    APP_ERROR_CHECK(err_code);
}

/**
* @func   BLE_GetNumBonds
* @brief  Get number of bond device
* @param  None
* @retval Number
*/

uint32_t BLE_GetNumBonds(void){
	return pm_peer_count();
}

/**
* @func   BLE_Task
* @brief  If enable LESC, must call this task to respond to LESC-related BLE events
* @param  None
* @retval None
*/

void BLE_Task(void){
	ret_code_t err_code;

	err_code = nrf_ble_lesc_request_handler();
	APP_ERROR_CHECK(err_code);
}

/**
* @func   BLE_Init
* @brief  Init BLE
* @param  None
* @retval None
*/

void BLE_Init(BleStateHandler initFunc){
	BLE_StackInit();
	BLE_InitGAP();
	BLE_GATTInit();
	BLE_AdvertisingInit();
	BLE_ServicesInit();
	BLE_ConnectParamInit();
	BLE_PeerManagerInit();
	
	if(initFunc != NULL){
		handleBleState = initFunc;
	}
}

/**
* @func   : BLE_SendData
* @brief  : Send data to app via ble
* @param  : Data and size
* @retval : None
*/

void BLE_SendData(uint8_t* data, uint16_t length){
	uint32_t err_code;
	
	if(length > m_ble_nus_max_data_len){
		DEBUG_PRINTLN("Data is too long");
		return;
	}
	
	do{
		err_code = ble_nus_data_send(&m_nus, data, &length, m_conn_handle);
		if ((err_code != NRF_ERROR_INVALID_STATE) &&
			(err_code != NRF_ERROR_RESOURCES) &&
			(err_code != NRF_ERROR_NOT_FOUND)){
				APP_ERROR_CHECK(err_code);
		}
	} while (err_code == NRF_ERROR_RESOURCES);
}









