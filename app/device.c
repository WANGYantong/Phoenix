#include "device.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

BUFPOOL gstrBufPool;		//��Ϣ�����

/**********************************************/
//��������:��ʼ�����
//�������:none
//����ֵ  :none
/**********************************************/
void DEV_SoftwareInit(void)
{
	DEV_BufferInit(&gstrBufPool);	//��ʼ����Ϣ�����

	gpstrSerialMsgQue = REMIX_QueueCreate((REMIX_QUEUE *) NULL, QUEUEPRIO);	//������ӡ��Ϣ�Ķ���
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
//��������:��ʼ��Ӳ��
//�������:none
//����ֵ  :none
/**********************************************/
void DEV_HardwareInit(void)
{
	DEV_UartInit();
}

/**********************************************/
//��������:��ʼ������
//�������:none
//����ֵ  :none
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
//��������:�򴮿ڷ���һ���ַ�
//�������:ucChar:��������ַ�
//����ֵ  :none
/**********************************************/
void DEV_PutChar(U8 ucChar)
{
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, ucChar);
}

/**********************************************/
//��������:���ַ�����ӡ���ڴ棬�ú�����ڲ�����printf����һ��
//�������:pvStringPt:��һ��������ָ�룬����Ϊ�ַ���ָ��
//         ...:��������
//����ֵ  :none
/**********************************************/
void DEV_PutStrToMem(U8 * pvStringPt, ...)
{
	MSGBUF *pstrMsgBuf;
	va_list args;

	/* ����buf, ���������Ҫ��ӡ���ַ� */
	pstrMsgBuf = (MSGBUF *) DEV_BufferAlloc(&gstrBufPool);
	if (NULL == pstrMsgBuf) {
		return;
	}

	/* ���ַ�����ӡ���ڴ� */
	va_start(args, pvStringPt);
	(void) vsprintf(pstrMsgBuf->aucBuf, pvStringPt, args);
	va_end(args);

	/* ���buf���� */
	pstrMsgBuf->ucLength = strlen(pstrMsgBuf->aucBuf);

	/* ��buf������� */
	(void) REMIX_QueuePutNode(gpstrSerialMsgQue, &pstrMsgBuf->strList);


}

/**********************************************/
//��������:���ڴ���ָ��λ���򴮿ڴ�ӡ����
//�������:pucBuf:��Ҫ��ӡ���ݴ�ŵĻ����ַ
//         uiLen:��Ҫ��ӡ�����ݳ��ȣ���λ:�ֽ�
//����ֵ  :none
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
//��������:��ʼ����Ϣ�����
//�������:pstrBufPool:��Ϣ�����ָ��
//����ֵ  :none
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
//��������:����Ϣ�������������Ϣ����
//�������:pstrBufPool:��Ϣ�����ָ��
//����ֵ  :�������Ϣ����ָ��
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
//��������:����Ϣ�����ͷŻ���Ϣ�����
//�������:pstrBufPool:��Ϣ�����ָ��
//         pstrQueNode:�ͷŵĻ���ڵ�ָ��
//����ֵ  :none
/**********************************************/
void DEV_BufferFree(BUFPOOL * pstrBufPool, REMIX_DLIST * pstrQueNode)
{
	(void) REMIX_IntLock();

	REMIX_DlistNodeAdd(&pstrBufPool->strFreeList, pstrQueNode);

	(void) REMIX_IntUnlock();
}
