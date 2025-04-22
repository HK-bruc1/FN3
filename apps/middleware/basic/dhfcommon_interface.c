/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-03-27 16:57:13
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-04-19 10:57:11
 * @FilePath: \AC700N_earphone_release_fn3app\apps\middleware\basic\dhfcommon_interface.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/************************************************************************
Copyright (c) 2025,ShenZhen DHF Electronic Ltd Co.
All rights reserved.

File Name: dhfcommon_interface.c
Summary:define all data type
Current Version: 1.1.0
Author: samson

Ver 1.0.0, dhf, 2025.03.21
Original version

**************************************************************************/
#include "rcsp_adv_bluetooth.h"
#include "le_rcsp_adv_module.h"
#include "dhfcommon_interface.h"
#include "custom_cfg.h"
#include "bt_tws.h"
#include "audio_anc.h"
#include "hw_eq.h"
#include "application/eq_config.h"
#include "dhf_os.h"
#include "app_main.h"
#include "dhfAPPcommFn3.h"
#include "adv_key_setting.h"
#define APPKEYMAX  12

static DHFRCSPTYPE_E eDHFProtocol = EM_RCSP_BLE;
static UINT8 s_sApkeyPairlist[APPKEYMAX][2]=
{
   {DHF_KEY_TYPE_NULL,FN3_KEY_TYPE_NULL},
   {DHF_KEY_TYPE_PP,FN3_KEY_TYPE_PP},
   {DHF_KEY_TYPE_NEXT,FN3_KEY_TYPE_NEXT},
   {DHF_KEY_TYPE_PREV,FN3_KEY_TYPE_PREV},
   {DHF_KEY_TYPE_INC_VOICE,FN3_KEY_TYPE_INC_VOICE},
   {DHF_KEY_TYPE_DESC_VOICE,FN3_KEY_TYPE_DESC_VOICE},
   {DHF_KEY_TYPE_ANC_VOICE,FN3_KEY_TYPE_ANC_VOICE},
   {DHF_KEY_TYPE_LOW,FN3_KEY_TYPE_LOW_LANTECY},
   {DHF_KEY_TYPE_SIRI,FN3_KEY_TYPE_SIRI},
   {DHF_KEY_TYPE_LOW+1,FN3_KEY_TYPE_ANC_CLOSE_VOICE},
   {DHF_KEY_TYPE_LOW+2,FN3_KEY_TYPE_LOW_VOICE},
   {DHF_KEY_TYPE_LOW+3,FN3_KEY_TYPE_ANSWER_CALL},
};

static UINT8 s_sApEqTypePairlist[5][2]=
{
   {EQ_IIR_TYPE_LOW_SHELF,EM_IIR_TYPE_LOW_SHELF},
   {EQ_IIR_TYPE_BAND_PASS,EM_IIR_TYPE_PEAK},
   {EQ_IIR_TYPE_HIGH_SHELF,EM_IIR_TYPE_HIGH_SHELF},
   {EQ_IIR_TYPE_LOW_PASS,EM_IIR_TYPE_LOW_PASS},
   {EQ_IIR_TYPE_HIGH_PASS,EM_IIR_TYPE_HIGH_PASS},
};

BOOL  DHFCommon_GetEarMaster(void)
{
      if (get_tws_sibling_connect_state() == TRUE) 
      {
          if(tws_api_get_role() == TWS_ROLE_MASTER)
          {
               return TRUE;
          }
     }
     return FALSE;
}

static BOOL  Dhfcommon_iGetEarConnstatus(UINT8 *uLear,UINT8 *uRear)
{
     UINT8 channel = 'U';
    *uLear = 0;//test samson
    *uRear = 0;//tset  samson
     if (get_tws_sibling_connect_state() == TRUE) 
     {
        *uLear = 1;
        *uRear = 1;
        return 1;
     }
     else 
     {
        if (get_tws_phone_connect_state()) 
        {
            channel = tws_api_get_local_channel();
            if ('L' == channel) {
                *uLear = 1;
            } else {
                *uRear = 1;
            }
            return 1;
       }
        
    }

    return 0;
}

static BOOL  Dhfcommon_iGetEarBattery(UINT8 *pucBattery)
{
    bt_adv_get_bat(pucBattery);
    return 1;
}
    
