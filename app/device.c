#include "device.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

BUFPOOL gstrBufPool;

void DEV_SoftwareInit(void)
{
	DEV_BufferInit(&gstrBufPool);

	gpstrSerialMsgQue = REMIX_QueueCreate((REMIX_QUEUE *) NULL, QUEUEPRIO);
//  gpstrSemMut = REMIX_SemCreate((REMIX_SEM *) NULL, SEMPRIINH | SEMMUT | SEMPRIO, SEMFULL);
	gpstrFlag = REMIX_FlagCreate((REMIX_FLAG *) NULL, REMIXFLAGSCHEDFIFO, (U32) 0);

#ifdef REMIX_INCLUDETASKHOOK
	REMIX_TaskCreateHookAdd(TEST_TaskCreatePrint);
	REMIX_TaskSwitchHookAdd(TEST_TaskSwitchPrint);
	REMIX_TaskDeleteHookAdd(TEST_TaskDeletePrint);

#endif

#ifdef REMIX_TASKROUNDROBIN
	REMIX_TaskTimeSlice(50, 5);
//  REMIX_TaskTimeSlice(60, 4);
#endif

}

void DEV_HardwareInit(void)
{
	DEV_UartInit();
}

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

void DEV_PutChar(U8 ucChar)
{
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, ucChar);
}

void DEV_PutStrToMem(U8 * pvStringPt, ...)
{
	MSGBUF *pstrMsgBuf;
	va_list args;

	pstrMsgBuf = (MSGBUF *) DEV_BufferAlloc(&gstrBufPool);
	if (NULL == pstrMsgBuf) {
		return;
	}

	va_start(args, pvStringPt);
	(void) vsprintf(pstrMsgBuf->aucBuf, pvStringPt, args);
	va_end(args);

	pstrMsgBuf->ucLength = strlen(pstrMsgBuf->aucBuf);

	(void) REMIX_QueuePutNode(gpstrSerialMsgQue, &pstrMsgBuf->strList);

}

void DEV_PrintMsg(U8 * pucBuf, U32 uiLen)
{
	U32 uiCounter;
	uiCounter = 0;

	while (uiLen > uiCounter) {
		(void) DEV_PutChar(pucBuf[uiCounter++]);
	}
}

void DEV_BufferInit(BUFPOOL * pstrBufPool)
{
	U32 i;

	REMIX_DlistInit(&pstrBufPool->strFreeList);

	for (i = 0; i < BUFPOOLNUM; i++) {
		REMIX_DlistNodeAdd(&pstrBufPool->strFreeList, &pstrBufPool->astrBufPool[i].strList);
	}
}

MSGBUF *DEV_BufferAlloc(BUFPOOL * pstrBufPool)
{
	MSGBUF *pstrBuf;

#ifdef REMIX_KERNEL_CRITICAL_ALL
	(void) REMIX_TaskLock(DISABLE_ALL_INTERRUPT);
#else
	(void) REMIX_TaskLock(DISABLE_SELECT_INTERRUPT);
#endif

	pstrBuf = (MSGBUF *) REMIX_DlistNodeDelete(&pstrBufPool->strFreeList);

#ifdef REMIX_KERNEL_CRITICAL_ALL
	(void) REMIX_TaskUnlock(DISABLE_ALL_INTERRUPT);
#else
	(void) REMIX_TaskUnlock(DISABLE_SELECT_INTERRUPT);
#endif

	return pstrBuf;
}

void DEV_BufferFree(BUFPOOL * pstrBufPool, REMIX_DLIST * pstrQueNode)
{
#ifdef REMIX_KERNEL_CRITICAL_ALL
	(void) REMIX_TaskLock(DISABLE_ALL_INTERRUPT);
#else
	(void) REMIX_TaskLock(DISABLE_SELECT_INTERRUPT);
#endif

	REMIX_DlistNodeAdd(&pstrBufPool->strFreeList, pstrQueNode);

#ifdef REMIX_KERNEL_CRITICAL_ALL
	(void) REMIX_TaskUnlock(DISABLE_ALL_INTERRUPT);
#else
	(void) REMIX_TaskUnlock(DISABLE_SELECT_INTERRUPT);
#endif
}
