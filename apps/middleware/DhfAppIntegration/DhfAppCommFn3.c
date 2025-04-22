/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-03-21 18:36:24
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-04-15 15:58:31
 * @FilePath: \AC700N_earphone_release_fn3app\apps\middleware\DhfAppIntegration\DhfAppCommFn3.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/************************************************************************
Copyright (c) 2025,ShenZhen DHF Electronic Ltd Co.
All rights reserved.

File Name: DhfAppCommFn3.c
Summary: Control the BLE protocol FN3
Current Version: 1.1.0
Author: samson

Ver 1.0.0, dhf, 2025.03.21
Original version

**************************************************************************/
#include "..\basic\dhf_types.h"
#include "..\basic\dhf_os.h"
#include "..\basic\dhfcommon_interface.h"
#include "DhfAppCommFn3.h"
/*
03 01 EA 03 02 02 11 03 03 2A 53 03 04 00 00 02 
05 00 02 07 00 

03 01 F4 03 02 02 11 03 03 46 DF 03 04 00 00 02 
05 01 02 06 01 02 07 01 
*/

typedef struct 
{
   UINT16 uCommond_id;
   UINT16 uMessge_id;
   UINT16 uRepos_code;
   UCHAR *pucPlayload_buf;
   UINT8  ucPlayLoad_len;
   UINT8  ucCheck;
}DHFAppComm_Field_S;

#define COMMFIELD_CN 5
#define COMMAND_LEN  188
static DHFAppComm_Field_S s_saCommField[COMMFIELD_CN]={0};
static int s_nCommField_cn = 0;
static UCHAR ucCommandBuf[COMMAND_LEN]={0};
static UINT8 DhfAppComm_iCheckSum(UCHAR *ucSrcBuf,int uSrcBuflen)
{
    INT32 i = 0;
    UINT32 unChecksum = 0;
    UINT32 unChecksum_tem = 0;
    for(i = 0; i < uSrcBuflen; i++)
    {
       unChecksum_tem +=(UINT32)ucSrcBuf[i];
    }
    unChecksum = (UINT8)(unChecksum_tem & 0xff);
    
    return unChecksum;
}

static BOOL DhfAppComm_iDatasend(DHFAppComm_Field_S *psField)
{
    UINT8 ucOffset = 0;
    DHF_MEMSET(ucCommandBuf,0,COMMAND_LEN);
    //comm id
    ucCommandBuf[0] = 0x03;
    ucCommandBuf[1] = 0x01;
    ucCommandBuf[2] = (UCHAR)((psField->uCommond_id)&0xFF);
    ucCommandBuf[3] = (UCHAR)(((psField->uCommond_id)&0xFF00)>>8);
    //dir 0x22/0x21
    ucCommandBuf[4] = 0x02;
    ucCommandBuf[5] = 0x02;
    if(psField->uCommond_id > 2000)
    {
        ucCommandBuf[6] = 0x21;
    }
    else
    {
       ucCommandBuf[6] = 0x22;
    }
    //msg id
    ucCommandBuf[7] = 0x03;
    ucCommandBuf[8] = 0x03;
    ucCommandBuf[9] = (UCHAR)((psField->uMessge_id)&0xFF);
    ucCommandBuf[10] = (UCHAR)(((psField->uMessge_id)&0xFF00)>>8);
    //respond code
    ucCommandBuf[11] = 0x03;
    ucCommandBuf[12] = 0x04;
    ucCommandBuf[13] = (UCHAR)((psField->uRepos_code)&0xFF);;
    ucCommandBuf[14] = (UCHAR)(((psField->uRepos_code)&0xFF00)>>8);
    //playload len
    ucCommandBuf[15] = 0x02;
    ucCommandBuf[16] = 0x05;
    ucCommandBuf[17] = psField->ucPlayLoad_len;
    if(psField->ucPlayLoad_len==0)
    {
        //check sum
        ucOffset = 18;
        ucCommandBuf[ucOffset] = 0x02;
        ucOffset++;
        ucCommandBuf[ucOffset] = 0x07;
        ucOffset++;
        ucCommandBuf[ucOffset] = 0x00;
        ucOffset++;
    }
    else
    {
        //playload buf
        ucCommandBuf[18] = psField->ucPlayLoad_len+1;
        ucCommandBuf[19] = 0x06;
        DHF_MEMCPY(&ucCommandBuf[20],psField->pucPlayload_buf,psField->ucPlayLoad_len);
        //check sum
        ucOffset = 20+psField->ucPlayLoad_len;
        ucCommandBuf[ucOffset] = 0x02;
        ucOffset++;
        ucCommandBuf[ucOffset] = 0x07;
        ucOffset++;
        ucCommandBuf[ucOffset] = psField->ucCheck;
        ucOffset++;
    }
     DHF_OS_Debug("[DhfAppComm_iDatasend]----data:%d-!\n",ucOffset);
     DHF_printf_buf(ucCommandBuf,ucOffset);
     return DHFCommon_Datasend(NULL,ucCommandBuf,ucOffset);
}