static BOOL DhfCommon_iGetANCMode(UINT8 *pucAncMode)
{
    #if 1
    UINT8 ucAncMode = 0;
    UINT8 ucAncbuf[25] ={0};
    anc_voice_info_get(ucAncbuf,25);
      ucAncMode = anc_mode_get();
      DHF_OS_Debug("DhfCommon_iGetANCMode-------samson-----ucAncMode = %d,%d\n",ucAncMode,ucAncbuf[0]);
      if((ucAncMode-1) != ucAncbuf[0])
      {
           ucAncMode = ucAncbuf[0]+1;
          anc_mode_switch(ucAncMode,0);
      }
      
    if((ucAncMode > 0) && (ucAncMode < 4))
    {
      pucAncMode[0] = ucAncMode-1;
    }
    else
    {
        pucAncMode[0] = 0x06;
    }

     ucAncMode = anc_mic_gain_get();///参考mic	  			range 0-19;  default 10
     pucAncMode[1] = ucAncMode/2;
     pucAncMode[2] = 0;
     #endif
     return 1;
}

static BOOL  DhfCommon_iReSetANCMode(void)
{
    UINT8 ucAncbuf[25] ={0};
    UINT8 ucAncMode = 0;
    ucAncMode = 1;
    anc_voice_info_get(ucAncbuf,25);
    ucAncbuf[0] = 0;
    anc_voice_info_set(ucAncbuf,25);
    anc_mode_switch(ucAncMode,0);
     return 1;
   
}

static BOOL  DhfCommon_iSetANCMode(UINT8 *pucAncMode)
{
    UINT8 ucAncbuf[25] ={0};
    UINT8 ucAncMode = 0;
    ucAncMode = pucAncMode[0];
    anc_voice_info_get(ucAncbuf,25);
    DHF_OS_Debug("DhfCommon_iSetANCMode-------samson-----pucAncMode = %d,%d\n",ucAncbuf[0],pucAncMode[0]);
    ucAncbuf[0] = pucAncMode[0];
    anc_voice_info_set(ucAncbuf,25);
    if(pucAncMode[0] < 3)
    {
     ucAncMode = pucAncMode[0]+1;
    }
    else
    {
       ucAncMode = 0x04;
    }
     anc_mode_switch(ucAncMode,0);
     ucAncMode = pucAncMode[1]*2;///参考mic	  			range 0-19;  default 10
     anc_cfg_btspp_update(ANC_REF_MIC_GAIN_SET, ucAncMode);
     return 1;
   
}
static BOOL DhfCommon_iGetFwVersion(UINT8 *pucFwVersion)
{
    UINT16 usFwversion = 0;
    usFwversion = get_vid_pid_ver_from_cfg_file(GET_VER_FROM_EX_CFG);
    pucFwVersion[0] = 1;
    pucFwVersion[1] = (UINT8)((usFwversion&0xFF00)>>8);
    pucFwVersion[2] =  (UINT8)(usFwversion&0x00FF);
    return 1;
}

static BOOL DhfCommon_iGetGameMode(UINT8 *pucGameMode)
{
    UINT8 ucGamemode = 0;
    ucGamemode = bt_get_low_latency_mode();
    *pucGameMode = ucGamemode;
    return 1;
}

static BOOL DhfCommon_iSetGameMode(UINT8 pucGameMode)
{
   bt_set_low_latency_mode(pucGameMode);
   return 1;
}

static BOOL DhfCommon_iGetMULTI_CONN_MODE(UINT8 *pucMULTI_CONN_MODE)
{
    // *pucMULTI_CONN_MODE = 1;
     return 0;
}
static BOOL DhfCommon_iGetSn_Number(UINT8 *pucSn_Number)
{
    UINT8 anSn[0x10] ={
     0x32,0x34,0x31,0x30,
     0x41,0x4A,0x51,0x38,
     0x30,0x30,0x30,0x31,
     0x30,0x30,0x30,0x31};
     DHF_MEMCPY(pucSn_Number,0x10,anSn);
     return 1;
}
static BOOL DhfCommon_iSetMULTI_CONN_MODE(UINT8 *pucMULTI_CONN_MODE)
{
    return 0;
}
extern u8 audio_bass_onoff(void);
extern void audio_bass_ctrl();

static BOOL DhfCommon_iGetBASS_MODE(UINT8 *pucBASS_MODE)
{
     #if 0
     *pucBASS_MODE = (UINT8)audio_bass_onoff();
     #endif
     //待实现
     return 0;
}
static BOOL DhfCommon_iSetBASS_MODE(UINT8 *pucBASS_MODE)
{
     #if 0
     if(*pucBASS_MODE!=(UINT8)audio_bass_onoff())
     {
          audio_bass_ctrl();
     }
     #endif
     //待实现
     return 0;
}
static BOOL DhfCommon_iGetBACTH_MODE(UINT8 *pucBACTH_MODE)
{
     //待实现
     *pucBACTH_MODE = 0x0A;
     return 1;
}

