/*
 * BQ27542G1.h
 *
 *  Created on: September 7, 2020
 *      Author: LongHD
 */
/******************************************************************************/

#ifndef _SOURCE_COMPONENTS_BQ27542G1_BQ27542G1_H_
#define _SOURCE_COMPONENTS_BQ27542G1_BQ27542G1_H_

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include "Config.h"

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/

#define BQ27542G1_SLAVE_ID                        0xAA
#define BQ27542G1_DEVICE_TYPE                     0x0542


#define BQ27542G1_CMD_CNTL                        0x00
#define BQ27542G1_CMD_AR                          0x02
#define BQ27542G1_CMD_ARTTE                       0x04
#define BQ27542G1_CMD_TEMP                        0x06
#define BQ27542G1_CMD_VOLT                        0x08
#define BQ27542G1_CMD_FLAGS                       0x0A
#define BQ27542G1_CMD_NAC                         0x0C
#define BQ27542G1_CMD_FAC                         0x0E
#define BQ27542G1_CMD_RM                          0x10
#define BQ27542G1_CMD_FCC                         0x12
#define BQ27542G1_CMD_AI                          0x14
#define BQ27542G1_CMD_TTE                         0x16
#define BQ27542G1_CMD_TTF                         0x18
#define BQ27542G1_CMD_SI                          0x1A
#define BQ27542G1_CMD_STTE                        0x1C
#define BQ27542G1_CMD_MLI                         0x1E
#define BQ27542G1_CMD_MLTTE                       0x20
#define BQ27542G1_CMD_AE                          0x22
#define BQ27542G1_CMD_AP                          0x24
#define BQ27542G1_CMD_TTECP                       0x26
#define BQ27542G1_CMD_CC                          0x2A
#define BQ27542G1_CMD_SOC                         0x2C
#define BQ27542G1_CMD_CCURR                       0x32

#define BQ27542G1_CNTLDATA_CONTROL_STATUS         0x0000
#define BQ27542G1_CNTLDATA_DEVICE_TYPE            0x0001
#define BQ27542G1_CNTLDATA_FW_VERSION             0x0002
#define BQ27542G1_CNTLDATA_DF_CHECKSUM            0x0004
#define BQ27542G1_CNTLDATA_RESET_DATA             0x0005
#define BQ27542G1_CNTLDATA_PREV_MACWRITE          0x0007
#define BQ27542G1_CNTLDATA_CHEM_ID                0x0008
#define BQ27542G1_CNTLDATA_BATL_ENABLE            0x000D
#define BQ27542G1_CNTLDATA_BATL_DISABLE           0x000E
#define BQ27542G1_CNTLDATA_SET_HIBERNATE          0x0011
#define BQ27542G1_CNTLDATA_CLEAR_HIBERNATE        0x0012
#define BQ27542G1_CNTLDATA_SET_SLEEP_PLUS         0x0013
#define BQ27542G1_CNTLDATA_CLEAR_SLEEP_PLUS       0x0014
#define BQ27542G1_CNTLDATA_SEALED                 0x0020
#define BQ27542G1_CNTLDATA_IT_ENABLE              0x0021
#define BQ27542G1_CNTLDATA_CAL_MODE               0x0040
#define BQ27542G1_CNTLDATA_RESET                  0x0041

//#define BQ27542G1_ReadBatteryVoltage()            BQ27542G1_ReadWord(BQ27542G1_CMD_VOLT)
//#define BQ27542G1_ReadBatteryCurrent()            BQ27542G1_ReadWord(BQ27542G1_CMD_AI)

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/



/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/



/******************************************************************************/
/*                                FUNCTIONS                                   */
/******************************************************************************/

bool BQ27542G1_CheckDeviceType(void);
uint16_t BQ27542G1_ReadWord(uint8_t command);


#define BQ25892_SLAVE_ID                0xD6
#define BQ25892_CHARGE_STATUS_REG       0x0B

enum{
	CHARGE_STATUS_NONE = 0,
	CHARGE_STATUS_CHARGING,
	CHARGE_STATUS_DONE
};

uint8_t BQ25892_ChargeState(void);

/******************************************************************************/

#endif /* _SOURCE_COMPONENTS_BQ27542G1_BQ27542G1_H_ */
