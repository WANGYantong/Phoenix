#include <stdlib.h>
#include "remix_private.h"

REMIX_SEM *REMIX_SemCreate(REMIX_SEM * pstrSem, U32 uiSemOpt, U32 uiInitVal)
{
	U8 *pucSemMemAddr;

	if (((SEMBIN != (SEMTYPEMASK & uiSemOpt)) && (SEMCNT != (SEMTYPEMASK & uiSemOpt))
	     && (SEMMUT != (SEMTYPEMASK & uiSemOpt)))
	    || ((SEMFIFO != (SEMSCHEDULEMASK & uiSemOpt)) && (SEMPRIO != (SEMSCHEDULEMASK & uiSemOpt)))
	    || (((SEMMUT != (SEMTYPEMASK & uiSemOpt)) || (SEMPRIO != (SEMSCHEDULEMASK & uiSemOpt)))
		&& (SEMPRIINH == (SEMPRIORITYINHMASK & uiSemOpt)))) {
		return (REMIX_SEM *) NULL;
	}

	if (SEMBIN == (SEMTYPEMASK & uiSemOpt)) {
		if ((SEMEMPTY != uiInitVal) && (SEMFULL != uiInitVal)) {
			return (REMIX_SEM *) NULL;
		}
	} else if (SEMMUT == (SEMTYPEMASK & uiSemOpt)) {
		if (SEMFULL != uiInitVal) {
			return (REMIX_SEM *) NULL;
		}
	}

	if (NULL == pstrSem) {

        (void)REMIX_InterruptLock();

		pucSemMemAddr = malloc(sizeof(REMIX_SEM));
		if (NULL == pucSemMemAddr) {

            (void)REMIX_InterruptUnlock();

			return (REMIX_SEM *) NULL;
		}

        (void)REMIX_InterruptUnlock();

		pstrSem = (REMIX_SEM *) pucSemMemAddr;
	} else {
		pucSemMemAddr = (U8 *) NULL;
	}

	REMIX_TaskSemTableInit(&pstrSem->strSemTab);
	pstrSem->uiCounter = uiInitVal;
	pstrSem->uiSemOpt = uiSemOpt;
	pstrSem->pucSemMem = pucSemMemAddr;
	pstrSem->pstrSemTask = (REMIX_TCB *) NULL;

	return pstrSem;
}

