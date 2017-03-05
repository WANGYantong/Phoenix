#ifndef __REMIX_CPU_PRIVATE_H__
#define __REMIX_CPU_PRIVATE_H__

#define RTN_HANDLER         0xFFFFFFF1
#define RTN_THREAD_MSP      0xFFFFFFF9
#define RTN_THREAD_PSP      0xFFFFFFFD

#define XPSR_EXTMASK        0x1FF

#define MSP_STACK               0
#define PSP_STACK               1

#define PRIVILEGED              0
#define UNPRIVILEGED            1

extern U32 guiIntLockCounter;

extern void REMIX_TaskStackInit(REMIX_TCB * pstrTcb, VFUNCTION vfFuncPointer, void * pvPara);
extern void REMIX_SystemHardwareInit(void);
extern void REMIX_TickTimerInit(void);
extern void REMIX_PendSvIsrInit(void);
extern void REMIX_SetChipWorkMode(U32 uiMode);
extern void REMIX_TaskSwiSched(void);
extern void REMIX_TaskOccurSwi(U32 uiSwiNo);


#endif

