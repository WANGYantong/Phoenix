#include <stdlib.h>
#include "remix_private.h"

REMIX_QUEUE *REMIX_QueueCreate(REMIX_QUEUE * pstrQue, U32 uiQueOpt)
{
	U8 *pucQueueMemAddr;

	if ((QUEUEFIFO != (QUEUESCHEDULEMASK & uiQueOpt))
	    && (QUEUEPRIO != (QUEUESCHEDULEMASK & uiQueOpt))) {
		return (REMIX_QUEUE *) NULL;
	}

	if ((U32) NULL != ((~QUEUESCHEDULEMASK) & uiQueOpt)) {
		return (REMIX_QUEUE *) NULL;
	}

	if (NULL == pstrQue) {

#ifdef REMIX_KERNEL_CRITICAL_ALL
		(void) REMIX_TaskLock(DISABLE_ALL_INTERRUPT);
#else
		(void) REMIX_TaskLock(DISABLE_SELECT_INTERRUPT);
#endif

		pucQueueMemAddr = malloc(sizeof(REMIX_QUEUE));
		if (NULL == pucQueueMemAddr) {
#ifdef REMIX_KERNEL_CRITICAL_ALL
			(void) REMIX_TaskUnlock(DISABLE_ALL_INTERRUPT);
#else
			(void) REMIX_TaskUnlock(DISABLE_SELECT_INTERRUPT);
#endif
			return (REMIX_QUEUE *) NULL;
		}
#ifdef REMIX_KERNEL_CRITICAL_ALL
		(void) REMIX_TaskUnlock(DISABLE_ALL_INTERRUPT);
#else
		(void) REMIX_TaskUnlock(DISABLE_SELECT_INTERRUPT);
#endif

		pstrQue = (REMIX_QUEUE *) pucQueueMemAddr;
	} else {
		pucQueueMemAddr = (U8 *) NULL;
	}

	REMIX_DlistInit(&pstrQue->strList);

	pstrQue->pucQueueMem = pucQueueMemAddr;

	if (NULL != REMIX_SemCreate(&pstrQue->strSem, SEMCNT | uiQueOpt, SEMEMPTY)) {
		return pstrQue;
	} else {
		if (NULL != pucQueueMemAddr) {
			free(pucQueueMemAddr);
		}
		return (REMIX_QUEUE *) NULL;
	}
}

U32 REMIX_QueuePutNode(REMIX_QUEUE * pstrQue, REMIX_DLIST * pstrQueNode)
{
	if ((NULL == pstrQue) || (NULL == pstrQueNode)) {
		return RTN_FAIL;
	}
#ifdef REMIX_KERNEL_CRITICAL_ALL
	(void) REMIX_TaskLock(DISABLE_ALL_INTERRUPT);
#else
	(void) REMIX_TaskLock(DISABLE_SELECT_INTERRUPT);
#endif

	REMIX_DlistNodeAdd(&pstrQue->strList, pstrQueNode);

#ifdef REMIX_KERNEL_CRITICAL_ALL
	(void) REMIX_TaskUnlock(DISABLE_ALL_INTERRUPT);
#else
	(void) REMIX_TaskUnlock(DISABLE_SELECT_INTERRUPT);
#endif

	return REMIX_SemGive(&pstrQue->strSem);
}

U32 REMIX_QueueGetNode(REMIX_QUEUE * pstrQue, REMIX_DLIST ** ppstrQueNode, U32 uiDelayTick)
{
	REMIX_DLIST *pstrQueNode;
	U32 uiRtn;

	if ((NULL == pstrQue) || (NULL == ppstrQueNode)) {
		return RTN_FAIL;
	}

	uiRtn = REMIX_SemTake(&pstrQue->strSem, uiDelayTick);
	if (RTN_SUCD != uiRtn) {
		return uiRtn;
	}
#ifdef REMIX_KERNEL_CRITICAL_ALL
	(void) REMIX_TaskLock(DISABLE_ALL_INTERRUPT);
#else
	(void) REMIX_TaskLock(DISABLE_SELECT_INTERRUPT);
#endif

	pstrQueNode = REMIX_DlistNodeDelete(&pstrQue->strList);

#ifdef REMIX_KERNEL_CRITICAL_ALL
	(void) REMIX_TaskUnlock(DISABLE_ALL_INTERRUPT);
#else
	(void) REMIX_TaskUnlock(DISABLE_SELECT_INTERRUPT);
#endif

	*ppstrQueNode = pstrQueNode;

	return RTN_SUCD;

}

U32 REMIX_QueueDelete(REMIX_QUEUE * pstrQue)
{
	if (NULL == pstrQue) {
		return RTN_FAIL;
	}

	if (RTN_SUCD != REMIX_SemFlushValue(&pstrQue->strSem, RTN_SEMTASKDELETE)) {
		return RTN_FAIL;
	}

	if (NULL != pstrQue->pucQueueMem) {

#ifdef REMIX_KERNEL_CRITICAL_ALL
		(void) REMIX_TaskLock(DISABLE_ALL_INTERRUPT);
#else
		(void) REMIX_TaskLock(DISABLE_SELECT_INTERRUPT);
#endif

		free(pstrQue->pucQueueMem);
		pstrQue->pucQueueMem = (U8 *) NULL;
#ifdef REMIX_KERNEL_CRITICAL_ALL
		(void) REMIX_TaskUnlock(DISABLE_ALL_INTERRUPT);
#else
		(void) REMIX_TaskUnlock(DISABLE_SELECT_INTERRUPT);
#endif
	}

	return RTN_SUCD;

}
