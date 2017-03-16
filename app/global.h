#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include "remix_public.h"

//版本号:主版本号.子版本号.修正版本号.编译版本号
#define SOFTWARE_VER                "003.004.001.001"

#define BUFPOOLNUM                  100	//BUFPOOL数量
#define MSGBUFLEN                   100	//每个BUF的长度，单位:字节

#define TASKSTACK                   1024	//任务栈大小，单位:字节

typedef struct msgbuf		//消息缓冲结构
{
	REMIX_DLIST strList;	//缓冲链表
	U8 ucLength;		//消息长度
	U8 aucBuf[MSGBUFLEN];	//消息缓冲
} MSGBUF;

typedef struct bufpool		//消息缓冲池结构
{
	REMIX_DLIST strFreeList;	//缓冲池空闲链表
	MSGBUF astrBufPool[BUFPOOLNUM];	//缓冲池
} BUFPOOL;

/************************************************************************/
extern BUFPOOL gstrBufPool;
extern REMIX_QUEUE *gpstrSerialMsgQue;
extern REMIX_TCB *gpstrSerialTaskTcb;
//extern REMIX_SEM *gpstrSemMut;

/************************************************************************/
extern void DEV_SoftwareInit(void);
extern void DEV_HardwareInit(void);
extern void DEV_PutStrToMem(U8 * pvStringPt, ...);
extern void DEV_PrintMsg(U8 * pucBuf, U32 uiLen);
extern MSGBUF *DEV_BufferAlloc(BUFPOOL * pstrBufPool);
extern void DEV_BufferFree(BUFPOOL * pstrBufPool, REMIX_DLIST * pstrQueNode);
extern void DEV_DelayMs(U32 uiMs);
extern void TEST_TestTask1(void *pvPara);
extern void TEST_TestTask2(void *pvPara);
extern void TEST_TestTask3(void *pvPara);
extern void TEST_TestTask4(void *pvPara);
extern void TEST_SerialPrintTask(void *pvPara);
extern void TEST_TaskCreatePrint(REMIX_TCB * pstrTcb);
extern void TEST_TaskSwitchPrint(REMIX_TCB * pstrOldTcb, REMIX_TCB * pstrNewTcb);
extern void TEST_TaskDeletePrint(REMIX_TCB * pstrTcb);
#endif
