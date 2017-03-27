#include <stdlib.h>
#include "remix_private.h"

#ifdef REMIX_SEMGROUPFLAG

REMIX_FLAG *REMIX_FlagCreate(REMIX_FLAG * pstrFlag, U32 uiFlagOpt, U32 uiFlagBit)
{
	U8 *pucFlagMemAddr;

	if (REMIXFLAGSCHEDFIFO != (uiFlagOpt & REMIXFLAGSCHEDMASK)
	    && REMIXFLAGSCHEDPRIO != (uiFlagOpt & REMIXFLAGSCHEDMASK)) {
		return (REMIX_FLAG *) NULL;
	}

	if (NULL == pstrFlag) {

#ifdef REMIX_KERNEL_CRITICAL_ALL
		(void) REMIX_TaskLock(DISABLE_ALL_INTERRUPT);
#else
		(void) REMIX_TaskLock(DISABLE_SELECT_INTERRUPT);
#endif

		pucFlagMemAddr = malloc(sizeof(REMIX_FLAG));
		if (NULL == pucFlagMemAddr) {

#ifdef REMIX_KERNEL_CRITICAL_ALL
			(void) REMIX_TaskUnlock(DISABLE_ALL_INTERRUPT);
#else
			(void) REMIX_TaskUnlock(DISABLE_SELECT_INTERRUPT);
#endif

			return (REMIX_FLAG *) NULL;
		}
#ifdef REMIX_KERNEL_CRITICAL_ALL
		(void) REMIX_TaskUnlock(DISABLE_ALL_INTERRUPT);
#else
		(void) REMIX_TaskUnlock(DISABLE_SELECT_INTERRUPT);
#endif

		pstrFlag = (REMIX_FLAG *) pucFlagMemAddr;
	} else {
		pucFlagMemAddr = (U8 *) NULL;
	}

	REMIX_TaskFlagTableInit(&pstrFlag->strFlagTab);
	pstrFlag->uiFlagOpt = uiFlagOpt;
	pstrFlag->uiFlagNowBit = uiFlagBit;
	pstrFlag->pucFlagMem = pucFlagMemAddr;

	return pstrFlag;

}

