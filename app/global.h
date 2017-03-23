#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include "remix_public.h"

//�汾��:���汾��.�Ӱ汾��.�����汾��.����汾��
#define SOFTWARE_VER                "003.004.001.001"

#define BUFPOOLNUM                  100	//BUFPOOL����
#define MSGBUFLEN                   100	//ÿ��BUF�ĳ��ȣ���λ:�ֽ�

#define TASKSTACK                   1024	//����ջ��С����λ:�ֽ�

typedef struct msgbuf		//��Ϣ����ṹ
{
	REMIX_DLIST strList;	//��������
	U8 ucLength;		//��Ϣ����
	U8 aucBuf[MSGBUFLEN];	//��Ϣ����
} MSGBUF;

typedef struct bufpool		//��Ϣ����ؽṹ
{
	REMIX_DLIST strFreeList;	//����ؿ�������
	MSGBUF astrBufPool[BUFPOOLNUM];	//�����
} BUFPOOL;

/************************************************************************/
extern BUFPOOL gstrBufPool;
extern REMIX_QUEUE *gpstrSerialMsgQue;
extern REMIX_TCB *gpstrSerialTaskTcb;
//extern REMIX_SEM *gpstrSemMut;
extern REMIX_FLAG* gpstrFlag;

extern U8 buffer[5][TASKSTACK];
extern REMIX_MEM* pstrMemBuf;
extern U8* pucTask1Stack;
extern U8* pucTask2Stack;
extern U8* pucTask3Stack;
extern U8* pucTask4Stack;

extern U32 uiTask1StackSizeBackUp;
extern U32 uiTask2StackSizeBackUp;
extern U32 uiTask3StackSizeBackUp;
extern U32 uiTask4StackSizeBackUp;

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
extern void TEST_TestTask5(void *pvPara);
extern void TEST_SerialPrintTask(void *pvPara);
extern void TEST_TaskCreatePrint(REMIX_TCB * pstrTcb);
extern void TEST_TaskSwitchPrint(REMIX_TCB * pstrOldTcb, REMIX_TCB * pstrNewTcb);
extern void TEST_TaskDeletePrint(REMIX_TCB * pstrTcb);
#endif
