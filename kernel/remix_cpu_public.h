#ifndef __REMIX_CPU_PUBLIC_H__
#define __REMIX_CPU_PUBLIC_H__

#define SWI_TASKSCHED           0x10001001

#define DISABLE_ALL_INTERRUPT           0x1
#define DISABLE_SELECT_INTERRUPT        0x2

#define DISABLE_INTERRUPT_OPT_MASK      0x3
#define DISABLE_INTERRUPT_PRIO_MASK     0xFC


extern void REMIX_IntPendSvSet(void);
extern U32 REMIX_RunInInt(void);
extern U32 REMIX_InterruptLock(void);
extern U32 REMIX_InterruptUnlock(void);


#endif