static BOOL DhfAppComm_iProcessGetType(DHFAppComm_Field_S *psField)
 {
    
    DHFAppComm_Field_S *psFieldex = psField;
    BOOL bRet = 0;
    if(psFieldex->pucPlayload_buf)
    {
        DHF_FREE(psFieldex->pucPlayload_buf);
        psFieldex->pucPlayload_buf = NULL;
    }
    psFieldex->uRepos_code = 0x0000;
    switch (psFieldex->uCommond_id)
    {
        case FN3_COMM_CONN_STATUS_APP_GET:
        case FN3_COMM_COMM_CONN_STATUS_CHANGE_DEVICE_SEND:
        {
            psFieldex->ucPlayLoad_len = 2;
            psFieldex->pucPlayload_buf = (UCHAR *)DHF_MALLOC(psFieldex->ucPlayLoad_len);
            if ( psFieldex->pucPlayload_buf)
            {
                bRet = DHFCommon_Geteardata(EM_EAR_CONN_STATUS,psFieldex->pucPlayload_buf);
                psFieldex->ucCheck = DhfAppComm_iCheckSum(psFieldex->pucPlayload_buf,psFieldex->ucPlayLoad_len);
            }

        }
        break;
        case FN3_COMM_BASS_MODE_APP_GET:
        case FN3_COMM_COMM_BASS_MODE_CHANGE_DEVICE_SEND:
         {
            psFieldex->ucPlayLoad_len = 1;
            psFieldex->pucPlayload_buf = (UCHAR *)DHF_MALLOC(psFieldex->ucPlayLoad_len);
            if ( psFieldex->pucPlayload_buf)
            {
                bRet = DHFCommon_Geteardata(EM_EAR_BASS_MODE,psFieldex->pucPlayload_buf);
                psFieldex->ucCheck = DhfAppComm_iCheckSum(psFieldex->pucPlayload_buf,psFieldex->ucPlayLoad_len);
            }

        }  
        break;
        case FN3_COMM_BATCH_NUMBER_APP_GET:
        {
            psFieldex->ucPlayLoad_len = 1;
            psFieldex->pucPlayload_buf = (UCHAR *)DHF_MALLOC(psFieldex->ucPlayLoad_len);
            if ( psFieldex->pucPlayload_buf)
            {
                bRet = DHFCommon_Geteardata(EM_EAR_BATCH_NUMBER,psFieldex->pucPlayload_buf);
                psFieldex->ucCheck = DhfAppComm_iCheckSum(psFieldex->pucPlayload_buf,psFieldex->ucPlayLoad_len);
            }

        }  
        break;
        case FN3_COMM_COMM_BATTERY_CHANGE_DEVICE_SEND:
        case FN3_COMM_BATTERY_APP_GET:
        {
            psFieldex->ucPlayLoad_len = 3;
            psFieldex->pucPlayload_buf = DHF_MALLOC(psFieldex->ucPlayLoad_len);
            if ( psFieldex->pucPlayload_buf)
            {
                bRet = DHFCommon_Geteardata(EM_EAR_BATTERY,psFieldex->pucPlayload_buf);
                psFieldex->ucCheck = DhfAppComm_iCheckSum(psFieldex->pucPlayload_buf,psFieldex->ucPlayLoad_len);
            }
           
        }
        break;
        case FN3_COMM_DEFAULT_BTN_CONFIG_APP_SET:
        {
            psFieldex->ucPlayLoad_len = 0x30;
            psFieldex->pucPlayload_buf = (UCHAR *)DHF_MALLOC(psFieldex->ucPlayLoad_len);
            if ( psFieldex->pucPlayload_buf)
            {
                bRet = DHFCommon_Geteardata(EM_EAR_DEFAULT_BTN_CONFIG,psFieldex->pucPlayload_buf);
                psFieldex->ucCheck = DhfAppComm_iCheckSum(psFieldex->pucPlayload_buf,psFieldex->ucPlayLoad_len);
            }

        }
        break;
        case FN3_COMM_BTN_CONFIG_APP_GET:
         {
            psFieldex->ucPlayLoad_len = 0x30;
            psFieldex->pucPlayload_buf = (UCHAR *)DHF_MALLOC(psFieldex->ucPlayLoad_len);
            if ( psFieldex->pucPlayload_buf)
            {
                bRet = DHFCommon_Geteardata(EM_EAR_BTN_CONFIG,psFieldex->pucPlayload_buf);
                psFieldex->ucCheck = DhfAppComm_iCheckSum(psFieldex->pucPlayload_buf,psFieldex->ucPlayLoad_len);
            }

        }  
        break;
        case FN3_COMM_COMM_MULTI_CONN_MODE_APP_GET:
        {
            psFieldex->ucPlayLoad_len = 1;
            psFieldex->pucPlayload_buf = (UCHAR *)DHF_MALLOC(psFieldex->ucPlayLoad_len);
            if ( psFieldex->pucPlayload_buf)
            {
                bRet = DHFCommon_Geteardata(EM_EAR_MULTI_CONN_MODE,psFieldex->pucPlayload_buf);
                psFieldex->ucCheck = DhfAppComm_iCheckSum(psFieldex->pucPlayload_buf,psFieldex->ucPlayLoad_len);
            }

        }  
        break;
        case FN3_COMM_EQ_INFO_APP_GET:
        {
            psFieldex->ucPlayLoad_len = DhfCommon_iGetCrntEQ_Len();
            psFieldex->pucPlayload_buf = (UCHAR *)DHF_MALLOC(psFieldex->ucPlayLoad_len);
            if ( psFieldex->pucPlayload_buf)
            {
                bRet = DHFCommon_Geteardata(EM_EAR_EQ_INFO,psFieldex->pucPlayload_buf);
                psFieldex->ucCheck = DhfAppComm_iCheckSum(psFieldex->pucPlayload_buf,psFieldex->ucPlayLoad_len);
            }
        } 
        break;
        case FN3_COMM_FW_VERSION_APP_GET:
        {
            psFieldex->ucPlayLoad_len = 3;
            psFieldex->pucPlayload_buf = (UCHAR *)DHF_MALLOC(psFieldex->ucPlayLoad_len);
            if ( psFieldex->pucPlayload_buf)
            {
                bRet = DHFCommon_Geteardata(EM_EAR_FW_VERSION,psFieldex->pucPlayload_buf);
                psFieldex->ucCheck = DhfAppComm_iCheckSum(psFieldex->pucPlayload_buf,psFieldex->ucPlayLoad_len);
            }

        } 
        break;
        case FN3_COMM_GAME_MODE_APP_GET:
        case FN3_COMM_COMM_GAME_MODE_CHANGE_DEVICE_SEND:
        {
            psFieldex->ucPlayLoad_len = 1;
            psFieldex->pucPlayload_buf = (UCHAR *)DHF_MALLOC(psFieldex->ucPlayLoad_len);
            if ( psFieldex->pucPlayload_buf)
            {
                bRet = DHFCommon_Geteardata(EM_EAR_GAME_MODE,psFieldex->pucPlayload_buf);
                psFieldex->ucCheck = DhfAppComm_iCheckSum(psFieldex->pucPlayload_buf,psFieldex->ucPlayLoad_len);
            }

        }      
        break;
        case FN3_COMM_SN_NUMBER_APP_GET:
        {
            psFieldex->ucPlayLoad_len = 0x10;
            psFieldex->pucPlayload_buf = (UCHAR *)DHF_MALLOC(psFieldex->ucPlayLoad_len);
            if ( psFieldex->pucPlayload_buf)
            {
                bRet = DHFCommon_Geteardata(EM_EAR_SN_NUMBER,psFieldex->pucPlayload_buf);
                psFieldex->ucCheck = DhfAppComm_iCheckSum(psFieldex->pucPlayload_buf,psFieldex->ucPlayLoad_len);
            }

        }         
        break;
        case FN3_COMM_COMM_ANC_CHANGE_DEVICE_SEND:
        case FN3_COMM_ANC_MODE_APP_GET:
        {
            psFieldex->ucPlayLoad_len = 0x3;
            psFieldex->pucPlayload_buf = (UCHAR *)DHF_MALLOC(psFieldex->ucPlayLoad_len);
            if ( psFieldex->pucPlayload_buf)
            {
                bRet = DHFCommon_Geteardata(EM_EAR_ANC_MODE,psFieldex->pucPlayload_buf);
                psFieldex->ucCheck = DhfAppComm_iCheckSum(psFieldex->pucPlayload_buf,psFieldex->ucPlayLoad_len);
            }

        }         
        break;
        default:
            break;
    }
      if ((!psFieldex->pucPlayload_buf )|| (!bRet))
        {
            if(psFieldex->pucPlayload_buf)
            {
                DHF_FREE(psFieldex->pucPlayload_buf);
                psFieldex->pucPlayload_buf = NULL;
            }
            psFieldex->uRepos_code = 10001;
                psFieldex->ucPlayLoad_len = 0;

                if(psFieldex->uCommond_id > 2000)//notify就返回
                {
                return FALSE;
                }

        }
      DhfAppComm_iDatasend(psFieldex);
 }

