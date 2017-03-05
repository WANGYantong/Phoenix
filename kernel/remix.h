#ifndef  __REMIX_H__
#define  __REMIX_H__

#ifdef __cplusplus
extern "C"{
#endif

#define REMIX_VERSION           "001.001.001.001"

//*******************************CPU related***************************//
typedef char                        U8;
typedef unsigned short              U16;
typedef unsigned int                U32;
typedef signed char                 S8;
typedef short                       S16;
typedef int                         S32;

#define MODE_USR                    0x01000000

#define ALIGN4MASK                  0xFFFFFFFC
#define ALIGN8MASK                  0xFFFFFFF8

#define ADDRVAL(addr)               (*((volatile U32*)(addr)))
//*******************************CPU related***************************//


typedef void                    (*VFUNCTION)(void*);
typedef void                    (*VFUNCTION1)(U8);

#ifndef NULL
    #define NULL                ((void*)0)
#endif

#ifndef RTN_SUCD
    #define RTN_SUCD            0u
#endif

#ifndef RTN_FAIL
    #define RTN_FAIL            1u
#endif

#define PRIORITY2048                2048
#define PRIORITY1024                1024
#define PRIORITY512                 512
#define PRIORITY256                 256
#define PRIORITY128                 128
#define PRIORITY64                  64
#define PRIORITY32                  32
#define PRIORITY16                  16
#define PRIORITY8                   8

#if PRIORITY2048 == PRIORITYNUM
 #define PRIOFLAGGRP1               256
 #define PRIOFLAGGRP2               32
 #define PRIOFLAGGPR3               4
#elif PRIORITY1024 == PRIORITYNUM
 #define PRIOFLAGGRP1               128
 #define PRIOFLAGGRP2               16
 #define PRIOFLAGGPR3               2
#elif PRIORITY512 == PRIORITYNUM
 #define PRIOFLAGGRP1               64
 #define PRIOFLAGGRP2               8
 #define PRIOFLAGGPR3               1
#elif PRIORITY256 == PRIORITYNUM
 #define PRIOFLAGGRP1               32
 #define PRIOFLAGGRP2               4
 #define PRIOFLAGGPR3               1
#elif PRIORITY128 == PRIORITYNUM
 #define PRIOFLAGGRP1               16
 #define PRIOFLAGGRP2               2
 #define PRIOFLAGGPR3               1
#elif PRIORITY64 == PRIORITYNUM
 #define PRIOFLAGGRP1               8
 #define PRIOFLAGGRP2               1
 #define PRIOFLAGGPR3               1
#elif PRIORITY32 == PRIORITYNUM
 #define PRIOFLAGGRP1               4
 #define PRIOFLAGGRP2               1
 #define PRIOFLAGGPR3               1
#elif PRIORITY16 == PRIORITYNUM
 #define PRIOFLAGGRP1               2
 #define PRIOFLAGGRP2               1
 #define PRIOFLAGGPR3               1
#elif PRIORITY8 == PRIORITYNUM
 #define PRIOFLAGGRP1               1
 #define PRIOFLAGGRP2               1
 #define PRIOFLAGGPR3               1
#else
 #error
#endif

#if PRIORITYNUM >=PRIORITY512
  #define PRIORITYBITS              U16
#else
  #define PRIORITYBITS              U8
#endif

typedef struct stackreg
{
    U32 uiR0;
    U32 uiR1;
    U32 uiR2;
    U32 uiR3;
    U32 uiR4;
    U32 uiR5;
    U32 uiR6;
    U32 uiR7;
    U32 uiR8;
    U32 uiR9;
    U32 uiR10;
    U32 uiR11;
    U32 uiR12;
    U32 uiR13;
    U32 uiR14;
    U32 uiR15;
    U32 uiXpsr;
    U32 uiExc_Rtn;
}STACKREG;


typedef struct remix_dlist
{
    struct remix_dlist* pstrHead;
    struct remix_dlist* pstrTail;
}REMIX_DLIST;

typedef struct remix_tcbque
{
    REMIX_DLIST strQueHead;
    struct remix_tcb* pstrTcb;
}REMIX_TCBQUE;

typedef struct remix_taskopt
{
    U8 ucTaskSta;
    U32 uiDelayTick;
}REMIX_TASKOPT;

typedef struct remix_prioflag
{
#if PRIORITYNUM >= PRIORITY1024
    U8 aucPrioFlagGrp1[PRIOFLAGGRP1];
    U8 aucPrioFlagGrp2[PRIOFLAGGRP2];
    U8 aucPrioFlagGrp3[PRIOFLAGGPR3];
    U8 ucPrioFlagGrp4;
#elif PRIORITYNUM >= PRIORITY128
    U8 aucPrioFlagGrp1[PRIOFLAGGRP1];
    U8 aucPrioFlagGrp2[PRIOFLAGGRP2];
    U8 ucPrioFlagGrp3;
#elif PRIORITYNUM >= PRIORITY16
    U8 aucPrioFlagGrp1[PRIOFLAGGRP1];
    U8 ucPrioFlagGrp2;
#else
    U8 ucPrioFlagGrp1;
#endif
}REMIX_PRIOFLAG;

typedef struct remix_taskschedtab
{
    REMIX_DLIST astrList[PRIORITYNUM];
    REMIX_PRIOFLAG strFlag;
}REMIX_TASKSCHEDTAB;

typedef struct remix_sem
{
    REMIX_TASKSCHEDTAB strSemTab;
    U32 uiCounter;
    U32 uiSemOpt;
    U8* pucSemMem;
    struct remix_tcb* pstrSemTask;  //ª•≥‚–≈∫≈¡ø
}REMIX_SEM;

#ifdef REMIX_CPUSTATISTIC

typedef struct remix_cpushare
{
    U32 uiSysTickVal;
    U32 uiCounter;
    U32 uiCpuShare;
}REMIX_CPUSHARE;

#endif

//”–“ª–©ø…“‘øº¬«∫œ≤¢£¨÷ÿ–¬∑‚◊∞
typedef struct remix_tcb
{
    STACKREG strStackReg;
    REMIX_TCBQUE strTaskQue;        //ALL Tasks in Remix
    REMIX_TCBQUE strTcbQue;         //register in Ready OR Delay
    REMIX_TCBQUE strSemQue;         //register in Semaphore queue
    REMIX_SEM* pstrSem;
    U8* pucTaskName;
    U8* pucTaskStack;
    U32 uiTaskFlag;                 // «∑Ò‘⁄delay±Ì÷–? «∑ÒºÃ≥–”≈œ»º∂? «∑Ò…Í«Î’ªÂ
    PRIORITYBITS ucTaskPrio;
#ifdef REMIX_TASKPRIOINHER
    U8 ucTaskPrioBackup;
#endif
    REMIX_TASKOPT strTaskOpt;           //¿Ô√ÊµƒucTaskStaº«¬º5÷–◊¥Ã¨:ready?delay?pend...
    U32 uiStillTick;
#ifdef REMIX_CPUSTATISTIC
    REMIX_CPUSHARE strCpuShare;
#endif
}REMIX_TCB;


#ifdef __cplusplus
}
#endif

#endif
