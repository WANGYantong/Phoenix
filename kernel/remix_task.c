#include <stdlib.h>
#include "remix_private.h"

REMIX_DLIST gstrTaskList;

#ifdef REMIX_TASKROUNDROBIN
U32 guiTimeSlice[PRIORITYNUM];
U32 gauiSliceCnt[PRIORITYNUM];
#endif

#ifdef REMIX_INCLUDETASKHOOK
VFHCRT gvfTaskCreateHook;
VFHSWT gvfTaskSwitchHook;
VFHDLT gvfTaskDeleteHook;
#endif

//*************************************************************************//
REMIX_TCB *REMIX_TaskCreate(U8 * pucTaskName, VFUNCTION vfFucPointer, void *pvPara, U8 * pucTaskStack, U32 uiStackSize,
			    PRIORITYBITS ucTaskPrio, REMIX_TASKOPT * pstrTaskOpt)
{
	REMIX_TCB *pstrTcb;

	if (NULL == vfFucPointer) {
		return (REMIX_TCB *) NULL;
	}

	if (0 == uiStackSize) {
		return (REMIX_TCB *) NULL;
	}

	if (NULL != pstrTaskOpt) {
		if ((TASKREADY != pstrTaskOpt->ucTaskSta) && (TASKDELAY != pstrTaskOpt->ucTaskSta)) {
			return (REMIX_TCB *) NULL;
		}
	}

	if (USERROOT == REMIX_GetUser()) {
		if (ucTaskPrio > LOWESTPRIORITY) {
			return (REMIX_TCB *) NULL;
		}
	} else {
		if ((ucTaskPrio < USERHIGHESTPRIORITY) || (ucTaskPrio > USERLOWESTPRIORITY)) {
			return (REMIX_TCB *) NULL;
		}
	}

	pstrTcb =
	    REMIX_TaskTcbInit(pucTaskName, vfFucPointer, pvPara, pucTaskStack, uiStackSize, ucTaskPrio, pstrTaskOpt);

	if (NULL == pstrTcb) {
		return (REMIX_TCB *) NULL;
	}

	if (SYSTEMSCHEDULE == guiSystemStatus) {

#ifdef REMIX_INCLUDETASKHOOK
		if (NULL != gvfTaskCreateHook) {
			gvfTaskCreateHook(pstrTcb);
		}
#endif

		REMIX_TaskSwiSched();
	}

	return pstrTcb;

}