static UINT8 DhfCommon_iGetDhfkeyByPriKey(UINT8 ucPrikey)
{
   UINT8 i = 0;
   for(i = 0;i < APPKEYMAX;i++)
   {
      if(s_sApkeyPairlist[i][1] == ucPrikey)
      {
        return s_sApkeyPairlist[i][0];
      }
    
   }

}

static UINT8 DhfCommon_iGetPrikeyByDhfKey(UINT8 ucDhfkey)
{
   UINT8 i = 0;
   for(i = 0;i < APPKEYMAX;i++)
   {
      if(s_sApkeyPairlist[i][0] == ucDhfkey)
      {
        return s_sApkeyPairlist[i][1];
      }
    
   }

}
static BOOL DhfCommon_iSetBTN_CONFIG(UINT8 *pucBTN_CONFIG)
{
     //待实现
     UINT8 ucKeylist[8] = {0};
     ucKeylist[0] = DhfCommon_iGetDhfkeyByPriKey(pucBTN_CONFIG[0]);
     ucKeylist[1] = DhfCommon_iGetDhfkeyByPriKey(pucBTN_CONFIG[1]);
     ucKeylist[3] = DhfCommon_iGetDhfkeyByPriKey(pucBTN_CONFIG[2]);
     ucKeylist[2] = DhfCommon_iGetDhfkeyByPriKey(pucBTN_CONFIG[3]);

     ucKeylist[4] = DhfCommon_iGetDhfkeyByPriKey(pucBTN_CONFIG[6]);
     ucKeylist[5] = DhfCommon_iGetDhfkeyByPriKey(pucBTN_CONFIG[7]);
     ucKeylist[7] = DhfCommon_iGetDhfkeyByPriKey(pucBTN_CONFIG[8]);
     ucKeylist[6] = DhfCommon_iGetDhfkeyByPriKey(pucBTN_CONFIG[9]);
     set_key_setting(ucKeylist);
     deal_key_setting(NULL,1,1);
     return 1;
}

static BOOL DhfCommon_iReSetDefaultBTN_CONFIG(void)
{
     UINT8 ucKeylist[8] = {0};
     UINT8 pucBTN_CONFIG[0x30] ={0};
     //待实现
      /*
    //1col 01:左耳 02 右耳；2col key 1:单击 2双击 3 长按 4三击；3col event;
    .key_setting = {
        0x01, 0x01, 0x05, \ 2
        0x02, 0x01, 0x05, \
        0x01, 0x02, 0x03, \4
        0x02, 0x02, 0x04, \3

        0x01, 0x03, 0x0c, \11
        0x02, 0x03, 0x0c, \11
        0x01, 0x04, 0x0a, \6
        0x02, 0x04, 0x09   \5
    },
    */
 //待实现
 //左耳
     pucBTN_CONFIG[0] = 0x7F;
     pucBTN_CONFIG[1] = 0x06;
     pucBTN_CONFIG[2] = 0x00;

     pucBTN_CONFIG[3] = 0x7F;
     pucBTN_CONFIG[4] = 0x06;
     pucBTN_CONFIG[5] = 0x00;
       
     pucBTN_CONFIG[6] = 0x7F;
     pucBTN_CONFIG[7] = 0x06;
     pucBTN_CONFIG[8] = 0x00;  

     pucBTN_CONFIG[9]  = 0x7F;
     pucBTN_CONFIG[10] = 0x06;
     pucBTN_CONFIG[11] = 0x00; 

     pucBTN_CONFIG[12] = 0x00;
     pucBTN_CONFIG[13] = 0x00;
     pucBTN_CONFIG[14] = 0x00;  
     pucBTN_CONFIG[15] = 0x00;
     pucBTN_CONFIG[16] = 0x00;
     pucBTN_CONFIG[17] = 0x00; 
     pucBTN_CONFIG[18] = 0x02;  
     pucBTN_CONFIG[19] = 0x04;
     pucBTN_CONFIG[20] = 0x06;
     pucBTN_CONFIG[21] = 0x07; 
     pucBTN_CONFIG[22] = 0x00;
     pucBTN_CONFIG[23] = 0x00; 

     pucBTN_CONFIG[24] = 0x7F;
     pucBTN_CONFIG[25] = 0x06;
     pucBTN_CONFIG[26] = 0x00;

     pucBTN_CONFIG[27] = 0x7F;
     pucBTN_CONFIG[28] = 0x06;
     pucBTN_CONFIG[29] = 0x00;
       
     pucBTN_CONFIG[30] = 0x7F;
     pucBTN_CONFIG[31] = 0x06;
     pucBTN_CONFIG[32] = 0x00;  

     pucBTN_CONFIG[33] = 0x7F;
     pucBTN_CONFIG[34] = 0x06;
     pucBTN_CONFIG[35] = 0x00; 

     pucBTN_CONFIG[36] = 0x00;
     pucBTN_CONFIG[37] = 0x00;
     pucBTN_CONFIG[38] = 0x00;  
     pucBTN_CONFIG[39] = 0x00;
     pucBTN_CONFIG[40] = 0x00;
     pucBTN_CONFIG[41] = 0x00; 
     pucBTN_CONFIG[42] = 0x02;  
     pucBTN_CONFIG[43] = 0x03;
     pucBTN_CONFIG[44] = 0x05;
     pucBTN_CONFIG[45] = 0x07; 
     pucBTN_CONFIG[46] = 0x00;
     pucBTN_CONFIG[47] = 0x00; 
   
     ucKeylist[0] = DhfCommon_iGetDhfkeyByPriKey(pucBTN_CONFIG[18]);
     ucKeylist[1] = DhfCommon_iGetDhfkeyByPriKey(pucBTN_CONFIG[19]);
     ucKeylist[3] = DhfCommon_iGetDhfkeyByPriKey(pucBTN_CONFIG[20]);
     ucKeylist[2] = DhfCommon_iGetDhfkeyByPriKey(pucBTN_CONFIG[21]);

     ucKeylist[4] = DhfCommon_iGetDhfkeyByPriKey(pucBTN_CONFIG[42]);
     ucKeylist[5] = DhfCommon_iGetDhfkeyByPriKey(pucBTN_CONFIG[43]);
     ucKeylist[7] = DhfCommon_iGetDhfkeyByPriKey(pucBTN_CONFIG[44]);
     ucKeylist[6] = DhfCommon_iGetDhfkeyByPriKey(pucBTN_CONFIG[45]);
     set_key_setting(ucKeylist);
     deal_key_setting(NULL,1,1);
     return 1;
}

