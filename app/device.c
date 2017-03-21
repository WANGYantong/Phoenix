#include "device.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

BUFPOOL gstrBufPool;		//消息缓冲池

/**********************************************/
//函数功能:初始化软件
//输入参数:none
//返回值  :none
/**********************************************/
void DEV_SoftwareInit(void)
{
	DEV_BufferInit(&gstrBufPool);	//初始化消息缓冲池

	gpstrSerialMsgQue = REMIX_QueueCreate((REMIX_QUEUE *) NULL, QUEUEPRIO);	//创建打印消息的队列
//      gpstrSemMut = REMIX_SemCreate((REMIX_SEM *) NULL, SEMPRIINH | SEMMUT | SEMPRIO, SEMFULL);
	gpstrFlag = REMIX_FlagCreate((REMIX_FLAG *) NULL, REMIXFLAGSCHEDFIFO, (U32) 0);

#ifdef REMIX_INCLUDETASKHOOK
	REMIX_TaskCreateHookAdd(TEST_TaskCreatePrint);
	REMIX_TaskSwitchHookAdd(TEST_TaskSwitchPrint);
	REMIX_TaskDeleteHookAdd(TEST_TaskDeletePrint);

#endif

#ifdef REMIX_TASKROUNDROBIN
	REMIX_TaskTimeSlice(50, 5);
//     REMIX_TaskTimeSlice(60, 4);
#endif

}

/**********************************************/
//函数功能:初始化硬件
//输入参数:none
//返回值  :none
/**********************************************/
void DEV_HardwareInit(void)
{
	DEV_UartInit();
}

/**********************************************/
//函数功能:初始化串口
//输入参数:none
//返回值  :none
/**********************************************/
void DEV_UartInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_StructInit(&USART_InitStructure);
	USART_Init(USART1, &USART_InitStructure);
	USART_Cmd(USART1, ENABLE);

}

/**********************************************/
//函数功能:向串口发送一个字符
//输入参数:ucChar:待输出的字符
//返回值  :none
/**********************************************/
void DEV_PutChar(U8 ucChar)
{
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, ucChar);
}

/**********************************************/
//函数功能:将字符串打印到内存，该函数入口参数与printf参数一致
//输入参数:pvStringPt:第一个参数的指针，必须为字符串指针
//         ...:其他参数
//返回值  :none
/**********************************************/
void DEV_PutStrToMem(U8 * pvStringPt, ...)
{
	MSGBUF *pstrMsgBuf;
	va_list args;

	/* 申请buf, 用来存放需要打印的字符 */
	pstrMsgBuf = (MSGBUF *) DEV_BufferAlloc(&gstrBufPool);
	if (NULL == pstrMsgBuf) {
		return;
	}

	/* 将字符串打印到内存 */
	va_start(args, pvStringPt);
	(void) vsprintf(pstrMsgBuf->aucBuf, pvStringPt, args);
	va_end(args);

	/* 填充buf参数 */
	pstrMsgBuf->ucLength = strlen(pstrMsgBuf->aucBuf);

	/* 将buf挂入队列 */
	(void) REMIX_QueuePutNode(gpstrSerialMsgQue, &pstrMsgBuf->strList);


}

/**********************************************/
//函数功能:从内存中指定位置向串口打印数据
//输入参数:pucBuf:需要打印数据存放的缓冲地址
//         uiLen:需要打印的数据长度，单位:字节
//返回值  :none
/**********************************************/
void DEV_PrintMsg(U8 * pucBuf, U32 uiLen)
{
	U32 uiCounter;
	uiCounter = 0;

	while (uiLen > uiCounter) {
		(void) DEV_PutChar(pucBuf[uiCounter++]);
	}
}

/**********************************************/
//函数功能:初始化消息缓冲池
//输入参数:pstrBufPool:消息缓冲池指针
//返回值  :none
/**********************************************/
void DEV_BufferInit(BUFPOOL * pstrBufPool)
{
	U32 i;

	REMIX_DlistInit(&pstrBufPool->strFreeList);

	for (i = 0; i < BUFPOOLNUM; i++) {
		REMIX_DlistNodeAdd(&pstrBufPool->strFreeList, &pstrBufPool->astrBufPool[i].strList);
	}
}


/**********************************************/
//函数功能:从消息缓冲池中申请消息缓冲
//输入参数:pstrBufPool:消息缓冲池指针
//返回值  :分配的消息缓冲指针
/**********************************************/
MSGBUF *DEV_BufferAlloc(BUFPOOL * pstrBufPool)
{
	MSGBUF *pstrBuf;

	(void) REMIX_IntLock();

	pstrBuf = (MSGBUF *) REMIX_DlistNodeDelete(&pstrBufPool->strFreeList);

	(void) REMIX_IntUnlock();

	return pstrBuf;
}


/**********************************************/
//函数功能:将消息缓冲释放回消息缓冲池
//输入参数:pstrBufPool:消息缓冲池指针
//         pstrQueNode:释放的缓冲节点指针
//返回值  :none
/**********************************************/
void DEV_BufferFree(BUFPOOL * pstrBufPool, REMIX_DLIST * pstrQueNode)
{
	(void) REMIX_IntLock();

	REMIX_DlistNodeAdd(&pstrBufPool->strFreeList, pstrQueNode);

	(void) REMIX_IntUnlock();
}
