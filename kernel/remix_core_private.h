#ifndef __REMIX_CORE_PRIVATE_H__
#define __REMIX_CORE_PRIVATE_H__

#define SYSTEMSCHEDULE              0
#define SYSTEMNOTSCHEDULE           1

#define TICKSCHEDCLR                0
#define TICKSCHEDSET                1

#define USERROOT                    0
#define USERGUEST                   1

extern U32 guiSystemStatus;
extern REMIX_TASKSCHEDTAB gstrReadyTab;
extern REMIX_DLIST gstrDelayTab;
extern U32 guiTick;
extern U8 gucTickSched;
extern STACKREG* gpstrCurTaskReg;
extern STACKREG* gpstrNextTaskReg;
extern REMIX_TCB* gpstrCurTcb;
extern REMIX_TCB* gpstrRootTaskTcb;
extern REMIX_TCB* gpstrIdleTaskTcb;
extern U32 guiUser;

#ifdef REMIX_UNMAP
extern const U8 caucTaskPrioUnmapTab[256];
#endif

extern void REMIX_BeforeRootTask(void * pvPara);
extern void REMIX_TaskSched(void);
extern void REMIX_TaskListInit(void);
extern void REMIX_TaskAddToTaskList(REMIX_DLIST * pstrNode);
extern REMIX_DLIST* REMIX_TaskDeleteFromTaskList(REMIX_DLIST * pstrNode);
extern void REMIX_TaskReadyTableInit(void);
extern void REMIX_TaskAddToReadyTable(REMIX_DLIST * pstrList, REMIX_DLIST * pstrNode, REMIX_PRIOFLAG * pstrPrioFlag, PRIORITYBITS ucTaskPrio);
extern REMIX_DLIST* REMIX_TaskDeleteFromReadyTable(REMIX_DLIST * pstrList, REMIX_PRIOFLAG * pstrPrioFlag, PRIORITYBITS ucTaskPrio);
extern REMIX_TCB* REMIX_TaskReadyTableSched(void);
extern void REMIX_TaskDelayTableInit(void);
extern void REMIX_TaskAddToDelayTable(REMIX_DLIST* pstrNode);
extern REMIX_DLIST* REMIX_TaskDeleteFromDelayTable(REMIX_DLIST * pstrNode);
extern void REMIX_TaskDelayTableSched(void);
extern void REMIX_TaskSemTableInit(REMIX_TASKSCHEDTAB * pstrSchedTab);
extern void REMIX_TaskAddToSemTable(REMIX_TCB * pstrTcb, REMIX_SEM * pstrSem);
extern REMIX_DLIST* REMIX_TaskDeleteFromSemTable(REMIX_TCB * pstrTcb);
extern REMIX_TCB* REMIX_TaskSemTableSche(REMIX_SEM * pstrSem);
extern void REMIX_TaskFlagTableInit(REMIX_TASKSCHEDTAB * pstrSchedTab);
extern void REMIX_TaskAddToFlagTable(REMIX_TCB * pstrTcb, REMIX_FLAG * pstrFlag);
extern REMIX_DLIST* REMIX_TaskDeleteFromFlagTable(REMIX_TCB * pstrTcb);
extern REMIX_TCB* REMIX_TaskFlagTableSche(REMIX_FLAG * pstrFlag);
extern REMIX_TCB* REMIX_TaskFlagTableCheck(REMIX_FLAG * pstrFlag);
extern void REMIX_TaskSchedTableInit(REMIX_TASKSCHEDTAB * pstrSchedTab);
extern void REMIX_TaskAddToSchedTable(REMIX_DLIST * pstrList, REMIX_DLIST * pstrNode, REMIX_PRIOFLAG * pstrPrioFlag, PRIORITYBITS ucTaskPrio);
extern REMIX_DLIST* REMIX_TaskDeleteFromSchedTable(REMIX_DLIST * pstrList, REMIX_PRIOFLAG * pstrPrioFlag, PRIORITYBITS ucTaskPrio);
extern void REMIX_TaskSetPrioFlag(REMIX_PRIOFLAG * pstrPrioFlag, PRIORITYBITS ucTaskPrio);
extern void REMIX_TaskClrPrioFlag(REMIX_PRIOFLAG * pstrPrioFlag, PRIORITYBITS ucTaskPrio);
extern PRIORITYBITS REMIX_TaskGetHighestPrio(REMIX_PRIOFLAG * pstrPrioFlag);
extern void REMIX_IdleTask(void * pvPara);
extern void REMIX_SetUser(U32 uiUser);
extern U32 REMIX_GetUser(void);
extern void REMIX_SwitchToTask(void);
extern U32 REMIX_GetXpsr(void);


#endif