REMIX_TCB *REMIX_TaskTcbInit(U8 * pucTaskName, VFUNCTION vfFuncPointer, void *pvPara, U8 * pucTaskStack,
			     U32 uiStackSize, PRIORITYBITS ucTaskPrio, REMIX_TASKOPT * pstrTaskOpt)
{
	REMIX_TCB *pstrTcb;
	REMIX_DLIST *pstrList;
	REMIX_DLIST *pstrNode;
	REMIX_PRIOFLAG *pstrPrioFlag;
	U8 *pucStackBy4;
	U32 uiTaskFlag;

#ifdef REMIX_KERNEL_CRITICAL_ALL
	(void) REMIX_TaskLock(DISABLE_ALL_INTERRUPT);
#else
	(void) REMIX_TaskLock(DISABLE_SELECT_INTERRUPT);
#endif

	if (NULL == pucTaskStack) {

		pucTaskStack = malloc(uiStackSize);

		if (NULL == pucTaskStack) {
#ifdef REMIX_KERNEL_CRITICAL_ALL
			(void) REMIX_TaskUnlock(DISABLE_ALL_INTERRUPT);
#else
			(void) REMIX_TaskUnlock(DISABLE_SELECT_INTERRUPT);
#endif
			return (REMIX_TCB *) NULL;
		}

		uiTaskFlag = TASKSTACKFLAG;

	} else {
		uiTaskFlag = 0;
	}

	pucStackBy4 = (U8 *) (((U32) pucTaskStack + uiStackSize) & STACKALIGNMASK);
	pstrTcb = (REMIX_TCB *) (((U32) pucStackBy4 - sizeof(REMIX_TCB)) & TCBALIGNMASK);

	pstrTcb->pucTaskStack = pucTaskStack;

	REMIX_TaskStackInit(pstrTcb, vfFuncPointer, pvPara);

	REMIX_TaskAddToTaskList(&pstrTcb->strTaskQue.strQueHead);

#ifdef REMIX_CPUSTATISTIC

	REMIX_TaskCPUShareInit(pstrTcb);

#endif

	pstrTcb->uiTaskFlag = 0;
	pstrTcb->uiTaskFlag |= uiTaskFlag;

	pstrTcb->strTaskQue.pstrTcb = pstrTcb;
	pstrTcb->strTcbQue.pstrTcb = pstrTcb;
	pstrTcb->strSemQue.pstrTcb = pstrTcb;

	pstrTcb->pucTaskName = pucTaskName;

	pstrTcb->ucTaskPrio = ucTaskPrio;

	if (NULL == pstrTaskOpt) {
		pstrTcb->strTaskOpt.ucTaskSta = TASKREADY;
	} else {
		pstrTcb->strTaskOpt.ucTaskSta = pstrTaskOpt->ucTaskSta;
		pstrTcb->strTaskOpt.uiDelayTick = pstrTaskOpt->uiDelayTick;
	}

	if (TASKREADY == (pstrTcb->strTaskOpt.ucTaskSta & TASKREADY)) {

		pstrList = &gstrReadyTab.astrList[ucTaskPrio];
		pstrNode = &pstrTcb->strTcbQue.strQueHead;
		pstrPrioFlag = &gstrReadyTab.strFlag;

		REMIX_TaskAddToReadyTable(pstrList, pstrNode, pstrPrioFlag, ucTaskPrio);
	}

	if (TASKDELAY == (pstrTcb->strTaskOpt.ucTaskSta & TASKDELAY)) {
		if (DELAYWAITFOREVER != pstrTaskOpt->uiDelayTick) {
			pstrTcb->uiStillTick = guiTick + pstrTaskOpt->uiDelayTick;
			pstrNode = &pstrTcb->strTcbQue.strQueHead;

			REMIX_TaskAddToDelayTable(pstrNode);

			pstrTcb->uiTaskFlag |= DELAYQUEFLAG;
		}
	}
#ifdef  REMIX_SEMGROUPFLAG

	pstrTcb->strTaskNodeFlag.pRemixFlag = (REMIX_FLAG *) NULL;
	pstrTcb->strTaskNodeFlag.uiFlagWantBit = (U32) 0;
	pstrTcb->strTaskNodeFlag.uiFlagNodeOpt = REMIXFLAGWAITCLRAND;

#endif

#ifdef REMIX_KERNEL_CRITICAL_ALL
	(void) REMIX_TaskUnlock(DISABLE_ALL_INTERRUPT);
#else
	(void) REMIX_TaskUnlock(DISABLE_SELECT_INTERRUPT);
#endif

	return pstrTcb;

}

//*************************************************************************//


