#ifndef __REMIX_DEBUG_PUBLIC_H__
#define __REMIX_DEBUG_PUBLIC_H__


#ifdef REMIX_DEBUGCONTEXT
extern void REMIX_SendCharFuncInit(void(* vfFuncPointer)(U8));
#endif

#ifdef REMIX_DEBUGSTACKCHECK
extern U32 REMIX_TaskStackCheck(REMIX_TCB * pstrTcb);
#endif

#ifdef REMIX_CPUSTATISTIC
extern U32 REMIX_GetCPUShare(REMIX_TCB * pstrTcb);
#endif


#endif
