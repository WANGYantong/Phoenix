#ifndef __REMIX_TASK_PUBLIC_H__
#define __REMIX_TASK_PUBLIC_H__

typedef void    (*VFHCRT)(REMIX_TCB*);
typedef void    (*VFHSWT)(REMIX_TCB*, REMIX_TCB*);
typedef void    (*VFHDLT)(REMIX_TCB*);


#define USERHIGHESTPRIORITY             (HIGHESTPRIORITY + 1)
#define USERLOWESTPRIORITY              (LOWESTPRIORITY - 1)

#define TASKRUNNING                     0x01
#define TASKREADY                       0x02
#define TASKDELAY                       0x04
#define TASKPEND                        0x08
#define TASKSUSPEND                     0x10

#ifdef REMIX_TASKROUNDROBIN
#define TASKTIMESLICEALLPRIO            0xFFFFFFFF
#endif

#ifndef RTN_SUCD
    #define RTN_SUCD                    0
#endif

#ifndef RTN_FAIL
    #define RTN_FAIL                    1
#endif

#define RTN_TASKDELAYTIMEOUT            2
#define RTN_TASKDELATBREAK              3

#define DELAYNOWAIT                     0
#define DELAYWAITFOREVER                0xFFFFFFFF

extern REMIX_TCB* REMIX_TaskCreate(U8 * pucTaskName, VFUNCTION vfFucPointer, void * pvPara, U8 * pucTaskStack, U32 uiStackSize, PRIORITYBITS ucTaskPrio, REMIX_TASKOPT * pstrTaskOpt);
extern U32 REMIX_TaskDelete(REMIX_TCB * pstrTcb);
extern void REMIX_TaskSelfDelete(void);
extern U32 REMIX_TaskDelay(U32 uiDelayTick);
extern U32 REMIX_TaskWake(REMIX_TCB * pstrTcb);
extern U32 REMIX_TaskSuspend(REMIX_TCB * pstrTcb);
extern U32 REMIX_TaskResume(REMIX_TCB * pstrTcb);
extern REMIX_DLIST* REMIX_GetTaskLinkRoot(void);

#ifdef REMIX_TASKROUNDROBIN
extern void REMIX_TaskTimeSlice(U32 uiTimeSlice, U32 TaskTimeSliceOpt);
#endif

#ifdef REMIX_INCLUDETASKHOOK
extern void REMIX_TaskCreateHookAdd(VFHCRT vfFuncPointer);
extern void REMIX_TaskCreateHookDel(void);
extern void REMIX_TaskSwitchHookAdd(VFHSWT vfFuncPointer);
extern void REMIX_TaskSwitchHookDel(void);
extern void REMIX_TaskDeleteHookAdd(VFHDLT vfFuncPointer);
extern void REMIX_TaskDeleteHookDel(void);
#endif


#endif