static BOOL DhfCommon_iSetDefaultBTN_CONFIG(UINT8 *pucBTN_CONFIG)
{
     UINT8 ucKeylist[8] = {0};
     //待实现
      /*
    //1col 01:左耳 02 右耳；2col key 1:单击 2双击 3 长按 4三击；3col event;
    .key_setting = {
        0x01, 0x01, 0x05, \ 2
        0x02, 0x01, 0x05, \
        0x01, 0x02, 0x03, \4
        0x02, 0x02, 0x04, \3

        0x01, 0x03, 0x0c, \11
        0x02, 0x03, 0x0c, \11
        0x01, 0x04, 0x0a, \6
        0x02, 0x04, 0x09   \5
    },
    */
 //待实现
 //左耳
     pucBTN_CONFIG[0] = 0x7F;
     pucBTN_CONFIG[1] = 0x06;
     pucBTN_CONFIG[2] = 0x00;

     pucBTN_CONFIG[3] = 0x7F;
     pucBTN_CONFIG[4] = 0x06;
     pucBTN_CONFIG[5] = 0x00;
       
     pucBTN_CONFIG[6] = 0x7F;
     pucBTN_CONFIG[7] = 0x06;
     pucBTN_CONFIG[8] = 0x00;  

     pucBTN_CONFIG[9]  = 0x7F;
     pucBTN_CONFIG[10] = 0x06;
     pucBTN_CONFIG[11] = 0x00; 

     pucBTN_CONFIG[12] = 0x00;
     pucBTN_CONFIG[13] = 0x00;
     pucBTN_CONFIG[14] = 0x00;  
     pucBTN_CONFIG[15] = 0x00;
     pucBTN_CONFIG[16] = 0x00;
     pucBTN_CONFIG[17] = 0x00; 
     pucBTN_CONFIG[18] = 0x02;  
     pucBTN_CONFIG[19] = 0x04;
     pucBTN_CONFIG[20] = 0x06;
     pucBTN_CONFIG[21] = 0x07; 
     pucBTN_CONFIG[22] = 0x00;
     pucBTN_CONFIG[23] = 0x00; 

     pucBTN_CONFIG[24] = 0x7F;
     pucBTN_CONFIG[25] = 0x06;
     pucBTN_CONFIG[26] = 0x00;

     pucBTN_CONFIG[27] = 0x7F;
     pucBTN_CONFIG[28] = 0x06;
     pucBTN_CONFIG[29] = 0x00;
       
     pucBTN_CONFIG[30] = 0x7F;
     pucBTN_CONFIG[31] = 0x06;
     pucBTN_CONFIG[32] = 0x00;  

     pucBTN_CONFIG[33] = 0x7F;
     pucBTN_CONFIG[34] = 0x06;
     pucBTN_CONFIG[35] = 0x00; 

     pucBTN_CONFIG[36] = 0x00;
     pucBTN_CONFIG[37] = 0x00;
     pucBTN_CONFIG[38] = 0x00;  
     pucBTN_CONFIG[39] = 0x00;
     pucBTN_CONFIG[40] = 0x00;
     pucBTN_CONFIG[41] = 0x00; 
     pucBTN_CONFIG[42] = 0x02;  
     pucBTN_CONFIG[43] = 0x03;
     pucBTN_CONFIG[44] = 0x05;
     pucBTN_CONFIG[45] = 0x07; 
     pucBTN_CONFIG[46] = 0x00;
     pucBTN_CONFIG[47] = 0x00; 
   
     ucKeylist[0] = DhfCommon_iGetDhfkeyByPriKey(pucBTN_CONFIG[18]);
     ucKeylist[1] = DhfCommon_iGetDhfkeyByPriKey(pucBTN_CONFIG[19]);
     ucKeylist[3] = DhfCommon_iGetDhfkeyByPriKey(pucBTN_CONFIG[20]);
     ucKeylist[2] = DhfCommon_iGetDhfkeyByPriKey(pucBTN_CONFIG[21]);

     ucKeylist[4] = DhfCommon_iGetDhfkeyByPriKey(pucBTN_CONFIG[42]);
     ucKeylist[5] = DhfCommon_iGetDhfkeyByPriKey(pucBTN_CONFIG[43]);
     ucKeylist[7] = DhfCommon_iGetDhfkeyByPriKey(pucBTN_CONFIG[44]);
     ucKeylist[6] = DhfCommon_iGetDhfkeyByPriKey(pucBTN_CONFIG[45]);
     set_key_setting(ucKeylist);
     deal_key_setting(NULL,1,1);
     return 1;
}

