#ifndef __REMIX_DEBUG_PRIVATE_H__
#define __REMIX_DEBUG_PRIVATE_H__

#ifdef REMIX_DEBUGSTACKCHECK
    #define STACKCHECKVALUE             0xCCCCCCCC
#endif

#ifdef REMIX_DEBUGCOMTEXT

#define VALIDTASKNAMEMAXLEN         20

#define ROMVALIDADDRSTART           0x08000000
#define ROMVALIDADDRLEN             0x40000
#define ROMVALIDADDREND             (ROMVALIDADDRSTART + ROMVALIDADDRLEN - 1)

typedef struct remix_conthead
{
    U32 uiAbsAddr;
    U32 uiRelAddr;
    U32 uiLen;
    U32 uiRemLen;
    void (*vfSendChar)(U8);
}REMIX_CONTHEAD;

typedef struct remix_contmsg
{
    U32 uiLen;
    U32 uiPreAddr;
    U32 uiPreTcb;
    U32 uiNextTcb;
    STACKREG strReg;
}REMIX_CONTMSG;

#endif


#ifdef REMIX_CPUSTATISTIC

#define CPUSHARETIME        (1000 / TICK)
#define SYSTICKPERIOD       0x1000000

#endif



#ifdef REMIX_DEBUGCONTEXT
extern REMIX_CONTHEAD* gpstrContext;
#endif

#ifdef REMIX_CPUSTATISTIC
extern U32 guiCpuSharePeriod;
#endif




extern void REMIX_SaveTaskContext(void);
extern void REMIX_FaultIsrPrint(void);

#ifdef REMIX_DEBUGCONTEXT
extern void REMIX_TaskContextInit(void);
extern void REMIX_PrintTaskContext(void);
extern void REMIX_CoverFirstContext(void);
extern void REMIX_SaveLastContext(U32 uiStackLen);
extern U32 REMIX_SaveOneContextData(U32 uiAddr, U32 uiData);
extern U32 REMIX_GetOneContextData(U32 uiAddr, U32 * puiData);
extern void REMIX_PrintContext(void);
extern void REMIX_SendChar(U8 ucChar);
extern void REMIX_SendString(U8 * pvStringPt, ...);
extern void REMIX_IfValidString(U8 * * pucString);
#endif

#ifdef REMIX_DEBUGSTACKCHECK
void REMIX_TaskStackCheckInit(REMIX_TCB * pstrTcb);
#endif

#ifdef REMIX_CPUSTATISTIC
extern void REMIX_TaskCpuShareInit(REMIX_TCB * pstrTcb);
extern void REMIX_CpuShareStatistic(REMIX_TCB * pstrOldTcb, REMIX_TCB * pstrNewTcb);
#endif



#endif
