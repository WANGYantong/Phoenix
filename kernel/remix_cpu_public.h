#ifndef __REMIX_CPU_H__
#define __REMIX_CPU_H__

#define SWI_TASKSCHED           0x10001001

extern void REMIX_IntPendSvSet(void);
extern U32 REMIX_RunInInt(void);
extern U32 REMIX_IntLock(void);
extern U32 REMIX_IntUnlock(void);

#endif

