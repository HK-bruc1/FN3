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
#ifndef _DHF_OS_H_
#define _DHF_OS_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "dhf_types.h"
///// 分配内存 /////
#define DHF_MALLOC  malloc
///// 释放内存 /////
#define DHF_FREE    free
#define DHF_MEMCPY  memcpy
#define DHF_MEMSET  memset
#define DHF_OS_DEBUG_
#ifdef  DHF_OS_DEBUG_
#define DHF_OS_Debug printf
#define DHF_putchar(x)                putchar(x)
#define DHF_printf_buf(x,len)         put_buf(x,len)
#else
#define DHF_OS_Debug(x, args...)
#define rcsp_putchar(...)
#define rcsp_printf_buf(...)
#endif
/* --------------------------------------------------------------------------*/
/**
 * brief 创建任务
 *
 * param threadfunc 任务回调函数
 * param Param 传递给任务回调函数的参数
 * param Priority 任务的优先级
 * param StackSize 任务的堆栈大小, 单位(u32)
 * param QueSize 任务的queue大小，单位(byte)
 * param Name 任务名 (名字长度不能超过configMAX_TASK_NAME_LEN字节)
 *
 * return 错误码
 */
/* ----------------------------------------------------------------------------*/
int DHFCreateThread (void (*threadfunc)(void* Param), void* Param,
                          unsigned int StackSize,unsigned int QueSize,
                          int Priority, const char* Name);


/* --------------------------------------------------------------------------*/
/**
 * brief 删除任务
 *
 * param pcuName 任务名
 *
 * return 错误码
 */
/* ----------------------------------------------------------------------------*/
int DhfDeleteThread (const char *pcuName);

HCSHANDLE DHFCreateMsgQueue(int MsgSize,int MaxMessages);

/* --------------------------------------------------------------------------*/
/**
 * brief 阻塞方式获取taskq
 *
 * param hMsgQueue 任务句柄
 * param pvMsg 存放queue的buf
 * param nMaxMsgBytes 最大可获取的queue长度，单位(int)
 *
 * return 错误码
 */
/* ----------------------------------------------------------------------------*/
INT32 DHFReceiveMsg(const HCSHANDLE hMsgQueue,void* pvMsg,int nMaxMsgBytes,unsigned int dwTimeout);

/* --------------------------------------------------------------------------*/
/**
 * brief 发送Q_MSG类型的taskq
 *
 * param hMsgQueue 句柄
 * param nMsgBytes msg长度
 *
 * @reutrn 错误码
 */
/* ----------------------------------------------------------------------------*/
INT32 DHFSendMsg(const HCSHANDLE hMsgQueue,  void* pvMsg, int nMsgBytes, unsigned int dwTimeout);                    

void DHFSleep(int time_tick);
#ifdef __cplusplus
}
#endif

#endif