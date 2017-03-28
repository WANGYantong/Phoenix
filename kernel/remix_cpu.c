#include "remix_private.h"

U32 guiIntLockCounter;

void REMIX_TaskStackInit(REMIX_TCB * pstrTcb, VFUNCTION vfFuncPointer, void *pvPara)
{
	STACKREG *pstrRegSp;
	U32 *puiStack;

	pstrRegSp = &pstrTcb->strStackReg;

	pstrRegSp->uiR0 = (U32) pvPara;
	pstrRegSp->uiR1 = 0;
	pstrRegSp->uiR2 = 0;
	pstrRegSp->uiR3 = 0;
	pstrRegSp->uiR4 = 0;
	pstrRegSp->uiR5 = 0;
	pstrRegSp->uiR6 = 0;
	pstrRegSp->uiR7 = 0;
	pstrRegSp->uiR8 = 0;
	pstrRegSp->uiR9 = 0;
	pstrRegSp->uiR10 = 0;
	pstrRegSp->uiR11 = 0;
	pstrRegSp->uiR12 = 0;
	pstrRegSp->uiR13 = (U32) pstrTcb - 32;
	pstrRegSp->uiR14 = (U32) REMIX_TaskSelfDelete;
	pstrRegSp->uiR15 = (U32) vfFuncPointer;
	pstrRegSp->uiXpsr = MODE_USR;
	pstrRegSp->uiExc_Rtn = RTN_THREAD_MSP;

#ifdef REMIX_DEBUGSTACKCHECK

	REMIX_TaskStackCheckInit(pstrTcb);

#endif

	puiStack = (U32 *) pstrTcb;
	*(--puiStack) = pstrRegSp->uiXpsr;
	*(--puiStack) = pstrRegSp->uiR15;
	*(--puiStack) = pstrRegSp->uiR14;
	*(--puiStack) = pstrRegSp->uiR12;
	*(--puiStack) = pstrRegSp->uiR3;
	*(--puiStack) = pstrRegSp->uiR2;
	*(--puiStack) = pstrRegSp->uiR1;
	*(--puiStack) = pstrRegSp->uiR0;
}

void REMIX_SystemHardwareInit(void)
{
	REMIX_TickTimerInit();

	REMIX_PendSvIsrInit();
}


void REMIX_TickTimerInit(void)
{
	(void) SysTick_Config(SystemCoreClock / (1000 / TICK));

	NVIC_SetPriority(SysTick_IRQn, 6 << 1);
}

void REMIX_PendSvIsrInit(void)
{
	NVIC_SetPriority(PendSV_IRQn, 7 << 1);
}

void REMIX_SetChipWorkMode(U32 uiMode)
{
	U32 uiCtrl;

	uiCtrl = __get_CONTROL();

	uiCtrl &= 0xFFFFFFFE;
	uiCtrl |= uiMode;

	__set_CONTROL(uiCtrl);
}


void REMIX_TaskSwiSched(void)
{
	if (RTN_SUCD != REMIX_RunInInt()) {
		REMIX_TaskOccurSwi(SWI_TASKSCHED);
	} else {
		REMIX_IntPendSvSet();
	}
}

void REMIX_IntPendSvSet(void)
{
#define HWREG(x)                (*((volatile unsigned long *)(x)))
#define NVIC_INT_CTRL           0xE000ED04
#define NVIC_INT_CTRL_PEND_SV   0x10000000

	HWREG(NVIC_INT_CTRL) |= NVIC_INT_CTRL_PEND_SV;
}

U8 REMIX_CalcPrioFromPrioFlag(U8 ucPrioFlag)
{
	U8 ucPrioTemp;

#ifdef  REMIX_RBITANDCLZ
	ucPrioTemp = (U8) REMIX_CalcPrioTemp((U32) ucPrioFlag);
	if (0x20 == ucPrioTemp)
		return 0;
	else
		return ucPrioTemp;
#endif

#ifdef REMIX_XORANDSWITCH
	ucPrioTemp = ucPrioFlag ^ (ucPrioFlag - 1);
	switch (ucPrioTemp) {
	case 0x01:
		return 0;
	case 0x03:
		return 1;
	case 0x07:
		return 2;
	case 0x0F:
		return 3;
	case 0x1F:
		return 4;
	case 0x3F:
		return 5;
	case 0x7F:
		return 6;
	case 0xFF:
		return 7;
	default:
		return 0;
	}
#endif

}

U32 REMIX_RunInInt(void)
{
	if (0 != (REMIX_GetXpsr() & XPSR_EXTMASK)) {
		return RTN_SUCD;
	} else {
		return RTN_FAIL;
	}
}

U32 REMIX_InterruptLock(void)
{
#ifdef REMIX_KERNEL_CRITICAL_ALL
    return REMIX_IntLock(DISABLE_ALL_INTERRUPT);
#else
    return REMIX_IntLock(DISABLE_SELECT_INTERRUPT+REMIX_MAX_SYSCALL_INTERRUPT_PRIORITY);
#endif
}

U32 REMIX_InterruptUnlock(void)
{
#ifdef REMIX_KERNEL_CRITICAL_ALL
    return REMIX_IntUnlock(DISABLE_ALL_INTERRUPT);
#else
    return REMIX_IntUnlock(DISABLE_SELECT_INTERRUPT);
#endif
}

U32 REMIX_IntLock(U8 ucOpt)
{
	if (RTN_SUCD == REMIX_RunInInt()) {
		return RTN_FAIL;
	}

	if ((DISABLE_ALL_INTERRUPT != (ucOpt&DISABLE_INTERRUPT_OPT_MASK))
	    && (DISABLE_SELECT_INTERRUPT != (ucOpt&DISABLE_INTERRUPT_OPT_MASK))) {
		return RTN_FAIL;
	}

	if (0 == guiIntLockCounter) {

		if (DISABLE_ALL_INTERRUPT == (ucOpt&DISABLE_INTERRUPT_OPT_MASK)) {
			__disable_irq();
		} else {
			(void) REMIX_SetBasepri(ucOpt&DISABLE_INTERRUPT_PRIO_MASK);
		}

		guiIntLockCounter++;

		return RTN_SUCD;
	} else if (guiIntLockCounter < 0xFFFFFFFF) {
		guiIntLockCounter++;

		return RTN_SUCD;
	} else {
		return RTN_FAIL;
	}
}

U32 REMIX_IntUnlock(U8 ucOpt)
{
	if (RTN_SUCD == REMIX_RunInInt()) {
		return RTN_FAIL;
	}

	if ((DISABLE_ALL_INTERRUPT != (ucOpt&DISABLE_INTERRUPT_OPT_MASK))
	    && (DISABLE_SELECT_INTERRUPT != (ucOpt&DISABLE_INTERRUPT_OPT_MASK))) {
		return RTN_FAIL;
	}

	if (guiIntLockCounter > 1) {
		guiIntLockCounter--;

		return RTN_SUCD;
	} else if (1 == guiIntLockCounter) {
		guiIntLockCounter--;

		if (DISABLE_ALL_INTERRUPT == (ucOpt&DISABLE_INTERRUPT_OPT_MASK)) {
			__enable_irq();
		} else {
			(void) REMIX_SetBasepri(0);
		}

		return RTN_SUCD;
	} else {
		return RTN_FAIL;
	}
}