U32 REMIX_SemTake(REMIX_SEM * pstrSem, U32 uiDelayTick)
{
	if (NULL == pstrSem) {
		return RTN_FAIL;
	}

	if (RTN_SUCD == REMIX_RunInInt()) {

		if (SEMMUT == (SEMTYPEMASK & pstrSem->uiSemOpt)) {
			return RTN_FAIL;
		} else {
			if (SEMNOWAIT != uiDelayTick) {
				return RTN_FAIL;
			}
		}
	}

    (void)REMIX_InterruptLock();

	if (SEMNOWAIT == uiDelayTick) {

		if (SEMBIN == (SEMTYPEMASK & pstrSem->uiSemOpt)) {
			if (SEMFULL == pstrSem->uiCounter) {
				pstrSem->uiCounter = SEMEMPTY;

                (void)REMIX_InterruptUnlock();

				return RTN_SUCD;
			} else {

                (void)REMIX_InterruptUnlock();

				return RTN_SEMTASKRETURN;
			}
		}

		else if (SEMCNT == (SEMTYPEMASK & pstrSem->uiSemOpt)) {
			if (SEMEMPTY != pstrSem->uiCounter) {
				pstrSem->uiCounter--;

                (void)REMIX_InterruptUnlock();

				return RTN_SUCD;
			} else {

                (void)REMIX_InterruptUnlock();

				return RTN_SEMTASKRETURN;
			}
		}

		else {
			if (SEMFULL == pstrSem->uiCounter) {
				pstrSem->pstrSemTask = gpstrCurTcb;
				pstrSem->uiCounter--;

                (void)REMIX_InterruptUnlock();

				return RTN_SUCD;
			} else {
				if (gpstrCurTcb == pstrSem->pstrSemTask) {
					if (SEMEMPTY != pstrSem->uiCounter) {
						pstrSem->uiCounter--;

                        (void)REMIX_InterruptUnlock();

						return RTN_SUCD;
					} else {

                        (void)REMIX_InterruptUnlock();

						return RTN_SEMTASKOVERFLOW;
					}
				} else {

                    (void)REMIX_InterruptUnlock();

					return RTN_SEMTASKRETURN;
				}
			}
		}
	}

	else {

		if (SEMBIN == (SEMTYPEMASK & pstrSem->uiSemOpt)) {
			if (SEMFULL == pstrSem->uiCounter) {
				pstrSem->uiCounter = SEMEMPTY;

                (void)REMIX_InterruptUnlock();

				return RTN_SUCD;
			} else {
				if (RTN_FAIL == REMIX_TaskPend(pstrSem, uiDelayTick)) {

                    (void)REMIX_InterruptUnlock();

					return RTN_FAIL;
				}

                (void)REMIX_InterruptUnlock();

				REMIX_TaskSwiSched();
				return gpstrCurTcb->strTaskOpt.uiDelayTick;
			}
		}

		else if (SEMCNT == (SEMTYPEMASK & pstrSem->uiSemOpt)) {
			if (SEMEMPTY != pstrSem->uiCounter) {
				pstrSem->uiCounter--;

                (void)REMIX_InterruptUnlock();

				return RTN_SUCD;
			} else {
				if (RTN_FAIL == REMIX_TaskPend(pstrSem, uiDelayTick)) {

                    (void)REMIX_InterruptUnlock();

					return RTN_FAIL;
				}

                (void)REMIX_InterruptUnlock();

				REMIX_TaskSwiSched();
				return gpstrCurTcb->strTaskOpt.uiDelayTick;
			}

		}

		else {
			if (SEMFULL == (pstrSem->uiCounter)) {
				pstrSem->pstrSemTask = gpstrCurTcb;
				pstrSem->uiCounter--;

                (void)REMIX_InterruptUnlock();

				return RTN_SUCD;
			} else {
				if (gpstrCurTcb == pstrSem->pstrSemTask) {
					if (SEMEMPTY != pstrSem->uiCounter) {
						pstrSem->uiCounter--;

                        (void)REMIX_InterruptUnlock();

						return RTN_SUCD;
					} else {

                        (void)REMIX_InterruptUnlock();

						return RTN_SEMTASKOVERFLOW;
					}
				} else {
#ifdef  REMIX_TASKPRIOINHER

					if (SEMPRIINH == (SEMPRIORITYINHMASK & pstrSem->uiSemOpt)) {
						REMIX_TaskPrioInheritance(pstrSem->pstrSemTask,
									  gpstrCurTcb->ucTaskPrio);
					}
#endif
					if (RTN_FAIL == REMIX_TaskPend(pstrSem, uiDelayTick)) {

                        (void)REMIX_InterruptUnlock();

						return RTN_FAIL;
					}

                    (void)REMIX_InterruptUnlock();

					REMIX_TaskSwiSched();
					return gpstrCurTcb->strTaskOpt.uiDelayTick;
				}
			}
		}

	}

}


