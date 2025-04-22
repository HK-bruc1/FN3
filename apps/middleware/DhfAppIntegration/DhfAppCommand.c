/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-03-21 18:22:44
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-04-11 14:35:21
 * @FilePath: \AC700N_earphone_release_fn3app\apps\middleware\DhfAppIntegration\DhfAppCommond.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/************************************************************************
Copyright (c) 2025,ShenZhen DHF Electronic Ltd Co.
All rights reserved.

File Name: DhfAppCommond.c
Summary: Control the BLE protocol FN3
Current Version: 1.1.0
Author: samson

Ver 1.0.0, dhf, 2025.03.21
Original version

**************************************************************************/
#include "..\basic\dhf_types.h"
#include "..\basic\dhf_os.h"
#include "DhfAppCommand.h"
#include "..\basic\dhfcommon_interface.h"
#include "DhfAppCommFn3.h"

#define DHFAPPTASK   "APPCOMM_TSK"
#define DHFAPPTASK_PRIORITY  1
#define DHFAPPTASK_STACK_SIZE  1024 * 5 
#define ARRAY_SIZE(array)  (sizeof(array)/sizeof(array[0]))

static void DhfAppComm_iMaintask(void *p_param)
{
    INT32 nMsg[8] = {0};
    INT32 nRet = 0;
    UCHAR * pucBuftem = NULL;
    INT32   nBuflen = 0;
    DHF_OS_Debug("----APPCOMM_TSK-start!!!!\n");
    while (1) {
        nRet = DHFReceiveMsg((const HCSHANDLE)((void*)DHFAPPTASK),nMsg,ARRAY_SIZE(nMsg),0xFFFFFFFF);
        if (nRet== 0) 
        {
           pucBuftem = (UCHAR *)nMsg[2];
           nBuflen = nMsg[1];
           if(pucBuftem!=NULL)
           {
                nRet = DhfAppComm_PaserData(pucBuftem,nBuflen);
                if(nRet==1)
                {
                   DhfAppComm_ProcessData();
                }
                DHF_FREE(pucBuftem);
                pucBuftem = NULL;
           }

        }

        DHFSleep(5);
    }
}


int DHFAppCommand_init(void)
{
   int nRet = 0;
   nRet =  DHFCreateThread(DhfAppComm_iMaintask, NULL, DHFAPPTASK_STACK_SIZE,512,DHFAPPTASK_PRIORITY, DHFAPPTASK);
   return nRet;
}

int DHFAPCommand_DataPut(unsigned char *uData,int uDatalen)
{
   int nRet = -1;
   UCHAR * pucBuff = (UCHAR *)DHF_MALLOC(uDatalen);
   if(pucBuff!=NULL)
   {
     DHF_MEMCPY(pucBuff,uData,uDatalen);
     nRet = DHFSendMsg((const HCSHANDLE)((void*)DHFAPPTASK), (void*)pucBuff, uDatalen, 0xFFFFFFFF);
   }
   return nRet;
}

int DhfAppCommand_DataNotify(DHFAPPCOMMANID_E eCommandType)
{
    return DhfAppComm_DataNotify(eCommandType);
}