//*************************************************************************//
U32 REMIX_TaskDelete(REMIX_TCB * pstrTcb)
{
	REMIX_DLIST *pstrList;
	REMIX_DLIST *pstrNode;
	REMIX_PRIOFLAG *pstrPrioFlag;
	PRIORITYBITS ucTaskPrio;
	U8 ucTaskSta;

	if (NULL == pstrTcb) {
		return RTN_FAIL;
	}

	if (gpstrIdleTaskTcb == pstrTcb) {
		return RTN_FAIL;
	}
#ifdef REMIX_KERNEL_CRITICAL_ALL
	(void) REMIX_TaskLock(DISABLE_ALL_INTERRUPT);
#else
	(void) REMIX_TaskLock(DISABLE_SELECT_INTERRUPT);
#endif

#ifdef REMIX_INCLUDETASKHOOK

	if ((VFHDLT) NULL != gvfTaskDeleteHook) {
		gvfTaskDeleteHook(pstrTcb);
	}
#endif

	ucTaskSta = pstrTcb->strTaskOpt.ucTaskSta;

	if (TASKREADY == (TASKREADY & ucTaskSta)) {
		ucTaskPrio = pstrTcb->ucTaskPrio;
		pstrList = &gstrReadyTab.astrList[ucTaskPrio];
		pstrPrioFlag = &gstrReadyTab.strFlag;

		REMIX_TaskDeleteFromReadyTable(pstrList, pstrPrioFlag, ucTaskPrio);
	}

	if (TASKDELAY == (TASKDELAY & ucTaskSta)) {
		pstrNode = &pstrTcb->strTcbQue.strQueHead;

		REMIX_TaskDeleteFromDelayTable(pstrNode);
	}

	if (TASKPEND == (TASKPEND & ucTaskSta)) {
		REMIX_TaskDeleteFromSemTable(pstrTcb);
	}

	REMIX_TaskDeleteFromTaskList(&pstrTcb->strTaskQue.strQueHead);

	if (gpstrCurTcb == pstrTcb) {
		gpstrCurTcb = NULL;

#ifdef REMIX_TASKROUNDROBIN
		gauiSliceCnt[gpstrCurTcb->ucTaskPrio] = 0;
#endif
	}

	if (TASKSTACKFLAG == (pstrTcb->uiTaskFlag & TASKSTACKFLAG)) {
		free(pstrTcb->pucTaskStack);
	}
#ifdef REMIX_KERNEL_CRITICAL_ALL
	(void) REMIX_TaskUnlock(DISABLE_ALL_INTERRUPT);
#else
	(void) REMIX_TaskUnlock(DISABLE_SELECT_INTERRUPT);
#endif

	REMIX_TaskSwiSched();

	return RTN_SUCD;

}

void REMIX_TaskSelfDelete(void)
{
	(void) REMIX_TaskDelete(gpstrCurTcb);
}

//*************************************************************************//

//*************************************************************************//
U32 REMIX_TaskDelay(U32 uiDelayTick)
{
	REMIX_DLIST *pstrList;
	REMIX_DLIST *pstrNode;
	REMIX_PRIOFLAG *pstrPrioFlag;
	PRIORITYBITS ucTaskPrio;


	if (DELAYNOWAIT != uiDelayTick) {
		if (gpstrCurTcb == gpstrIdleTaskTcb) {
			return RTN_FAIL;
		}

		ucTaskPrio = gpstrCurTcb->ucTaskPrio;
		pstrList = &gstrReadyTab.astrList[ucTaskPrio];
		pstrPrioFlag = &gstrReadyTab.strFlag;

#ifdef REMIX_KERNEL_CRITICAL_ALL
		(void) REMIX_TaskLock(DISABLE_ALL_INTERRUPT);
#else
		(void) REMIX_TaskLock(DISABLE_SELECT_INTERRUPT);
#endif

		pstrNode = REMIX_TaskDeleteFromReadyTable(pstrList, pstrPrioFlag, ucTaskPrio);
		gpstrCurTcb->strTaskOpt.ucTaskSta &= ~((U8) TASKREADY);

		gpstrCurTcb->strTaskOpt.uiDelayTick = uiDelayTick;

		if (DELAYWAITFOREVER != uiDelayTick) {
			gpstrCurTcb->uiStillTick = guiTick + uiDelayTick;
			REMIX_TaskAddToDelayTable(pstrNode);
			gpstrCurTcb->uiTaskFlag |= DELAYQUEFLAG;
		}

		gpstrCurTcb->strTaskOpt.ucTaskSta |= TASKDELAY;

#ifdef REMIX_TASKROUNDROBIN
		gauiSliceCnt[gpstrCurTcb->ucTaskPrio] = 0;
#endif

#ifdef REMIX_KERNEL_CRITICAL_ALL
		(void) REMIX_TaskUnlock(DISABLE_ALL_INTERRUPT);
#else
		(void) REMIX_TaskUnlock(DISABLE_SELECT_INTERRUPT);
#endif
	} else {
		gpstrCurTcb->strTaskOpt.uiDelayTick = RTN_SUCD;
	}

	REMIX_TaskSwiSched();

	return gpstrCurTcb->strTaskOpt.uiDelayTick;
}