static BOOL  DhfCommon_iGetBTN_CONFIG(UINT8 *pucBTN_CONFIG)
{
    UINT8 ucKeylist[8] = {0};
    get_key_setting(ucKeylist);
     //待实现
     pucBTN_CONFIG[0] = 0x7F;
     pucBTN_CONFIG[1] = 0x06;
     pucBTN_CONFIG[2] = 0x00;

     pucBTN_CONFIG[3] = 0x7F;
     pucBTN_CONFIG[4] = 0x06;
     pucBTN_CONFIG[5] = 0x00;
       
     pucBTN_CONFIG[6] = 0x7F;
     pucBTN_CONFIG[7] = 0x06;
     pucBTN_CONFIG[8] = 0x00;  

     pucBTN_CONFIG[9]  = 0x7F;
     pucBTN_CONFIG[10] = 0x06;
     pucBTN_CONFIG[11] = 0x00; 

     pucBTN_CONFIG[12] = 0x00;
     pucBTN_CONFIG[13] = 0x00;
     pucBTN_CONFIG[14] = 0x00;  
     pucBTN_CONFIG[15] = 0x00;
     pucBTN_CONFIG[16] = 0x00;
     pucBTN_CONFIG[17] = 0x00; 
     pucBTN_CONFIG[18] = DhfCommon_iGetPrikeyByDhfKey(ucKeylist[0]);  
     pucBTN_CONFIG[19] = DhfCommon_iGetPrikeyByDhfKey(ucKeylist[1]);  
     pucBTN_CONFIG[20] = DhfCommon_iGetPrikeyByDhfKey(ucKeylist[3]);  
     pucBTN_CONFIG[21] = DhfCommon_iGetPrikeyByDhfKey(ucKeylist[2]);  
     pucBTN_CONFIG[22] = 0x00;
     pucBTN_CONFIG[23] = 0x00; 

     pucBTN_CONFIG[24] = 0x7F;
     pucBTN_CONFIG[25] = 0x06;
     pucBTN_CONFIG[26] = 0x00;

     pucBTN_CONFIG[27] = 0x7F;
     pucBTN_CONFIG[28] = 0x06;
     pucBTN_CONFIG[29] = 0x00;
       
     pucBTN_CONFIG[30] = 0x7F;
     pucBTN_CONFIG[31] = 0x06;
     pucBTN_CONFIG[32] = 0x00;  

     pucBTN_CONFIG[33] = 0x7F;
     pucBTN_CONFIG[34] = 0x06;
     pucBTN_CONFIG[35] = 0x00; 

     pucBTN_CONFIG[36] = 0x00;
     pucBTN_CONFIG[37] = 0x00;
     pucBTN_CONFIG[38] = 0x00;  
     pucBTN_CONFIG[39] = 0x00;
     pucBTN_CONFIG[40] = 0x00;
     pucBTN_CONFIG[41] = 0x00; 
     pucBTN_CONFIG[42] = DhfCommon_iGetPrikeyByDhfKey(ucKeylist[4]); 
     pucBTN_CONFIG[43] = DhfCommon_iGetPrikeyByDhfKey(ucKeylist[5]);
     pucBTN_CONFIG[44] = DhfCommon_iGetPrikeyByDhfKey(ucKeylist[7]);
     pucBTN_CONFIG[45] = DhfCommon_iGetPrikeyByDhfKey(ucKeylist[6]);
     pucBTN_CONFIG[46] = 0x00;
     pucBTN_CONFIG[47] = 0x00; 
     return 1;
}

