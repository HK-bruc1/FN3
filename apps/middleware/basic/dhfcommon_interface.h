/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-03-27 17:09:33
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-04-19 10:48:53
 * @FilePath: \AC700N_earphone_release_fn3app\apps\middleware\basic\dhfcommon_interface.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/************************************************************************
Copyright (c) 2025,ShenZhen DHF Electronic Ltd Co.
All rights reserved.

File Name: dhfcommon_interface.h
Summary:define all data type
Current Version: 1.1.0
Author: samson

Ver 1.0.0, dhf, 2025.03.21
Original version

**************************************************************************/
#ifndef _DHFCOMMON_INTERFACE_H_
#define _DHFCOMMON_INTERFACE_H_
#include "dhf_types.h"
#include "..\DhfAppIntegration\DhfAppCommand.h"

typedef enum
{          
    EM_EAR_CONN_STATUS                = 0x00,       
    EM_EAR_BATTERY,                    
    EM_EAR_ANC_MODE,  
    EM_EAR_EQ_INFO,
    EM_EAR_FW_VERSION,
    EM_EAR_BTN_CONFIG,
    EM_EAR_DEFAULT_BTN_CONFIG,
    EM_EAR_GAME_MODE,
    EM_EAR_BASS_MODE,
    EM_EAR_BATCH_NUMBER,
    EM_EAR_SN_NUMBER,
    EM_EAR_FIND_DEVICE,
    EM_EAR_RESET_DEVICE,
    EM_EAR_MULTI_CONN_MODE,         
}DHFEARDATA_E;


typedef enum  {
    DHF_KEY_TYPE_NULL = 0x0,
    DHF_KEY_TYPE_POWER_ON,
    DHF_KEY_TYPE_POWER_OFF,
    DHF_KEY_TYPE_PREV = 0x3,
    DHF_KEY_TYPE_NEXT,
    DHF_KEY_TYPE_PP,
    DHF_KEY_TYPE_ANSWER_CALL,
    DHF_KEY_TYPE_HANG_UP,
    DHF_KEY_TYPE_CALL_BACK,
    DHF_KEY_TYPE_INC_VOICE,
    DHF_KEY_TYPE_DESC_VOICE,
    DHF_KEY_TYPE_TAKE_PHOTO,
    DHF_KEY_TYPE_SIRI,
	DHF_KEY_TYPE_LOW,
    DHF_KEY_TYPE_ANC_VOICE = 0xFF,
}FHDEARKEY_E;

typedef enum
{          
    EM_RCSP_BLE             = 0x00,       
    EM_RCSP_SPP,                    
}DHFRCSPTYPE_E;
/*
函数：DHFCommon_Geteardata
描述：获取耳机的某个参数数据
输入：
	eEardatatype - 耳机参数类型
输出:
	Outdata - 输出的耳机参数指针
返回：
	TRUE - 函数执行成功
	FALSE - 函数执行失败
*/

BOOL DHFCommon_Geteardata(DHFEARDATA_E eEardatatype,void* Outdata);

/*
DHFCommon_Seteardata
描述：设置耳机的某个参数数据
输入：
	eEardatatype - 耳机参数类型
	Indata - 设置的耳机参数指针
返回：
	TRUE - 函数执行成功
	FALSE - 函数执行失败
*/
BOOL DHFCommon_Seteardata(DHFEARDATA_E eEardatatype,void* Indata);
/*
DHFCommon_Datasend
描述：数据传输
输入：
	priv - 私有数据
	buf - 数据内容
    len-数据长度
返回：
	TRUE - 函数执行成功
	FALSE - 函数执行失败
*/
BOOL DHFCommon_Datasend(void *priv, UINT8 *buf, UINT16 len);

/*
Dhfcommon_GetEarMaster
描述：获取当前耳机是否为主耳机
输入：

返回：
	TRUE - 函数执行成功
	FALSE - 函数执行失败
*/
BOOL  DHFCommon_GetEarMaster(void);
#endif