U32 REMIX_TaskWake(REMIX_TCB * pstrTcb)
{
	REMIX_DLIST *pstrList;
	REMIX_DLIST *pstrNode;
	REMIX_PRIOFLAG *pstrPrioFlag;
	PRIORITYBITS ucTaskPrio;

	if (NULL == pstrTcb) {
		return RTN_FAIL;
	}
#ifdef REMIX_KERNEL_CRITICAL_ALL
	(void) REMIX_TaskLock(DISABLE_ALL_INTERRUPT);
#else
	(void) REMIX_TaskLock(DISABLE_SELECT_INTERRUPT);
#endif

	if (TASKDELAY != (TASKDELAY & pstrTcb->strTaskOpt.ucTaskSta)) {
#ifdef REMIX_KERNEL_CRITICAL_ALL
		(void) REMIX_TaskUnlock(DISABLE_ALL_INTERRUPT);
#else
		(void) REMIX_TaskUnlock(DISABLE_SELECT_INTERRUPT);
#endif
		return RTN_FAIL;
	}

	pstrNode = &pstrTcb->strTcbQue.strQueHead;

	if (DELAYWAITFOREVER != pstrTcb->strTaskOpt.uiDelayTick) {
		(void) REMIX_TaskDeleteFromDelayTable(pstrNode);
		pstrTcb->uiTaskFlag &= (~((U32) DELAYQUEFLAG));
	}

	pstrTcb->strTaskOpt.ucTaskSta &= ~((U8) TASKDELAY);
	pstrTcb->strTaskOpt.uiDelayTick = RTN_TASKDELATBREAK;

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

//*************************************************************************//

//*************************************************************************//
U32 REMIX_TaskPend(REMIX_SEM * pstrSem, U32 uiDelayTick)
{
	REMIX_DLIST *pstrList;
	REMIX_DLIST *pstrNode;
	REMIX_PRIOFLAG *pstrPrioFlag;
	PRIORITYBITS ucTaskPrio;

	if (gpstrCurTcb == gpstrIdleTaskTcb) {
		return RTN_FAIL;
	}

	ucTaskPrio = gpstrCurTcb->ucTaskPrio;
	pstrList = &gstrReadyTab.astrList[ucTaskPrio];
	pstrPrioFlag = &gstrReadyTab.strFlag;

	pstrNode = REMIX_TaskDeleteFromReadyTable(pstrList, pstrPrioFlag, ucTaskPrio);
	gpstrCurTcb->strTaskOpt.ucTaskSta &= ~((U8) TASKREADY);

	gpstrCurTcb->strTaskOpt.uiDelayTick = uiDelayTick;

	if (SEMWAITFOREVER != uiDelayTick) {
		gpstrCurTcb->uiStillTick = guiTick + uiDelayTick;

		REMIX_TaskAddToDelayTable(pstrNode);
		gpstrCurTcb->uiTaskFlag |= DELAYQUEFLAG;
	}

	REMIX_TaskAddToSemTable(gpstrCurTcb, pstrSem);
	gpstrCurTcb->uiTaskFlag &= (~((U32) TASKSEMGROUPFLAG));
	gpstrCurTcb->strTaskOpt.ucTaskSta |= TASKPEND;

#ifdef REMIX_TASKROUNDROBIN

	gauiSliceCnt[gpstrCurTcb->ucTaskPrio] = 0;

#endif

	return RTN_SUCD;


}

#ifdef REMIX_SEMGROUPFLAG

U32 REMIX_FlagBlock(REMIX_FLAG * pstrFlag, U32 uiFlagWantBit, U32 uiFlagNodeOpt, U32 uiDelayTick)
{
	REMIX_DLIST *pstrList;
	REMIX_DLIST *pstrNode;
	REMIX_PRIOFLAG *pstrPrioFlag;
	PRIORITYBITS ucTaskPrio;

	if (gpstrCurTcb == gpstrIdleTaskTcb) {
		return RTN_FAIL;
	}

	ucTaskPrio = gpstrCurTcb->ucTaskPrio;
	pstrList = &gstrReadyTab.astrList[ucTaskPrio];
	pstrPrioFlag = &gstrReadyTab.strFlag;

	pstrNode = REMIX_TaskDeleteFromReadyTable(pstrList, pstrPrioFlag, ucTaskPrio);
	gpstrCurTcb->strTaskOpt.ucTaskSta &= ~((U8) TASKREADY);

	gpstrCurTcb->strTaskOpt.uiDelayTick = uiDelayTick;

	if (SEMWAITFOREVER != uiDelayTick) {
		gpstrCurTcb->uiStillTick = guiTick + uiDelayTick;

		REMIX_TaskAddToDelayTable(pstrNode);
		gpstrCurTcb->uiTaskFlag |= DELAYQUEFLAG;
	}

	gpstrCurTcb->strTaskNodeFlag.pRemixFlag = pstrFlag;
	gpstrCurTcb->strTaskNodeFlag.uiFlagWantBit = uiFlagWantBit;
	gpstrCurTcb->strTaskNodeFlag.uiFlagNodeOpt = uiFlagNodeOpt;

	REMIX_TaskAddToFlagTable(gpstrCurTcb, pstrFlag);
	gpstrCurTcb->uiTaskFlag |= TASKSEMGROUPFLAG;
	gpstrCurTcb->strTaskOpt.ucTaskSta |= TASKPEND;

#ifdef REMIX_TASKROUNDROBIN

	gauiSliceCnt[gpstrCurTcb->ucTaskPrio] = 0;

#endif

	return RTN_SUCD;

}

#endif

//*************************************************************************//

REMIX_DLIST *REMIX_GetTaskLinkRoot(void)
{
	return &gstrTaskList;
}

//*************************************************************************//
#ifdef REMIX_TASKPRIOINHER

void REMIX_TaskPrioInheritance(REMIX_TCB * pstrTcb, PRIORITYBITS ucTaskPrio)
{
	REMIX_DLIST *pstrList;
	REMIX_DLIST *pstrNode;
	REMIX_PRIOFLAG *pstrPrioFlag;
	PRIORITYBITS ucTaskPrioTemp;

	if (NULL == pstrTcb) {
		return;
	}

	if (pstrTcb->ucTaskPrio <= ucTaskPrio) {
		return;
	}

	if (TASKREADY == (TASKREADY & pstrTcb->strTaskOpt.ucTaskSta)) {
		ucTaskPrioTemp = pstrTcb->ucTaskPrio;
		pstrList = &gstrReadyTab.astrList[ucTaskPrioTemp];
		pstrPrioFlag = &gstrReadyTab.strFlag;
		pstrNode = REMIX_TaskDeleteFromReadyTable(pstrList, pstrPrioFlag, ucTaskPrioTemp);
	}

	if (TASKPRIINHFLAG != (pstrTcb->uiTaskFlag & TASKPRIINHFLAG)) {
		pstrTcb->ucTaskPrioBackup = pstrTcb->ucTaskPrio;
	}

	pstrTcb->ucTaskPrio = ucTaskPrio;

	pstrTcb->uiTaskFlag |= TASKPRIINHFLAG;

	if (TASKREADY == (TASKREADY & pstrTcb->strTaskOpt.ucTaskSta)) {
		pstrList = &gstrReadyTab.astrList[ucTaskPrio];
		REMIX_TaskAddToReadyTable(pstrList, pstrNode, pstrPrioFlag, ucTaskPrio);
	}
}

void REMIX_TaskPrioResume(REMIX_TCB * pstrTcb)
{
	REMIX_DLIST *pstrList;
	REMIX_DLIST *pstrNode;
	REMIX_PRIOFLAG *pstrPrioFlag;
	U8 ucTaskPrioTemp;

	if (NULL == pstrTcb) {
		return;
	}

	if (TASKPRIINHFLAG != (pstrTcb->uiTaskFlag & TASKPRIINHFLAG)) {
		return;
	}

	ucTaskPrioTemp = pstrTcb->ucTaskPrio;
	pstrList = &gstrReadyTab.astrList[ucTaskPrioTemp];
	pstrPrioFlag = &gstrReadyTab.strFlag;
	pstrNode = REMIX_TaskDeleteFromReadyTable(pstrList, pstrPrioFlag, ucTaskPrioTemp);

	pstrTcb->ucTaskPrio = pstrTcb->ucTaskPrioBackup;
	pstrTcb->uiTaskFlag &= (~((U32) TASKPRIINHFLAG));

	pstrList = &gstrReadyTab.astrList[pstrTcb->ucTaskPrio];
	REMIX_TaskAddToReadyTable(pstrList, pstrNode, pstrPrioFlag, pstrTcb->ucTaskPrio);
}

#endif
//*************************************************************************//

//*************************************************************************//
#ifdef REMIX_TASKROUNDROBIN

void REMIX_TaskTimeSlice(U32 uiTimeSlice, U32 TaskTimeSliceOpt)
{
	U32 i;

#ifdef REMIX_KERNEL_CRITICAL_ALL
	(void) REMIX_TaskLock(DISABLE_ALL_INTERRUPT);
#else
	(void) REMIX_TaskLock(DISABLE_SELECT_INTERRUPT);
#endif

	if (((TaskTimeSliceOpt < USERHIGHESTPRIORITY)
	     || (TaskTimeSliceOpt > USERLOWESTPRIORITY))
	    && (TASKTIMESLICEALLPRIO != TaskTimeSliceOpt)) {

#ifdef REMIX_KERNEL_CRITICAL_ALL
		(void) REMIX_TaskUnlock(DISABLE_ALL_INTERRUPT);
#else
		(void) REMIX_TaskUnlock(DISABLE_SELECT_INTERRUPT);
#endif

		return;

	}

	if (TASKTIMESLICEALLPRIO == TaskTimeSliceOpt) {
		for (i = 0; i < PRIORITYNUM; i++) {
			guiTimeSlice[i] = uiTimeSlice;
		}
	} else {
		guiTimeSlice[TaskTimeSliceOpt] = uiTimeSlice;
	}

	for (i = 0; i < PRIORITYNUM; i++) {
		gauiSliceCnt[i] = 0;
	}

#ifdef REMIX_KERNEL_CRITICAL_ALL
	(void) REMIX_TaskUnlock(DISABLE_ALL_INTERRUPT);
#else
	(void) REMIX_TaskUnlock(DISABLE_SELECT_INTERRUPT);
#endif
}

#endif
//*************************************************************************//

//*************************************************************************//
#ifdef REMIX_INCLUDETASKHOOK

//*************************************************************************//
void REMIX_TaskHookInit(void)
{
	gvfTaskCreateHook = (VFHCRT) NULL;
	gvfTaskSwitchHook = (VFHSWT) NULL;
	gvfTaskDeleteHook = (VFHDLT) NULL;
}

//*************************************************************************//
void REMIX_TaskCreateHookAdd(VFHCRT vfFuncPointer)
{
	gvfTaskCreateHook = vfFuncPointer;
}

void REMIX_TaskCreateHookDel(void)
{
	gvfTaskCreateHook = (VFHCRT) NULL;
}

//*************************************************************************//
void REMIX_TaskSwitchHookAdd(VFHSWT vfFuncPointer)
{
	gvfTaskSwitchHook = vfFuncPointer;
}

void REMIX_TaskSwitchHookDel(void)
{
	gvfTaskSwitchHook = (VFHSWT) NULL;
}

//*************************************************************************//
void REMIX_TaskDeleteHookAdd(VFHDLT vfFuncPointer)
{
	gvfTaskDeleteHook = vfFuncPointer;
}

void REMIX_TaskDeleteHookDel(void)
{
	gvfTaskDeleteHook = (VFHDLT) NULL;
}

//*************************************************************************//

#endif