static UINT8 DhfCommon_iGetDhfEqTypeByPriEqType(UINT8 ucPriEqType)
{
   UINT8 i = 0;
   for(i = 0;i < 5;i++)
   {
      if(s_sApEqTypePairlist[i][1] == ucPriEqType)
      {
        return s_sApEqTypePairlist[i][0];
      }
    
   }

}

static UINT8 DhfCommon_iGetPriEqTypeByDhfEqType(UINT8 ucDhfEqType)
{
   UINT8 i = 0;
   for(i = 0;i < APPKEYMAX;i++)
   {
      if(s_sApEqTypePairlist[i][0] == ucDhfEqType)
      {
        return s_sApEqTypePairlist[i][1];
      }
    
   }

}
extern const EQ_CFG_SEG *eq_type_tab[EQ_MODE_MAX];
int  DhfCommon_iGetCrntEQ_Len(void)
{
    //EQ_MODE eEqmode = 0;
    //eEqmode = eq_mode_get_cur();
   // EQ_CFG_SEG ** peEqcfglist = NULL;
    //EQ_CFG_SEG *eEqcfg = NULL;
    //peEqcfglist = (EQ_CFG_SEG **)get_eq_mode_tab();
   // eEqcfg = eq_type_tab[eEqmode];
    int nEqlen = 5;//sizeof(eEqcfg)/sizeof(eEqcfg[0]);
   // DHF_OS_Debug("1111---eqmode = %d,nEqlen = %d\n",eEqmode,nEqlen+1);
    return (8*nEqlen)+1;
}
static BOOL DhfCommon_iGetEQ_INFO(UINT8 *pucEQ_INFO)
{
    EQ_MODE eEqmode = 0;
    u8 ucEqmode = 0;
    UINT8 ucEq_setting[11]={0};
    eEqmode = eq_mode_get_cur();
    ucEqmode = get_eq_mode();
    DHF_OS_Debug("DhfCommon_iGetEQ_INFO-------samson-----eEqmode = %d,ucEqmode = %d\n",eEqmode,ucEqmode);
    //u8 eq_pre_fetch_info[1  +  20  + (1 + 10) * 10] = {0}; // num + freq + all_gain_of_eq [max]
    //u8 eq_per_fetch_size = app_get_eq_all_info(eq_pre_fetch_info);
   // DHF_printf_buf(eq_pre_fetch_info,eq_per_fetch_size);
    if(eEqmode!=ucEqmode)
    {
          eEqmode = ucEqmode;
          eq_mode_set(eEqmode);
          get_eq_setting(ucEq_setting);
          ucEq_setting[0] = eEqmode;
          deal_eq_setting(ucEq_setting, 1, 1);
    }
    //EQ_CFG_SEG ** peEqcfglist = NULL;
    EQ_CFG_SEG *eEqcfg = NULL;
   // peEqcfglist = (EQ_CFG_SEG **)get_eq_mode_tab();
    eEqcfg = eq_type_tab[eEqmode];
    int nEqlen = 5;//sizeof(eEqcfg)/sizeof(eEqcfg[0]);
     DHF_OS_Debug("222---eqmode = %d,nEqlen = %d\n",eEqmode,nEqlen);
    int i = 0;
    if(eEqmode == EQ_MODE_NORMAL)
    {
      pucEQ_INFO[0] = 0x00;
    }
    else if(eEqmode == EQ_MODE_CLASSIC)
    {
      pucEQ_INFO[0] = 40;
    }
    else if(eEqmode == EQ_MODE_COUNTRY)
    {
       pucEQ_INFO[0] = 30;
    }
     else if(eEqmode == EQ_MODE_JAZZ)
    {
          pucEQ_INFO[0] = 60;
    }
     else if(eEqmode == EQ_MODE_POP)
    {
         pucEQ_INFO[0] = 50;
    }
     else if(eEqmode == EQ_MODE_ROCK)
    {
         pucEQ_INFO[0] = 20;
    }
    for(i = 0; i< nEqlen; i++)
    {
      pucEQ_INFO[1+i*8] = (UCHAR)(eEqcfg[i].freq&0xFF);
      pucEQ_INFO[2+i*8] = (UCHAR)((eEqcfg[i].freq&0xFF00)>>8);
      if((UCHAR)eEqcfg[i].gain==0)
      {
          pucEQ_INFO[3+i*8] = 0x00;
          pucEQ_INFO[4+i*8] = 0x00;
          pucEQ_INFO[5+i*8] = 0x00;
      }
      else
      {
          pucEQ_INFO[3+i*8] = 0xFF;
          pucEQ_INFO[4+i*8] = 0xFF;
          pucEQ_INFO[5+i*8] = 0xFF;
      }

      pucEQ_INFO[6+i*8] = (UCHAR)(eEqcfg[i].gain);
      pucEQ_INFO[7+i*8] = (UCHAR)(eEqcfg[i].q*20);
      pucEQ_INFO[8+i*8] = (UCHAR)(DhfCommon_iGetPriEqTypeByDhfEqType((uint8_t)eEqcfg[i].iir_type));
    }
     //待实现
    return 1;
}
static BOOL DhfCommon_iSetEQ_INFO(UINT8 *pucEQ_INFO)
{
     //待实现
      /*
     03 01 EE 03 02 02 11 03 03 88 FE 03 04 00 00 02 
     05 71 
     72 06 
     00 //eq 编号
     //EQ 分组
     14 00 00 00 40 C1 10 00 
     17 00 00 00 40 C1 10 00 
     15 00 00 00 40 C1 10 00 
     16 00 00 00 00 C1 10 01 
     DA 00 33 33 73 C0 18 01 
     44 02 33 33 73 C0 0C 01 
     3C 00 CD CC CC 3F 14 01 
     E2 04 00 00 00 C1 10 02 
     72 06 00 00 00 C0 20 01 
     68 10 00 00 60 C0 28 01 
     9C 18 33 33 73 40 14 01 
     10 27 00 00 00 00 0F 01 
     E0 2E 00 00 00 41 10 02 
     80 3E 00 00 00 00 0F 01 
     //eq end
     02 07 F3 
     */
    EQ_MODE eEqmode = 0;
    UINT8 ucEq_setting[11]={0};
    if(pucEQ_INFO[0] == 0x00)
    {
      eEqmode = EQ_MODE_NORMAL;
    }
    else if(pucEQ_INFO[0] == 40)
    {
      eEqmode = EQ_MODE_CLASSIC;
    }
    else if(pucEQ_INFO[0] == 20)
    {
       eEqmode = EQ_MODE_ROCK;
    }
     else if( pucEQ_INFO[0] == 60)
    {
         eEqmode = EQ_MODE_JAZZ;
    }
     else if(pucEQ_INFO[0] == 50)
    {
         eEqmode = EQ_MODE_POP;
    }
     else if(pucEQ_INFO[0] == 30)
    {
         eEqmode = EQ_MODE_COUNTRY;
    }
    eq_mode_set(eEqmode);
    get_eq_setting(ucEq_setting);
    ucEq_setting[0] = eEqmode;
    deal_eq_setting(ucEq_setting, 1, 1);
    return 1;
}
extern void audio_dac_ch_mute(struct audio_dac_hdl * dac, u8 ch, u8 mute);
 //0:L 1: R
