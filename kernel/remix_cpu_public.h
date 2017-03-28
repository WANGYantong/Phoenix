#ifndef __REMIX_CPU_PUBLIC_H__
#define __REMIX_CPU_PUBLIC_H__

#define SWI_TASKSCHED           0x10001001

#define DISABLE_ALL_INTERRUPT      0x0
#define DISABLE_SELECT_INTERRUPT   0x1

extern void REMIX_IntPendSvSet(void);
extern U32 REMIX_RunInInt(void);
extern U32 REMIX_TaskLock(U8 ucOpt);
extern U32 REMIX_TaskUnlock(U8 ucOpt);

#endif