U32 REMIX_FlagTake(REMIX_FLAG * pstrFlag, U32 uiFlagWantBit, U32 uiFlagNodeOpt, U32 uiDelayTick)
{
	U32 uiFlagSta;
	U8 ucConsume;

	if (NULL == pstrFlag) {
		return RTN_FAIL;
	}

	if ((REMIXFLAGWAITCLRAND != (uiFlagNodeOpt & REMIXFLAGWAITTYPEMASK))
	    && (REMIXFLAGWAITSETAND != (uiFlagNodeOpt & REMIXFLAGWAITTYPEMASK))
	    && (REMIXFLAGWAITCLROR != (uiFlagNodeOpt & REMIXFLAGWAITTYPEMASK))
	    && (REMIXFLAGWAITSETOR != (uiFlagNodeOpt & REMIXFLAGWAITTYPEMASK))) {
		return RTN_FAIL;
	}

	if ((RTN_SUCD == REMIX_RunInInt()) && (FLAGNOWAIT != uiDelayTick)) {
		return RTN_FAIL;
	}

	if (REMIXFLAGCONSUME == (uiFlagNodeOpt & REMIXFLAGCONSUMEMASK)) {
		ucConsume = 1;
		uiFlagNodeOpt &= ~REMIXFLAGCONSUME;
	} else {
		ucConsume = 0;
	}

#ifdef REMIX_KERNEL_CRITICAL_ALL
	(void) REMIX_TaskLock(DISABLE_ALL_INTERRUPT);
#else
	(void) REMIX_TaskLock(DISABLE_SELECT_INTERRUPT);
#endif

	switch (uiFlagNodeOpt) {

	case REMIXFLAGWAITSETAND:

		uiFlagSta = pstrFlag->uiFlagNowBit & uiFlagWantBit;

		if (uiFlagSta == uiFlagWantBit) {

			if (1 == ucConsume) {
				pstrFlag->uiFlagNowBit &= ~uiFlagSta;
			}
#ifdef REMIX_KERNEL_CRITICAL_ALL
			(void) REMIX_TaskUnlock(DISABLE_ALL_INTERRUPT);
#else
			(void) REMIX_TaskUnlock(DISABLE_SELECT_INTERRUPT);
#endif

			return RTN_SUCD;
		} else {

			if (FLAGNOWAIT == uiDelayTick) {

#ifdef REMIX_KERNEL_CRITICAL_ALL
				(void) REMIX_TaskUnlock(DISABLE_ALL_INTERRUPT);
#else
				(void) REMIX_TaskUnlock(DISABLE_SELECT_INTERRUPT);
#endif

				return RTN_FLAGTASKRETURN;

			}

			if (RTN_FAIL == REMIX_FlagBlock(pstrFlag, uiFlagWantBit, uiFlagNodeOpt, uiDelayTick)) {
#ifdef REMIX_KERNEL_CRITICAL_ALL
				(void) REMIX_TaskUnlock(DISABLE_ALL_INTERRUPT);
#else
				(void) REMIX_TaskUnlock(DISABLE_SELECT_INTERRUPT);
#endif
				return RTN_FAIL;
			}
#ifdef REMIX_KERNEL_CRITICAL_ALL
			(void) REMIX_TaskUnlock(DISABLE_ALL_INTERRUPT);
#else
			(void) REMIX_TaskUnlock(DISABLE_SELECT_INTERRUPT);
#endif

			REMIX_TaskSwiSched();

			if ((RTN_SUCD == gpstrCurTcb->strTaskOpt.uiDelayTick) && (1 == ucConsume)) {
				pstrFlag->uiFlagNowBit &= ~uiFlagWantBit;
			}

			return gpstrCurTcb->strTaskOpt.uiDelayTick;

		}

	case REMIXFLAGWAITSETOR:

		uiFlagSta = pstrFlag->uiFlagNowBit & uiFlagWantBit;

		if (uiFlagSta != (U32) 0) {

			if (1 == ucConsume) {
				pstrFlag->uiFlagNowBit &= ~uiFlagSta;
			}
#ifdef REMIX_KERNEL_CRITICAL_ALL
			(void) REMIX_TaskUnlock(DISABLE_ALL_INTERRUPT);
#else
			(void) REMIX_TaskUnlock(DISABLE_SELECT_INTERRUPT);
#endif

			return RTN_SUCD;
		} else {
			if (FLAGNOWAIT == uiDelayTick) {

#ifdef REMIX_KERNEL_CRITICAL_ALL
				(void) REMIX_TaskUnlock(DISABLE_ALL_INTERRUPT);
#else
				(void) REMIX_TaskUnlock(DISABLE_SELECT_INTERRUPT);
#endif

				return RTN_FLAGTASKRETURN;
			}

			if (RTN_FAIL == REMIX_FlagBlock(pstrFlag, uiFlagWantBit, uiFlagNodeOpt, uiDelayTick)) {
#ifdef REMIX_KERNEL_CRITICAL_ALL
				(void) REMIX_TaskUnlock(DISABLE_ALL_INTERRUPT);
#else
				(void) REMIX_TaskUnlock(DISABLE_SELECT_INTERRUPT);
#endif
				return RTN_FAIL;
			}
#ifdef REMIX_KERNEL_CRITICAL_ALL
			(void) REMIX_TaskUnlock(DISABLE_ALL_INTERRUPT);
#else
			(void) REMIX_TaskUnlock(DISABLE_SELECT_INTERRUPT);
#endif

			REMIX_TaskSwiSched();

			if ((RTN_SUCD == gpstrCurTcb->strTaskOpt.uiDelayTick) && (1 == ucConsume)) {
				pstrFlag->uiFlagNowBit &= ~uiFlagWantBit;
			}

			return gpstrCurTcb->strTaskOpt.uiDelayTick;

		}


	case REMIXFLAGWAITCLRAND:

		uiFlagSta = ~pstrFlag->uiFlagNowBit & uiFlagWantBit;

		if (uiFlagSta == uiFlagWantBit) {

			if (1 == ucConsume) {
				pstrFlag->uiFlagNowBit |= uiFlagSta;
			}
#ifdef REMIX_KERNEL_CRITICAL_ALL
			(void) REMIX_TaskUnlock(DISABLE_ALL_INTERRUPT);
#else
			(void) REMIX_TaskUnlock(DISABLE_SELECT_INTERRUPT);
#endif

			return RTN_SUCD;

		} else {
			if (FLAGNOWAIT == uiDelayTick) {

#ifdef REMIX_KERNEL_CRITICAL_ALL
				(void) REMIX_TaskUnlock(DISABLE_ALL_INTERRUPT);
#else
				(void) REMIX_TaskUnlock(DISABLE_SELECT_INTERRUPT);
#endif

				return RTN_FLAGTASKRETURN;

			}

			if (RTN_FAIL == REMIX_FlagBlock(pstrFlag, uiFlagWantBit, uiFlagNodeOpt, uiDelayTick)) {
#ifdef REMIX_KERNEL_CRITICAL_ALL
				(void) REMIX_TaskUnlock(DISABLE_ALL_INTERRUPT);
#else
				(void) REMIX_TaskUnlock(DISABLE_SELECT_INTERRUPT);
#endif
				return RTN_FAIL;
			}
#ifdef REMIX_KERNEL_CRITICAL_ALL
			(void) REMIX_TaskUnlock(DISABLE_ALL_INTERRUPT);
#else
			(void) REMIX_TaskUnlock(DISABLE_SELECT_INTERRUPT);
#endif

			REMIX_TaskSwiSched();

			if ((RTN_SUCD == gpstrCurTcb->strTaskOpt.uiDelayTick) && (1 == ucConsume)) {
				pstrFlag->uiFlagNowBit |= uiFlagWantBit;
			}

			return gpstrCurTcb->strTaskOpt.uiDelayTick;

		}

	case REMIXFLAGWAITCLROR:

		uiFlagSta = ~pstrFlag->uiFlagNowBit & uiFlagWantBit;

		if (uiFlagSta != (U32) 0) {

			if (1 == ucConsume) {
				pstrFlag->uiFlagNowBit |= uiFlagSta;
			}
#ifdef REMIX_KERNEL_CRITICAL_ALL
			(void) REMIX_TaskUnlock(DISABLE_ALL_INTERRUPT);
#else
			(void) REMIX_TaskUnlock(DISABLE_SELECT_INTERRUPT);
#endif

			return RTN_SUCD;

		} else {
			if (FLAGNOWAIT == uiDelayTick) {

#ifdef REMIX_KERNEL_CRITICAL_ALL
				(void) REMIX_TaskUnlock(DISABLE_ALL_INTERRUPT);
#else
				(void) REMIX_TaskUnlock(DISABLE_SELECT_INTERRUPT);
#endif

				return RTN_FLAGTASKRETURN;

			}

			if (RTN_FAIL == REMIX_FlagBlock(pstrFlag, uiFlagWantBit, uiFlagNodeOpt, uiDelayTick)) {
#ifdef REMIX_KERNEL_CRITICAL_ALL
				(void) REMIX_TaskUnlock(DISABLE_ALL_INTERRUPT);
#else
				(void) REMIX_TaskUnlock(DISABLE_SELECT_INTERRUPT);
#endif
				return RTN_FAIL;
			}
#ifdef REMIX_KERNEL_CRITICAL_ALL
			(void) REMIX_TaskUnlock(DISABLE_ALL_INTERRUPT);
#else
			(void) REMIX_TaskUnlock(DISABLE_SELECT_INTERRUPT);
#endif

			REMIX_TaskSwiSched();

			if ((RTN_SUCD == gpstrCurTcb->strTaskOpt.uiDelayTick) && (1 == ucConsume)) {
				pstrFlag->uiFlagNowBit |= uiFlagWantBit;
			}

			return gpstrCurTcb->strTaskOpt.uiDelayTick;

		}

	default:

#ifdef REMIX_KERNEL_CRITICAL_ALL
		(void) REMIX_TaskUnlock(DISABLE_ALL_INTERRUPT);
#else
		(void) REMIX_TaskUnlock(DISABLE_SELECT_INTERRUPT);
#endif

		return RTN_FAIL;
	}
}