BOOL DhfAppComm_DataNotify(DHFAPPCOMMANID_E e_CmdType)
{
   
     switch (e_CmdType)
     {
        case  EM_COMM_CONN_STATUS_CHANGE_DEVICE_SEND:
            s_saCommField[0].uCommond_id = FN3_COMM_COMM_CONN_STATUS_CHANGE_DEVICE_SEND;
            s_saCommField[0].uMessge_id = FN3_COMM_COMM_CONN_STATUS_CHANGE_DEVICE_SEND;
            break;
        case  EM_COMM_ANC_CHANGE_DEVICE_SEND:
            s_saCommField[0].uCommond_id = FN3_COMM_COMM_ANC_CHANGE_DEVICE_SEND;
            s_saCommField[0].uMessge_id = FN3_COMM_COMM_ANC_CHANGE_DEVICE_SEND;
        break;
        case  EM_COMM_BASS_MODE_CHANGE_DEVICE_SEND:
            s_saCommField[0].uCommond_id = FN3_COMM_COMM_BASS_MODE_CHANGE_DEVICE_SEND;
            s_saCommField[0].uMessge_id = FN3_COMM_COMM_BASS_MODE_CHANGE_DEVICE_SEND;
        break;
        case  EM_COMM_BATTERY_CHANGE_DEVICE_SEND:
            s_saCommField[0].uCommond_id = FN3_COMM_COMM_BATTERY_CHANGE_DEVICE_SEND;
            s_saCommField[0].uMessge_id =  FN3_COMM_COMM_BATTERY_CHANGE_DEVICE_SEND;
        break;
        case  EM_COMM_GAME_MODE_CHANGE_DEVICE_SEND:
            s_saCommField[0].uCommond_id = FN3_COMM_COMM_GAME_MODE_CHANGE_DEVICE_SEND;
            s_saCommField[0].uMessge_id =  FN3_COMM_COMM_GAME_MODE_CHANGE_DEVICE_SEND;
        break;
        default:
            break;
     }
     s_nCommField_cn = 1;
     DhfAppComm_iProcessGetType(&s_saCommField[0]);
}
static BOOL DhfAppComm_iProcessSetType(DHFAppComm_Field_S *psField)
 {
    DHFAppComm_Field_S *psFieldex = psField;
    BOOL bRet = 0;
    switch (psField->uCommond_id)
    {
        case FN3_COMM_COMM_MULTI_CONN_MODE_APP_SET:
             if(psField->pucPlayload_buf)
             {
                bRet = DHFCommon_Seteardata(EM_EAR_MULTI_CONN_MODE,psFieldex->pucPlayload_buf);
             }
            break;
        case FN3_COMM_BASS_MODE_APP_SET:
            if(psField->pucPlayload_buf)
             {
                bRet = DHFCommon_Seteardata(EM_EAR_BASS_MODE,psFieldex->pucPlayload_buf);
             }
            break;
        case FN3_COMM_RESET_DEVICE_APP_SET:
             //if(psField->pucPlayload_buf)
             {
                bRet = DHFCommon_Seteardata(EM_EAR_RESET_DEVICE,psFieldex->pucPlayload_buf);
             }
        break;
        case FN3_COMM_BTN_CONFIG_APP_SET:
          if(psField->pucPlayload_buf)
        {
            bRet = DHFCommon_Seteardata(EM_EAR_BTN_CONFIG,psFieldex->pucPlayload_buf);
        }
        break;     

        case FN3_COMM_EQ_INFO_APP_SET:
          if(psField->pucPlayload_buf)
        {
        bRet = DHFCommon_Seteardata(EM_EAR_EQ_INFO,psFieldex->pucPlayload_buf);
        }     

        break;
        case FN3_COMM_ANC_MODE_APP_SET:
          if(psField->pucPlayload_buf)
        {
        bRet = DHFCommon_Seteardata(EM_EAR_ANC_MODE,psFieldex->pucPlayload_buf);
        }
        break;

        case FN3_COMM_FIND_DEVICE_APP_SET:
          if(psField->pucPlayload_buf)
        {
            bRet = DHFCommon_Seteardata(EM_EAR_FIND_DEVICE,psFieldex->pucPlayload_buf);
        }
        break;
        case FN3_COMM_GAME_MODE_APP_SET:
        if(psField->pucPlayload_buf)
        {
            bRet = DHFCommon_Seteardata(EM_EAR_GAME_MODE,psFieldex->pucPlayload_buf);
        }
        break;
        default:
            break;
    }

            if ( !bRet)
            {
                if(psFieldex->pucPlayload_buf)
                {
                    DHF_FREE(psFieldex->pucPlayload_buf);
                    psFieldex->pucPlayload_buf = NULL;
                }
                psFieldex->uRepos_code = 10001;
                psFieldex->ucPlayLoad_len = 0;
            }
            else
            {
                psFieldex->uRepos_code = 0x0000;
                psFieldex->ucPlayLoad_len = 0;

            }
        DhfAppComm_iDatasend(psFieldex);
 }