static BOOL DhfCommon_iSetFind_Device(UINT8 *pucFindDev)
{
     //待实现
     if((pucFindDev[0]==1)&&(pucFindDev[1]==1))
     {
          fine_ear_interval(SEEK_ALL);
     }
     else
     {
       if(pucFindDev[0]==1)
       {
            fine_ear_interval(SEEK_LEFT);
       }
       else if(pucFindDev[1]==1)
       {
            fine_ear_interval(SEEK_RIGHT);
       }
       else
       {
         fine_ear_interval(CANCLE_SEEK);
       }
     }
     return 1;
}

static BOOL  DhfCommon_iSetResetDevice(UINT8 *pucResetDev)
{
     //待实现
     UINT8 eEqmode = 0;
     UINT8 ucEq_setting[11]={0};
     eq_mode_set(eEqmode);
     get_eq_setting(ucEq_setting);
     ucEq_setting[0] = eEqmode;
     deal_eq_setting(ucEq_setting, 1, 1);
     DhfCommon_iReSetANCMode();
     DhfCommon_iReSetDefaultBTN_CONFIG();
     DHFSleep(5);
     extern void factory_reset(void);
     factory_reset();
     return 1;
}

BOOL DHFCommon_Geteardata(DHFEARDATA_E eEardatatype,void* Outdata)
{
    UINT8 *puOutbuf = NULL;
    BOOL bRet = 0;
   if (Outdata==NULL)
   {
    return FALSE;
   }
   puOutbuf = (UINT8 *)Outdata;
   switch (eEardatatype)
   {
    case EM_EAR_CONN_STATUS:
    bRet =Dhfcommon_iGetEarConnstatus(&puOutbuf[0],&puOutbuf[1]);
    break;
    case EM_EAR_BATTERY:
    bRet =Dhfcommon_iGetEarBattery(puOutbuf);
    break;
    case EM_EAR_ANC_MODE:
    bRet =DhfCommon_iGetANCMode(puOutbuf);
    break;
    case EM_EAR_FW_VERSION:
    bRet =DhfCommon_iGetFwVersion(puOutbuf);
    break;
    case EM_EAR_BTN_CONFIG:
   bRet = DhfCommon_iGetBTN_CONFIG(puOutbuf);
    break;
    case EM_EAR_DEFAULT_BTN_CONFIG:
   bRet = DhfCommon_iSetDefaultBTN_CONFIG(puOutbuf);
    break;
    case EM_EAR_EQ_INFO:
    bRet =DhfCommon_iGetEQ_INFO(puOutbuf);
    break;
    case EM_EAR_GAME_MODE:
    bRet =DhfCommon_iGetGameMode(puOutbuf);
    break;
    case EM_EAR_BASS_MODE:
    bRet =DhfCommon_iGetBASS_MODE(puOutbuf);
    break;
    case EM_EAR_BATCH_NUMBER:
    bRet =DhfCommon_iGetBACTH_MODE(puOutbuf);
    break;  
    case EM_EAR_SN_NUMBER:
    bRet = DhfCommon_iGetSn_Number(puOutbuf);
    break;
    case EM_EAR_MULTI_CONN_MODE:
     bRet =DhfCommon_iGetMULTI_CONN_MODE(puOutbuf);
    break;
   default:
    break;
   }
   return bRet;
}