U32 REMIX_FlagGive(REMIX_FLAG * pstrFlag, U32 uiSetBit, U8 ucOpt)
{
	REMIX_TCB *pstrTcb;
	REMIX_DLIST *pstrList;
	REMIX_DLIST *pstrNode;
	REMIX_PRIOFLAG *pstrPrioFlag;
	PRIORITYBITS ucTaskPrio;

	if (NULL == pstrFlag) {
		return RTN_FAIL;
	}

	if ((REMIXFLAGCLR != (ucOpt & REMIXFLAGSETMASK))
	    && (REMIXFLAGSET != (ucOpt & REMIXFLAGSETMASK))) {
		return RTN_FAIL;
	}
#ifdef REMIX_KERNEL_CRITICAL_ALL
	(void) REMIX_TaskLock(DISABLE_ALL_INTERRUPT);
#else
	(void) REMIX_TaskLock(DISABLE_SELECT_INTERRUPT);
#endif

	switch (ucOpt) {
	case REMIXFLAGCLR:
		pstrFlag->uiFlagNowBit &= ~uiSetBit;
		break;
	case REMIXFLAGSET:
		pstrFlag->uiFlagNowBit |= uiSetBit;
		break;
	default:
#ifdef REMIX_KERNEL_CRITICAL_ALL
		(void) REMIX_TaskUnlock(DISABLE_ALL_INTERRUPT);
#else
		(void) REMIX_TaskUnlock(DISABLE_SELECT_INTERRUPT);
#endif
		return RTN_FAIL;
	}

	pstrTcb = REMIX_TaskFlagTableSche(pstrFlag);

	if (NULL != pstrTcb) {

		(void) REMIX_TaskDeleteFromFlagTable(pstrTcb);

		if (DELAYQUEFLAG == (DELAYQUEFLAG & pstrTcb->uiTaskFlag)) {
			pstrNode = &pstrTcb->strTcbQue.strQueHead;
			(void) REMIX_TaskDeleteFromDelayTable(pstrNode);
			pstrTcb->uiTaskFlag &= (~((U32) DELAYQUEFLAG));
		}

		pstrTcb->strTaskOpt.ucTaskSta &= (~((U8) TASKPEND));
		pstrTcb->strTaskOpt.uiDelayTick = RTN_SUCD;
		pstrTcb->uiTaskFlag &= (~((U32) TASKSEMGROUPFLAG));

		pstrNode = &pstrTcb->strTcbQue.strQueHead;
		ucTaskPrio = pstrTcb->ucTaskPrio;
		pstrList = &gstrReadyTab.astrList[ucTaskPrio];
		pstrPrioFlag = &gstrReadyTab.strFlag;
		REMIX_TaskAddToReadyTable(pstrList, pstrNode, pstrPrioFlag, ucTaskPrio);
		pstrTcb->strTaskOpt.ucTaskSta |= TASKREADY;

#ifdef REMIX_KERNEL_CRITICAL_ALL
		(void) REMIX_TaskUnlock(DISABLE_ALL_INTERRUPT);
#else
		(void) REMIX_TaskUnlock(DISABLE_SELECT_INTERRUPT);
#endif

		REMIX_TaskSwiSched();

		return RTN_SUCD;
	}
#ifdef REMIX_KERNEL_CRITICAL_ALL
	(void) REMIX_TaskUnlock(DISABLE_ALL_INTERRUPT);
#else
	(void) REMIX_TaskUnlock(DISABLE_SELECT_INTERRUPT);
#endif
	return RTN_SUCD;
}

