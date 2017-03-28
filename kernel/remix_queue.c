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

        (void)REMIX_InterruptLock();

		pucQueueMemAddr = malloc(sizeof(REMIX_QUEUE));
		if (NULL == pucQueueMemAddr) {

            (void)REMIX_InterruptUnlock();

			return (REMIX_QUEUE *) NULL;
		}

        (void)REMIX_InterruptUnlock();

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

    (void)REMIX_InterruptLock();

	REMIX_DlistNodeAdd(&pstrQue->strList, pstrQueNode);

    (void)REMIX_InterruptUnlock();

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

    (void)REMIX_InterruptLock();

	pstrQueNode = REMIX_DlistNodeDelete(&pstrQue->strList);

    (void)REMIX_InterruptUnlock();

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

        (void)REMIX_InterruptLock();

		free(pstrQue->pucQueueMem);
		pstrQue->pucQueueMem = (U8 *) NULL;
        (void)REMIX_InterruptUnlock();
	}

	return RTN_SUCD;

}