U32 REMIX_SemGive(REMIX_SEM * pstrSem)
{
#define NOTCKECKPENDTASK            0
#define CHECKPENDTASK               1

	REMIX_TCB *pstrTcb;
	REMIX_DLIST *pstrList;
	REMIX_DLIST *pstrNode;
	REMIX_PRIOFLAG *pstrPrioFlag;
	U32 uiRtn;
	PRIORITYBITS ucTaskPrio;
	U8 ucPendTaskFlag;

	if (NULL == pstrSem) {
		return RTN_FAIL;
	}

	if (RTN_SUCD == REMIX_RunInInt()) {
		if (SEMMUT == (SEMTYPEMASK & pstrSem->uiSemOpt)) {
			return RTN_FAIL;
		}
	}

	uiRtn = RTN_SUCD;

    (void)REMIX_InterruptLock();

	if (SEMBIN == (SEMTYPEMASK & pstrSem->uiSemOpt)) {
		if (SEMEMPTY == pstrSem->uiCounter) {
			ucPendTaskFlag = CHECKPENDTASK;
		} else {
			ucPendTaskFlag = NOTCKECKPENDTASK;
		}
	}

	else if (SEMCNT == (SEMTYPEMASK & pstrSem->uiSemOpt)) {
		if (SEMEMPTY == pstrSem->uiCounter) {
			ucPendTaskFlag = CHECKPENDTASK;
		} else {
			ucPendTaskFlag = NOTCKECKPENDTASK;

			if (SEMFULL != pstrSem->uiCounter) {
				pstrSem->uiCounter++;
			} else {
				uiRtn = RTN_SEMGIVEOVERFLOW;
			}
		}
	}

	else {
		if (gpstrCurTcb != pstrSem->pstrSemTask) {

            (void)REMIX_InterruptUnlock();

			return RTN_FAIL;
		}

		pstrSem->uiCounter++;

		if (SEMFULL != pstrSem->uiCounter) {
			ucPendTaskFlag = NOTCKECKPENDTASK;
		} else {
			ucPendTaskFlag = CHECKPENDTASK;
		}
	}

	if (CHECKPENDTASK == ucPendTaskFlag) {

#ifdef REMIX_TASKPRIOINHER

		REMIX_TaskPrioResume(gpstrCurTcb);

#endif

		pstrTcb = REMIX_TaskSemTableSche(pstrSem);

		if (NULL != pstrTcb) {

			(void) REMIX_TaskDeleteFromSemTable(pstrTcb);

			if (DELAYQUEFLAG == (DELAYQUEFLAG & pstrTcb->uiTaskFlag)) {
				pstrNode = &pstrTcb->strTcbQue.strQueHead;
				(void) REMIX_TaskDeleteFromDelayTable(pstrNode);
				pstrTcb->uiTaskFlag &= (~((U32) DELAYQUEFLAG));
			}

			pstrTcb->strTaskOpt.ucTaskSta &= (~((U8) TASKPEND));
			pstrTcb->strTaskOpt.uiDelayTick = RTN_SUCD;

			pstrNode = &pstrTcb->strTcbQue.strQueHead;
			ucTaskPrio = pstrTcb->ucTaskPrio;
			pstrList = &gstrReadyTab.astrList[ucTaskPrio];
			pstrPrioFlag = &gstrReadyTab.strFlag;
			REMIX_TaskAddToReadyTable(pstrList, pstrNode, pstrPrioFlag, ucTaskPrio);
			pstrTcb->strTaskOpt.ucTaskSta |= TASKREADY;

			if (SEMMUT == (SEMTYPEMASK & pstrSem->uiSemOpt)) {
				pstrSem->pstrSemTask = pstrTcb;
				pstrSem->uiCounter--;
			}

            (void)REMIX_InterruptUnlock();

			REMIX_TaskSwiSched();

			return uiRtn;
		} else {

			if (SEMBIN == (SEMTYPEMASK & pstrSem->uiSemOpt)) {
				pstrSem->uiCounter = SEMFULL;
			}

			else if (SEMCNT == (SEMTYPEMASK & pstrSem->uiSemOpt)) {
				pstrSem->uiCounter++;
			}

			else {
				pstrSem->pstrSemTask = (REMIX_TCB *) NULL;
			}

		}
	}

    (void)REMIX_InterruptUnlock();

	return uiRtn;

}

U32 REMIX_SemFlushValue(REMIX_SEM * pstrSem, U32 uiRtnValue)
{
	REMIX_TCB *pstrTcb;
	REMIX_DLIST *pstrList;
	REMIX_DLIST *pstrNode;
	REMIX_PRIOFLAG *pstrPrioFlag;
	PRIORITYBITS ucTaskPrio;

	if (NULL == pstrSem) {
		return RTN_FAIL;
	}

	if (SEMMUT == (SEMTYPEMASK & pstrSem->uiSemOpt)) {
		return RTN_FAIL;
	}

    (void)REMIX_InterruptLock();

	while (1) {
		pstrTcb = REMIX_TaskSemTableSche(pstrSem);

		if (NULL != pstrTcb) {
			(void) REMIX_TaskDeleteFromSemTable(pstrTcb);

			if (DELAYQUEFLAG == (DELAYQUEFLAG & pstrTcb->uiTaskFlag)) {
				pstrNode = &pstrTcb->strTcbQue.strQueHead;
				(void) REMIX_TaskDeleteFromDelayTable(pstrNode);
				pstrTcb->uiTaskFlag &= (~((U32) DELAYQUEFLAG));
			}

			pstrTcb->strTaskOpt.ucTaskSta &= (~((U8) TASKPEND));
			pstrTcb->strTaskOpt.uiDelayTick = uiRtnValue;

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

	pstrSem->uiCounter = SEMEMPTY;

    (void)REMIX_InterruptUnlock();

	return RTN_SUCD;
}

U32 REMIX_SemFlush(REMIX_SEM * pstrSem)
{
	return REMIX_SemFlushValue(pstrSem, RTN_SUCD);
}

U32 REMIX_SemDelete(REMIX_SEM * pstrSem)
{
	if (NULL == pstrSem) {
		return RTN_FAIL;
	}

	if (SEMMUT != (SEMTYPEMASK & pstrSem->uiSemOpt)) {
		if (RTN_SUCD != REMIX_SemFlushValue(pstrSem, RTN_SEMTASKDELETE)) {
			return RTN_FAIL;
		}
	}

	if (NULL != pstrSem->pucSemMem) {

        (void)REMIX_InterruptLock();

		free(pstrSem->pucSemMem);
		pstrSem->pucSemMem = (U8 *) NULL;

        (void)REMIX_InterruptUnlock();
	}

	return RTN_SUCD;
}
