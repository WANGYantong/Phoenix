
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "remix_private.h"

#ifdef REMIX_CPUSTATISTIC
U32 guiCpuSharePeriod;
#endif

//*************************************************************************//
#ifdef REMIX_DEBUGSTACKCHECK

void REMIX_TaskStackCheckInit(REMIX_TCB * pstrTcb)
{
	U32 uiStackBottom;
	U32 i;

	uiStackBottom = ((U32) pstrTcb->pucTaskStack + 3) & ALIGN4MASK;

	for (i = 0; i < (U32) pstrTcb - uiStackBottom; i += 4) {
		ADDRVAL(uiStackBottom + i) = STACKCHECKVALUE;
	}
}

U32 REMIX_TaskStackCheck(REMIX_TCB * pstrTcb)
{
	U32 uiStackBottom;
	U32 i;

	uiStackBottom = ((U32) pstrTcb->pucTaskStack + 3) & ALIGN4MASK;

	for (i = 0; i < (U32) pstrTcb - uiStackBottom; i += 4) {
		if (STACKCHECKVALUE != ADDRVAL(uiStackBottom + i)) {
			break;
		}
	}

	return i;
}

#endif
//*************************************************************************//


//*************************************************************************//
#ifdef REMIX_CPUSTATISTIC

void REMIX_TaskCPUShareInit(REMIX_TCB * pstrTcb)
{
	pstrTcb->strCpuShare.uiSysTickVal = 0;
	pstrTcb->strCpuShare.uiCounter = 0;
	pstrTcb->strCpuShare.uiCpuShare = 0;
}

void REMIX_CPUShareStatistic(REMIX_TCB * pstrOldTcb, REMIX_TCB * pstrNewTcb)
{
	static U32 suiTick = 0;
	REMIX_DLIST *pstrTaskList;
	REMIX_TCB *pstrTaskTcb;
	U32 uiSysTickVal;

	uiSysTickVal = SysTick->VAL;

	if (NULL != pstrOldTcb) {
		if (suiTick == guiTick) {
			pstrOldTcb->strCpuShare.uiCounter += (pstrOldTcb->strCpuShare.uiSysTickVal - uiSysTickVal);
		} else {
			pstrOldTcb->strCpuShare.uiCounter +=
			    (pstrOldTcb->strCpuShare.uiSysTickVal + SYSTICKPERIOD - uiSysTickVal);
		}
	}

	suiTick = guiTick;

	pstrNewTcb->strCpuShare.uiSysTickVal = uiSysTickVal;

	if (CPUSHARETIME == guiCpuSharePeriod) {
		pstrTaskList = &gstrTaskList;

		while (NULL != (pstrTaskList = REMIX_DlistNextNodeEmpInq(&gstrTaskList, pstrTaskList))) {
			pstrTaskTcb = ((REMIX_TCBQUE *) pstrTaskList)->pstrTcb;

			pstrTaskTcb->strCpuShare.uiCpuShare = (pstrTaskTcb->strCpuShare.uiCounter + SYSTICKPERIOD / 2)
			    / SYSTICKPERIOD * 100 / CPUSHARETIME;

			pstrTaskTcb->strCpuShare.uiCounter = 0;
		}

		guiCpuSharePeriod = 0;
	}
}

U32 REMIX_GetCPUShare(REMIX_TCB * pstrTcb)
{
	return pstrTcb->strCpuShare.uiCpuShare;
}

#endif

/***********************************************************************/

void REMIX_IdleHook(void)
{
#ifdef REMIX_IDLETASKHOOK

    __WFI();

#endif
}

