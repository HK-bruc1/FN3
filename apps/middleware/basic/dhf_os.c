/*
 * @Author: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @Date: 2025-03-27 17:09:48
 * @LastEditors: error: error: git config user.name & please set dead value or install git && error: git config user.email & please set dead value or install git & please set dead value or install git
 * @LastEditTime: 2025-03-29 12:07:12
 * @FilePath: \AC700N_earphone_release_fn3app\apps\middleware\basic\dhf_os.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
/************************************************************************
Copyright (c) 2025,ShenZhen DHF Electronic Ltd Co.
All rights reserved.

File Name: dhf_os.h
Summary:define all data type
Current Version: 1.1.0
Author: samson

Ver 1.0.0, dhf, 2025.03.21
Original version

**************************************************************************/
#include "dhf_os.h"
#include "os/os_error.h"
#include "os/os_api.h"
#include "dhf_types.h"
int DHFCreateThread(void (*threadfunc)(void* Param), void* Param,
                          unsigned int StackSize,unsigned int QueSize,
                          int Priority, const char* Name)
{
    int dReturnValue = 0;
    dReturnValue = os_task_create(threadfunc,Param,Priority,StackSize,QueSize,Name);
	if(dReturnValue!=OS_NO_ERR)
	{
		DHF_OS_Debug("[OS_TaskCreate] Task Create Failed!!\n");
	}

    return dReturnValue;
}

int  DHFDeleteThread (const char *pcuName)
{ 
    return os_task_del(pcuName);
}

HCSHANDLE DHFCreateMsgQueue(int MsgSize,int MaxMessages)
{ 
    HCSHANDLE  msg_id = 0;
	return msg_id;
}

INT32 DHFReceiveMsg(const HCSHANDLE hMsgQueue,void* pvMsg,int nMaxMsgBytes,unsigned int dwTimeout)
{
    INT32 nRet = 0;
    nRet = os_taskq_pend((const char*)hMsgQueue, (int*)pvMsg, nMaxMsgBytes);
    if (nRet!=OS_TASKQ)
    {
        return -1;
    }

    return 0;
    
}

INT32 DHFSendMsg(const HCSHANDLE hMsgQueue,  void* pvMsg, int nMsgBytes, unsigned int dwTimeout)
{
	return  os_taskq_post_msg((const char*)hMsgQueue, 2,nMsgBytes,pvMsg);
}
/* --------------------------------------------------------------------------*/
/**
 * @brief 延时。中断函数或者关闭系统总中断的情况下不能调用此函数
 *
 * @param time_tick 延时时间 1 = 10ms
 */
/* ----------------------------------------------------------------------------*/
void DHFSleep(int time_tick)
{
   os_time_dly(time_tick);   
}


