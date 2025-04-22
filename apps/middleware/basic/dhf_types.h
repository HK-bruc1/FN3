/************************************************************************
Copyright (c) 2025,ShenZhen DHF Electronic Ltd Co.
All rights reserved.

File Name: dhf_types.h
Summary:define all data type
Current Version: 1.1.0
Author: samson

Ver 1.0.0, dhf, 2025.03.21
Original version

**************************************************************************/

#ifndef _DHF_TYPES_H_
#define _DHF_TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef char			 CHAR;
typedef CHAR			 *PCHAR;
typedef CHAR			 *LPCHAR;

typedef unsigned char    UCHAR;
typedef UCHAR			 *PUCHAR;
typedef UCHAR			 *LPUCHAR;

typedef short			 SHORT;
typedef SHORT			 *PSHORT;
typedef SHORT			 *LPSHORT;

typedef unsigned short   USHORT;
typedef USHORT			 *PUSHORT;
typedef USHORT			 *LPUSHORT;

typedef long			 LONG;
typedef LONG			 *PLONG;
typedef LONG			 *LPLONG;

typedef unsigned long	 ULONG;
typedef ULONG			 *PULONG;
typedef ULONG			 *LPULONG;

#ifndef DEFINED_BOOL
typedef unsigned char              BOOL;
#define  DEFINED_BOOL

typedef BOOL             *PBOOL;
typedef BOOL             *LPBOOL;

typedef unsigned char    BYTE;
typedef BYTE             *PBYTE;
typedef BYTE             *LPBYTE;

typedef unsigned short   WORD;
typedef WORD             *PWORD;
typedef WORD             *LPWORD;
typedef unsigned int     DWORD;
typedef DWORD            *PDWORD;
typedef DWORD            *LPDWORD;

typedef int              INT;
typedef int              *PINT;
typedef int              *LPINT;

typedef unsigned int     UINT;
typedef unsigned int     *PUINT;
typedef unsigned int     *LPUINT;

typedef float            FLOAT;
typedef FLOAT            *PFLOAT;
typedef FLOAT            *LPFLOAT;

typedef void			 VOID;
typedef void			 *PVOID;
typedef void             *LPVOID;
typedef const void       *LPCVOID;
#endif
typedef signed long		 HCSHANDLE; 

/*Standard constants*/
#undef FALSE
#undef TRUE
#undef NULL

#define FALSE   0
#define TRUE    1
#define NULL    0
typedef signed char	INT8;
typedef signed short	INT16;
typedef signed long	INT32;
typedef unsigned char	UINT8;
typedef unsigned short	UINT16;
typedef unsigned long	UINT32;

#ifndef SUCCESS
#define SUCCESS		0 
#endif

#ifndef FAILURE
#define FAILURE		-1 
#endif

typedef struct _TDATE
{
	DWORD m_dwYear_13         :13;	/**< Year*/
	DWORD m_dwMonth_8         :8;	/**< Month*/
	DWORD m_dwDay_8           :8;		/**< Day*/
	DWORD m_dwWeek_3          :3;	/**< Week*/
}TDATE,*PTDATE;

/**@brief �����ע��*/
typedef struct _TTIME
{
	DWORD m_dwHour_8         :8;		/**< Hours*/
	DWORD m_dwMinute_8       :8;		/**< Minutes*/
	DWORD m_dwSecond_8       :8;	/**< Seconds*/
	DWORD m_dwMilSecond_8    :8;	/**< ����,�˴����岻�����1s=1000ms,�ð�λ��ʾ�ǲ�����*/
}TTIME,*PTTIME;

/**@brief �����ע��*/
typedef struct _TSYSTEMTIME
{
	TDATE m_TDATE;				/**< ϵͳ����*/
	TTIME m_TTIME;				/**< ϵͳʱ��*/
}TSYSTEMTIME,*PTSYSTEMTIME;
#ifdef __cplusplus
}
#endif

#endif