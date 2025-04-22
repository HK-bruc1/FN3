/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-03-21 18:36:24
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-04-15 15:50:38
 * @FilePath: \AC700N_earphone_release_fn3app\apps\middleware\DhfAppIntegration\DhfAppCommFn3.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/************************************************************************
Copyright (c) 2025,ShenZhen DHF Electronic Ltd Co.
All rights reserved.

File Name: DhfAppCommFn3.h
Summary: Control the BLE protocol FN3
Current Version: 1.1.0
Author: samson

Ver 1.0.0, dhf, 2025.03.21
Original version

**************************************************************************/
#ifndef DHFAPPCOMMNFN3.h
#define DHFAPPCOMMNFN3.h

#define   FN3_COMM_CONN_STATUS_APP_GET 1001
#define   FN3_COMM_BATTERY_APP_GET    1002
#define   FN3_COMM_ANC_MODE_APP_GET   1003
#define   FN3_COMM_ANC_MODE_APP_SET    1004
#define   FN3_COMM_EQ_INFO_APP_GET     1005
#define   FN3_COMM_EQ_INFO_APP_SET      1006
#define   FN3_COMM_FW_VERSION_APP_GET    1007
#define   FN3_COMM_BTN_CONFIG_APP_GET    1008
#define   FN3_COMM_BTN_CONFIG_APP_SET    1009
#define   FN3_COMM_DEFAULT_BTN_CONFIG_APP_SET  1010
#define   FN3_COMM_GAME_MODE_APP_GET    1011
#define   FN3_COMM_GAME_MODE_APP_SET    1012
#define   FN3_COMM_BASS_MODE_APP_GET    1013
#define   FN3_COMM_BASS_MODE_APP_SET    1014
#define   FN3_COMM_BATCH_NUMBER_APP_GET  1015
#define   FN3_COMM_SN_NUMBER_APP_GET     1016
#define   FN3_COMM_FIND_DEVICE_APP_SET   1017
#define   FN3_COMM_RESET_DEVICE_APP_SET        1018
#define   FN3_COMM_COMM_MULTI_CONN_MODE_APP_GET  1019
#define   FN3_COMM_COMM_MULTI_CONN_MODE_APP_SET  1020
#define   FN3_COMM_COMM_CONN_STATUS_CHANGE_DEVICE_SEND  2001
#define   FN3_COMM_COMM_BATTERY_CHANGE_DEVICE_SEND      2002
#define   FN3_COMM_COMM_ANC_CHANGE_DEVICE_SEND          2003
#define   FN3_COMM_COMM_GAME_MODE_CHANGE_DEVICE_SEND    2004
#define   FN3_COMM_COMM_BASS_MODE_CHANGE_DEVICE_SEND    2005


#define  FN3_CMD_DIR_APP_REQ            0x11     
#define  FN3_CMD_DIR_APP_ANSWER         0x12
#define  FN3_CMD_DIR_DEVICE_REQ         0x21
#define  FN3_CMD_DIR_DEVICE_ANSWER      0x22

typedef enum
{          
    EM_CMD_ID  = 1,       
    EM_CMD_DIR,              
    EM_CMD_MSG_ID,
    EM_CMD_RESPONSE_CODE,
    EM_CMD_PAYLOAD_LEN,
    EM_CMD_PAYLOAD_BUF,
    EM_CMD_CHECK,
}FIELDTYPE_E;

typedef enum {
    EM_ANC_OFF = 0,		    /*关闭模式*/
    EM_ANC_ON,				/*降噪模式*/
    EM_ANC_TRANSPARENCY,	/*通透模式*/
    EM_ANC_REDUCE_WIND_NOISE_ON,	/*防风噪模式*/
    EM_ANC_LEISURE_MODE_ON,			/*休闲模式*/
    EM_ANC_ADAPTIVE_MODE_ON,	    /*自适应模式*/
    EM_ANC_CUSTOM_ANC_MODE_ON,     /*自定义*/
}FN3ANC_MODE_E;

typedef enum {
    EM_IIR_TYPE_LOW_SHELF = 0x00,
    EM_IIR_TYPE_PEAK,
    EM_IIR_TYPE_HIGH_SHELF,
    EM_IIR_TYPE_LOW_PASS,
    EM_IIR_TYPE_BAND_PASS,
    EM_IIR_TYPE_HIGH_PASS,
} FN3EQ_IIR_TYPE;
typedef enum {
    FN3_KEY_TYPE_NULL = 0x1,
    FN3_KEY_TYPE_PP,
    FN3_KEY_TYPE_NEXT,
    FN3_KEY_TYPE_PREV,
    FN3_KEY_TYPE_INC_VOICE,
    FN3_KEY_TYPE_DESC_VOICE,
    FN3_KEY_TYPE_ANC_VOICE,
    FN3_KEY_TYPE_ANC_CLOSE_VOICE,
    FN3_KEY_TYPE_LOW_VOICE,
    FN3_KEY_TYPE_LOW_LANTECY,
    FN3_KEY_TYPE_SIRI,
    FN3_KEY_TYPE_ANSWER_CALL,
}FN3KEY_KEY_E;


BOOL DhfAppComm_PaserData(UCHAR *ucSrcBuf,int uSrcBuflen);
BOOL DhfAppComm_DataNotify(DHFAPPCOMMANID_E e_CmdType);
#endif