#ifndef __REMIX_CORE_PUBLIC_H__
#define __REMIX_CORE_PUBLIC_H__

extern void REMIX_TaskTick(void);
extern U32 REMIX_GetSystemTick(void);
extern REMIX_TCB* REMIX_GetCurrentTcb(void);
extern REMIX_TCB* REMIX_GetRootTcb(void);
extern REMIX_TCB* REMIX_GetIdleTcb(void);
extern U8* REMIX_GetREMIXVersion(void);

#endif

