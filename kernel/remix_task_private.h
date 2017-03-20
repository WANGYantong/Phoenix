#ifndef __REMIX_TASK_PRIVATE_H__
#define __REMIX_TASK_PRIVATE_H__

#define HIGHESTPRIORITY             0
#define LOWESTPRIORITY              (PRIORITYNUM - 1)

#define DELAYQUEFLAG                0x00000001
#define TASKSTACKFLAG               0x00000002
#define TASKPRIINHFLAG              0x00000004
#define TASKSEMGROUPFLAG            0x00000008

extern REMIX_DLIST gstrTaskList;

#ifdef REMIX_TASKROUNDROBIN
extern U32 guiTimeSlice[PRIORITYNUM];
extern U32 gauiSliceCnt[PRIORITYNUM];
#endif

#ifdef REMIX_INCLUDETASKHOOK
extern VFHCRT gvfTaskCreateHook;
extern VFHSWT gvfTaskSwitchHook;
extern VFHDLT gvfTaskDeleteHook;
#endif

extern REMIX_TCB* REMIX_TaskTcbInit(U8 * pucTaskName, VFUNCTION vfFucPionter, void * pvPara, U8 * pucTaskStack, U32 uiStackSize, PRIORITYBITS ucTaskPrio, REMIX_TASKOPT * pstrTaskOpt);
extern U32 REMIX_TaskPend(REMIX_SEM * pstrSem, U32 uiDelayTick);

#ifdef REMIX_TASKPRIOINHER
extern void REMIX_TaskPrioInheritance(REMIX_TCB * pstrTcb, PRIORITYBITS ucTaskPrio);
extern void REMIX_TaskPrioResume(REMIX_TCB * pstrTcb);
#endif

#ifdef REMIX_INCLUDETASKHOOK
extern void REMIX_TaskHookInit(void);
#endif

#endif
