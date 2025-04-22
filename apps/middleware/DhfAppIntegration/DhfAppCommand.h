/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-03-21 18:22:44
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-04-09 15:43:02
 * @FilePath: \AC700N_earphone_release_fn3app\apps\middleware\DhfAppIntegration\DhfAppCommond.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/************************************************************************
Copyright (c) 2025,ShenZhen DHF Electronic Ltd Co.
All rights reserved.

File Name: DhfAppCommond.h
Summary: Control the BLE protocol FN3
Current Version: 1.1.0
Author: samson

Ver 1.0.0, dhf, 2025.03.21
Original version

**************************************************************************/
#ifndef DHFAPPCOMMAND_H
#define DHFAPPCOMMAND_H
typedef enum
{
    //get set 0--99;
    EM_COMM_CONN_STATUS_APP_GET = 0,
    EM_COMM_BATTERY_APP_GET,
    EM_COMM_ANC_MODE_APP_GET,
    EM_COMM_ANC_MODE_APP_SET,
    EM_COMM_EQ_INFO_APP_GET,
    EM_COMM_EQ_INFO_APP_SET,
    EM_COMM_FW_VERSION_APP_GET,
    EM_COMM_BTN_CONFIG_APP_GET,
    EM_COMM_BTN_CONFIG_APP_SET,
    EM_COMM_DEFAULT_BTN_CONFIG_APP_SET,
    EM_COMM_GAME_MODE_APP_GET,
    EM_COMM_GAME_MODE_APP_SET,
    EM_COMM_BASS_MODE_APP_GET,
    EM_COMM_BASS_MODE_APP_SET,
    EM_COMM_BATCH_NUMBER_APP_GET,
    EM_COMM_SN_NUMBER_APP_GET,
    EM_COMM_FIND_DEVICE_APP_SET,
    EM_COMM_RESET_DEVICE_APP_SET,
    EM_COMM_MULTI_CONN_MODE_APP_GET,
    EM_COMM_MULTI_CONN_MODE_APP_SET,
    //send  101--199
    EM_COMM_CONN_STATUS_CHANGE_DEVICE_SEND = 100,
    EM_COMM_BATTERY_CHANGE_DEVICE_SEND,
    EM_COMM_ANC_CHANGE_DEVICE_SEND,
    EM_COMM_GAME_MODE_CHANGE_DEVICE_SEND,
    EM_COMM_BASS_MODE_CHANGE_DEVICE_SEND,
}DHFAPPCOMMANID_E;

typedef enum
{          
    EM_CMD_DIR_APP_REQ                = 0x00,       
    EM_CMD_DIR_APP_ANSWER             = 0x01,
    EM_CMD_DIR_DEVICE_REQ             = 0x10,
    EM_CMD_DIR_DEVICE_ANSWER          = 0x11,
}DHFAPPCOMMANDIR_E;



/// @brief 
/// @param  
/// @return 
int DHFAppCommand_init(void);

/// @brief 
/// @param uData 
/// @param uDatalen 
/// @return 
int DHFAPCommand_DataPut(unsigned char *uData,int uDatalen);

/// @brief 
/// @param eCommandType 
/// @return 
int DhfAppCommand_DataNotify(DHFAPPCOMMANID_E eCommandType);
#endif