#ifndef __REMIX_DEBUG_PRIVATE_H__
#define __REMIX_DEBUG_PRIVATE_H__

#ifdef REMIX_DEBUGSTACKCHECK
    #define STACKCHECKVALUE             0xCCCCCCCC
#endif

#ifdef REMIX_CPUSTATISTIC

#define CPUSHARETIME        (1000 / TICK)
#define SYSTICKPERIOD       0x1000000

#endif

#ifdef REMIX_CPUSTATISTIC
extern U32 guiCpuSharePeriod;
#endif

#ifdef REMIX_DEBUGSTACKCHECK
extern void REMIX_TaskStackCheckInit(REMIX_TCB * pstrTcb);
#endif

#ifdef REMIX_CPUSTATISTIC
extern void REMIX_TaskCPUShareInit(REMIX_TCB * pstrTcb);
extern void REMIX_CPUShareStatistic(REMIX_TCB * pstrOldTcb, REMIX_TCB * pstrNewTcb);
#endif

#endif