BOOL DhfAppComm_PaserData(UCHAR *ucSrcBuf,int uSrcBuflen)
{
     UCHAR * pucBuftem = ucSrcBuf;
     UCHAR * pucPlayLoadbuf = NULL;
     int   nLentem = uSrcBuflen;
     int   nFieldlen = 0;
     FIELDTYPE_E uCommonidType = 0;
     UINT8 uCheck = 0;
     s_nCommField_cn = 0;
     DHF_OS_Debug("[DhfAppComm_PaserData]----data:%d-!\n",nLentem);
     DHF_printf_buf(pucBuftem,nLentem);
     while (nLentem > 0)
     {
          nFieldlen = pucBuftem[0];
          uCommonidType = pucBuftem[1];
          if(uCommonidType==EM_CMD_ID)
          {
             s_saCommField[s_nCommField_cn].uCommond_id = (pucBuftem[3]<<8)|pucBuftem[2];
              DHF_OS_Debug("uCommond_id 0x%x\n",s_saCommField[s_nCommField_cn].uCommond_id);
          }
          else if(uCommonidType==EM_CMD_MSG_ID)
          {
            s_saCommField[s_nCommField_cn].uMessge_id = (pucBuftem[3]<<8)|pucBuftem[2];
            DHF_OS_Debug("uMessge_id 0x%x\n",s_saCommField[s_nCommField_cn].uMessge_id);
          }
          else if(uCommonidType==EM_CMD_PAYLOAD_LEN)
          {
            s_saCommField[s_nCommField_cn].ucPlayLoad_len = pucBuftem[2];
            DHF_OS_Debug("ucPlayLoad_len %d\n",pucBuftem[2]);
          }
          else if(uCommonidType==EM_CMD_PAYLOAD_BUF)
          {
            pucPlayLoadbuf = (UCHAR*)DHF_MALLOC(nFieldlen-1);
            if(pucPlayLoadbuf!=NULL)
            {
                DHF_MEMCPY(pucPlayLoadbuf,pucBuftem+2,nFieldlen-1);
                s_saCommField[s_nCommField_cn].pucPlayload_buf = pucPlayLoadbuf;
                uCheck = DhfAppComm_iCheckSum(pucPlayLoadbuf,nFieldlen-1);
                DHF_OS_Debug("uCheck %d,playload:\n",uCheck);
                DHF_printf_buf(pucPlayLoadbuf,nFieldlen-1);
            }
          }
          else if(uCommonidType==EM_CMD_CHECK)
          {
              if(s_saCommField[s_nCommField_cn].ucPlayLoad_len==0)//if playload is null reset ucheck,else ucheck is last field's;
              {
                 uCheck = 0;
              }

              if(uCheck != pucBuftem[2])
              {
                  DHF_FREE(pucPlayLoadbuf);
                  pucPlayLoadbuf = NULL;
                  s_saCommField[s_nCommField_cn].ucPlayLoad_len = 0;
                  s_saCommField[s_nCommField_cn].pucPlayload_buf = NULL;
              }
              
              s_nCommField_cn ++;// check field is last's ,it is end flag  
              DHF_OS_Debug("s_nCommField_cn %d,paser data sucess!\n",s_nCommField_cn);
          }
          else if(uCommonidType==EM_CMD_DIR)
          {
               DHF_OS_Debug("[DhfAppComm_PaserData]----dir!\n");
          }
          else if(uCommonidType==EM_CMD_RESPONSE_CODE)
          {
               DHF_OS_Debug("[DhfAppComm_PaserData]----resp!\n");
          }
          else
          {
            DHF_OS_Debug("paser data failure!\n");
             return FALSE;
          }
         pucBuftem += nFieldlen+1;
         nLentem -= nFieldlen+1;
     }
     
     return TRUE;
}
 BOOL DhfAppComm_ProcessData(void)
 {
     int  i = 0;
     for(i = 0; i<  s_nCommField_cn; i++)
     {
         if((s_saCommField[i].uCommond_id==FN3_COMM_CONN_STATUS_APP_GET)
         || (s_saCommField[i].uCommond_id==FN3_COMM_BASS_MODE_APP_GET)
         ||(s_saCommField[i].uCommond_id==FN3_COMM_BATCH_NUMBER_APP_GET)
         ||(s_saCommField[i].uCommond_id==FN3_COMM_BATTERY_APP_GET)
         ||(s_saCommField[i].uCommond_id==FN3_COMM_BTN_CONFIG_APP_GET)
         ||(s_saCommField[i].uCommond_id==FN3_COMM_DEFAULT_BTN_CONFIG_APP_SET)
         ||(s_saCommField[i].uCommond_id==FN3_COMM_COMM_MULTI_CONN_MODE_APP_GET)
         ||(s_saCommField[i].uCommond_id==FN3_COMM_EQ_INFO_APP_GET)
         ||(s_saCommField[i].uCommond_id==FN3_COMM_FW_VERSION_APP_GET)
         ||(s_saCommField[i].uCommond_id==FN3_COMM_GAME_MODE_APP_GET)
         ||(s_saCommField[i].uCommond_id==FN3_COMM_SN_NUMBER_APP_GET)
         ||(s_saCommField[i].uCommond_id==FN3_COMM_ANC_MODE_APP_GET))
          {
             DhfAppComm_iProcessGetType(&s_saCommField[i]);
          }
          else
          {
            DhfAppComm_iProcessSetType(&s_saCommField[i]);
          }
        
     }
     
     return TRUE;
 }