BOOL DHFCommon_Seteardata(DHFEARDATA_E eEardatatype,void* Indata)
{
    UINT8 *puOutbuf = NULL;
    BOOL bRet = 0;
   if ((Indata==NULL)&&(eEardatatype != EM_EAR_RESET_DEVICE))
   {
    return FALSE;
   }
   puOutbuf = (UINT8 *)Indata;
   switch (eEardatatype)
   {
    case EM_EAR_ANC_MODE:
     bRet =DhfCommon_iSetANCMode(puOutbuf);
    break;
    case EM_EAR_BTN_CONFIG:
     bRet =DhfCommon_iSetBTN_CONFIG(puOutbuf);
    break;
    case EM_EAR_EQ_INFO:
     bRet =DhfCommon_iSetEQ_INFO(puOutbuf);
    break;
    case EM_EAR_GAME_MODE:
     bRet =DhfCommon_iSetGameMode(puOutbuf);
    break;
    case EM_EAR_BASS_MODE:
     bRet =DhfCommon_iSetBASS_MODE(puOutbuf);
    break;
    case EM_EAR_FIND_DEVICE:
     bRet =DhfCommon_iSetFind_Device(puOutbuf);
    break;
    case EM_EAR_RESET_DEVICE:
     bRet =DhfCommon_iSetResetDevice(puOutbuf);
    break;
    case EM_EAR_MULTI_CONN_MODE:
     bRet = DhfCommon_iSetMULTI_CONN_MODE(puOutbuf);
    break;
   default:
    break;
   }
   return bRet;
}

void DHFCommon_SetRCSPType(UINT8 ucRcsptype)
{
  eDHFProtocol = ucRcsptype;
}

BOOL DHFCommon_Datasend(void *priv, UCHAR *buf, UINT16 len)
{
     if(eDHFProtocol==EM_RCSP_BLE)
     {
          return rcsp_data_send(priv,buf,len);
     }
     else
     {
          return user_spp_send_data(buf,len);
     }

     return 0;
}