U32 REMIX_FlagFlushValue(REMIX_FLAG * pstrFlag, U32 uiRtnValue)
{
	REMIX_TCB *pstrTcb;
	REMIX_DLIST *pstrList;
	REMIX_DLIST *pstrNode;
	REMIX_PRIOFLAG *pstrPrioFlag;
	PRIORITYBITS ucTaskPrio;

	if (NULL == pstrFlag) {
		return RTN_FAIL;
	}
#ifdef REMIX_KERNEL_CRITICAL_ALL
	(void) REMIX_TaskLock(DISABLE_ALL_INTERRUPT);
#else
	(void) REMIX_TaskLock(DISABLE_SELECT_INTERRUPT);
#endif

	while (1) {
		pstrTcb = REMIX_TaskFlagTableCheck(pstrFlag);

		if (NULL != pstrTcb) {
			(void) REMIX_TaskDeleteFromFlagTable(pstrTcb);

			if (DELAYQUEFLAG == (DELAYQUEFLAG & pstrTcb->uiTaskFlag)) {
				pstrNode = &pstrTcb->strTcbQue.strQueHead;
				(void) REMIX_TaskDeleteFromDelayTable(pstrNode);
				pstrTcb->uiTaskFlag &= (~((U32) DELAYQUEFLAG));
			}

			pstrTcb->strTaskOpt.ucTaskSta &= (~((U8) TASKPEND));
			pstrTcb->strTaskOpt.uiDelayTick = uiRtnValue;
			pstrTcb->uiTaskFlag &= (~((U32) TASKSEMGROUPFLAG));

			pstrNode = &pstrTcb->strTcbQue.strQueHead;
			ucTaskPrio = pstrTcb->ucTaskPrio;
			pstrList = &gstrReadyTab.astrList[ucTaskPrio];
			pstrPrioFlag = &gstrReadyTab.strFlag;
			REMIX_TaskAddToReadyTable(pstrList, pstrNode, pstrPrioFlag, ucTaskPrio);
			pstrTcb->strTaskOpt.ucTaskSta |= TASKREADY;
		} else {
			break;
		}
	}

#ifdef REMIX_KERNEL_CRITICAL_ALL
	(void) REMIX_TaskUnlock(DISABLE_ALL_INTERRUPT);
#else
	(void) REMIX_TaskUnlock(DISABLE_SELECT_INTERRUPT);
#endif

	return RTN_SUCD;
}

U32 REMIX_FlagFlush(REMIX_FLAG * pstrFlag)
{
	return REMIX_FlagFlushValue(pstrFlag, RTN_SUCD);
}

U32 REMIX_FlagDelete(REMIX_FLAG * pstrFlag)
{
	if (NULL == pstrFlag) {
		return RTN_FAIL;
	}

	if (RTN_SUCD != REMIX_FlagFlushValue(pstrFlag, RTN_FLAGTASKDELETE)) {
		return RTN_FAIL;
	}

	if (NULL != pstrFlag->pucFlagMem) {

#ifdef REMIX_KERNEL_CRITICAL_ALL
		(void) REMIX_TaskLock(DISABLE_ALL_INTERRUPT);
#else
		(void) REMIX_TaskLock(DISABLE_SELECT_INTERRUPT);
#endif

		free(pstrFlag->pucFlagMem);
		pstrFlag->pucFlagMem = (U8 *) NULL;
#ifdef REMIX_KERNEL_CRITICAL_ALL
		(void) REMIX_TaskUnlock(DISABLE_ALL_INTERRUPT);
#else
		(void) REMIX_TaskUnlock(DISABLE_SELECT_INTERRUPT);
#endif
	}

	return RTN_SUCD;
}

#endif
