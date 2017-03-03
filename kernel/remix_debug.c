
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "remix_private.h"


#ifdef REMIX_DEBUGCONTEXT
REMIX_CONTHEAD *gpstrContext;
#endif


#ifdef REMIX_CPUSTATISTIC
U32 guiCpuSharePeriod;
#endif

void REMIX_SaveTaskContext(void)
{
#ifdef REMIX_DEBUGCONTEXT

	U32 uiStack;
	U32 uiTcbAddr;
	U32 uiSaveLen;

	if (0 == gpstrContext->uiAbsAddr) {
		return;
	}

	if (gpstrCurTaskReg == gpstrNextTaskReg) {
		return;
	}

	uiStack = gpstrCurTaskReg->uiR13;

	uiTcbAddr = (U32) gpstrCurTaskReg;

	uiSaveLen = sizeof(REMIX_CONTMSG) + (uiTcbAddr - uiStack);

	if (uiSaveLen > gpstrContext->uiLen) {
		gpstrContext->uiAbsAddr = 0;

		return;
	}

	while (1) {
		if (uiSaveLen > gpstrContext->uiRemLen) {
			REMIX_CoverFirstContext();
		} else {
			break;
		}
	}

	REMIX_SaveLastContext(uiTcbAddr - uiStack);

#endif
}


void REMIX_FaultIsrPrint(void)
{
#ifdef REMIX_DEBUGCONTEXT

	gpstrCurTaskReg = &gpstrCurTcb->strStackReg;

	gpstrNextTaskReg = (STACKREG *) NULL;

	REMIX_SaveTaskContext();

	REMIX_PrintTaskContext();

#endif
}

//*************************************************************************//
#ifdef REMIX_DEBUGCONTEXT

void REMIX_TaskContextInit(void)
{
	REMIX_CONTMSG *pstrCurAddr;

	gpstrContext = (REMIX_CONTHEAD *) ((REMIX_CONTEXTADDR + 3) & ALIGN4MASK);

	gpstrContext->uiAbsAddr = (U32) gpstrContext + sizeof(REMIX_CONTHEAD);

	if (0 == gpstrContext->uiAbsAddr) {
		return;
	}

	gpstrContext->uiRelAddr = 0;

	gpstrContext->uiLen = ((REMIX_CONTEXTADDR + REMIX_CONTEXTLEN) & ALIGN4MASK)
	    - gpstrContext->uiAbsAddr;

	if (gpstrContext->uiLen < sizeof(REMIX_CONTMSG)) {
		gpstrContext->uiAbsAddr = 0;

		return;
	}

	if (gpstrContext->uiAbsAddr > 0xFFFFFFFF - gpstrContext->uiLen) {
		gpstrContext->uiAbsAddr = 0;

		return;
	}

	gpstrContext->uiRemLen = gpstrContext->uiLen;

	pstrCurAddr = (REMIX_CONTMSG *) (gpstrContext->uiAbsAddr + gpstrContext->uiRelAddr);
	pstrCurAddr->uiLen = 0;
}


void REMIX_PrintTaskContext(void)
{
	if (0 == gpstrContext->uiAbsAddr) {
		return;
	}

	REMIX_SendString("\r\n\r\n!!!!!EXCEPTION OCCUR!!!!!\n\r");

	while (gpstrContext->uiLen > gpstrContext->uiRemLen) {
		REMIX_PrintContext();
	}

	REMIX_SendString("\r\n\r\n!!!!!EXCEPTION PRINT OVER!!!!!\n\r");
}


void REMIX_CoverFirstContext(void)
{
	REMIX_CONTMSG *pstrLstMsg;
	REMIX_CONTMSG *pstrFstMsg;
	U32 uiIdleAddr;
	U32 uiFstAddr;

	pstrLstMsg = (REMIX_CONTMSG *) (gpstrContext->uiAbsAddr + gpstrContext->uiRelAddr);

	uiIdleAddr = gpstrContext->uiRelAddr + pstrLstMsg->uiLen;

	if (uiIdleAddr >= gpstrContext->uiLen) {
		uiIdleAddr -= gpstrContext->uiLen;

		uiFstAddr = uiIdleAddr + gpstrContext->uiRemLen;
		pstrFstMsg = (REMIX_CONTMSG *) (gpstrContext->uiAbsAddr + uiFstAddr);
	} else {
		uiFstAddr = uiIdleAddr + gpstrContext->uiRemLen;

		if (uiFstAddr >= gpstrContext->uiLen) {
			uiFstAddr -= gpstrContext->uiLen;
		}

		pstrFstMsg = (REMIX_CONTMSG *) (gpstrContext->uiAbsAddr + uiFstAddr);
	}

	gpstrContext->uiRemLen += pstrFstMsg->uiLen;
}


void REMIX_SaveLastContext(U32 uiStackLen)
{
	REMIX_CONTMSG *pstrLstMsg;
	U32 *puiDataAddr;
	U32 uiLstAddr;
	U32 uiDataAddr;
	U32 uiTcbAddr;
	U32 i;

	pstrLstMsg = (REMIX_CONTMSG *) (gpstrContext->uiAbsAddr + gpstrContext->uiRelAddr);

	uiLstAddr = gpstrContext->uiRelAddr + pstrLstMsg->uiLen;

	if (uiLstAddr >= gpstrContext->uiLen) {
		uiLstAddr -= gpstrContext->uiLen;
	}

	uiDataAddr = REMIX_SaveOneContextData(uiLstAddr, sizeof(REMIX_CONTMSG) + uiStackLen);

	uiDataAddr = REMIX_SaveOneContextData(uiDataAddr, gpstrContext->uiRelAddr);

	uiTcbAddr = (U32) gpstrCurTaskReg;
	uiDataAddr = REMIX_SaveOneContextData(uiDataAddr, uiTcbAddr);

	if (NULL != gpstrNextTaskReg) {
		uiTcbAddr = (U32) gpstrNextTaskReg;
	} else {
		uiTcbAddr = 0;
	}

	uiDataAddr = REMIX_SaveOneContextData(uiDataAddr, uiTcbAddr);

	puiDataAddr = &gpstrCurTaskReg->uiExc_Rtn;
	for (i = 0; i < (sizeof(STACKREG) / sizeof(U32)); i++) {
		uiDataAddr = REMIX_SaveOneContextData(uiDataAddr, *puiDataAddr--);
	}

	for (i = 0; i < (uiStackLen / sizeof(U32)); i++) {
		uiDataAddr = REMIX_SaveOneContextData(uiDataAddr, *puiDataAddr--);
	}

	gpstrContext->uiRelAddr = uiLstAddr;
	gpstrContext->uiRemLen -= (sizeof(REMIX_CONTMSG) + uiStackLen);
}


U32 REMIX_SaveOneContextData(U32 uiAddr, U32 uiData)
{
	ADDRVAL(gpstrContext->uiAbsAddr + uiAddr) = uiData;

	if (gpstrContext->uiLen != uiAddr + 4) {
		uiAddr += 4;
	} else {
		uiAddr = 0;
	}

	return uiAddr;
}


U32 REMIX_GetOneContextData(U32 uiAddr, U32 * puiData)
{
	*puiData = ADDRVAL(gpstrContext->uiAbsAddr + uiAddr);

	if (gpstrContext->uiLen != uiAddr + 4) {
		uiAddr += 4;
	} else {
		uiAddr = 0;
	}

	return uiAddr;
}


void REMIX_PrintContext(void)
{
	U8 *pucTaskName;
	U32 uiData;
	U32 uiLen;
	U32 uiDataAddr;
	U32 uiTcb;
	U32 i;

	uiDataAddr = gpstrContext->uiRelAddr;

	uiDataAddr = REMIX_GetOneContextData(uiDataAddr, &uiLen);

	uiDataAddr = REMIX_GetOneContextData(uiDataAddr, &uiData);
	gpstrContext->uiRelAddr = uiData;

	uiDataAddr = REMIX_GetOneContextData(uiDataAddr, &uiTcb);
	pucTaskName = ((REMIX_TCB *) uiTcb)->pucTaskName;
	REMIX_IfValidString(&pucTaskName);
	REMIX_SendString("\r\n\r\nTask %s 0x%08X ", pucTaskName, uiTcb);

	uiDataAddr = REMIX_GetOneContextData(uiDataAddr, &uiData);
	if (0 != uiData) {
		pucTaskName = ((REMIX_TCB *) uiData)->pucTaskName;
		REMIX_IfValidString(&pucTaskName);
		REMIX_SendString("switch to task %s 0x%08X ", pucTaskName, uiData);
	} else {
		REMIX_SendString("occur exception!");
	}

	uiDataAddr = REMIX_GetOneContextData(uiDataAddr, &uiData);
	REMIX_SendString("\r\nExc_Rtn = 0x%08X", uiData);

	uiDataAddr = REMIX_GetOneContextData(uiDataAddr, &uiData);
	REMIX_SendString("\r\nXPSR= 0x%08X", uiData);

	uiDataAddr = REMIX_GetOneContextData(uiDataAddr, &uiData);
	REMIX_SendString("\r\nR15 = 0x%08X", uiData);

	uiDataAddr = REMIX_GetOneContextData(uiDataAddr, &uiData);
	REMIX_SendString(" R14 = 0x%08X", uiData);

	uiDataAddr = REMIX_GetOneContextData(uiDataAddr, &uiData);
	REMIX_SendString(" R13 = 0x%08X", uiData);

	uiDataAddr = REMIX_GetOneContextData(uiDataAddr, &uiData);
	REMIX_SendString(" R12 = 0x%08X", uiData);

	uiDataAddr = REMIX_GetOneContextData(uiDataAddr, &uiData);
	REMIX_SendString("\r\nR11 = 0x%08X", uiData);

	uiDataAddr = REMIX_GetOneContextData(uiDataAddr, &uiData);
	REMIX_SendString(" R10 = 0x%08X", uiData);

	uiDataAddr = REMIX_GetOneContextData(uiDataAddr, &uiData);
	REMIX_SendString(" R9  = 0x%08X", uiData);

	uiDataAddr = REMIX_GetOneContextData(uiDataAddr, &uiData);
	REMIX_SendString(" R8  = 0x%08X", uiData);

	uiDataAddr = REMIX_GetOneContextData(uiDataAddr, &uiData);
	REMIX_SendString("\r\nR7  = 0x%08X", uiData);

	uiDataAddr = REMIX_GetOneContextData(uiDataAddr, &uiData);
	REMIX_SendString(" R6  = 0x%08X", uiData);

	uiDataAddr = REMIX_GetOneContextData(uiDataAddr, &uiData);
	REMIX_SendString(" R5  = 0x%08X", uiData);

	uiDataAddr = REMIX_GetOneContextData(uiDataAddr, &uiData);
	REMIX_SendString(" R4  = 0x%08X", uiData);

	uiDataAddr = REMIX_GetOneContextData(uiDataAddr, &uiData);
	REMIX_SendString("\r\nR3  = 0x%08X", uiData);

	uiDataAddr = REMIX_GetOneContextData(uiDataAddr, &uiData);
	REMIX_SendString(" R2  = 0x%08X", uiData);

	uiDataAddr = REMIX_GetOneContextData(uiDataAddr, &uiData);
	REMIX_SendString(" R1  = 0x%08X", uiData);

	uiDataAddr = REMIX_GetOneContextData(uiDataAddr, &uiData);
	REMIX_SendString(" R0  = 0x%08X", uiData);

	for (i = 0; i < (uiLen - sizeof(REMIX_CONTMSG)) / sizeof(U32); i++) {
		uiTcb -= 4;

		uiDataAddr = REMIX_GetOneContextData(uiDataAddr, &uiData);

		REMIX_SendString("\r\n0x%08X: 0x%08X", uiTcb, uiData);
	}

	gpstrContext->uiRemLen += uiLen;
}

void REMIX_SendCharFuncInit(void (*vfFuncPointer) (U8))
{
	gpstrContext->vfSendChar = vfFuncPointer;
}

void REMIX_SendChar(U8 ucChar)
{
	if (NULL == gpstrContext->vfSendChar) {
		return;
	}

	gpstrContext->vfSendChar(ucChar);
}

void REMIX_SendString(U8 * pvStringPt, ...)
{
#define BUFLEN      200
	U8 aucBuf[BUFLEN];
	U32 i;
	va_list args;

	va_start(args, pvStringPt);
	(void) vsprintf(aucBuf, pvStringPt, args);
	va_end(args);

	for (i = 0; NULL != aucBuf[i]; i++) {
		REMIX_SendChar(aucBuf[i]);
	}
}

void REMIX_IfValidString(U8 ** pucString)
{
	U8 *pucStr;
	U32 i;

	if (NULL == pucString) {
		goto BAD_RTN;
	}

	pucStr = *pucString;

	if (((U32) pucStr < ROMVALIDADDRSTART) || ((U32) pucStr > ROMVALIDADDREND)
	    || (NULL == pucStr)) {
		goto BAD_RTN;
	}

	for (i = 0; i < VALIDTASKNAMEMAXLEN; i++) {
		if (NULL == pucStr[i]) {
			return;
		}

		if ((pucStr[i] < ' ') || (pucStr[i] > '~')) {
			goto BAD_RTN;
		}
	}


      BAD_RTN:

	*pucString = "BAD DATA!";

	return;
}

#endif

//*************************************************************************//


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

void REMIX_TaskCpuShareInit(REMIX_TCB * pstrTcb)
{
	pstrTcb->strCpuShare.uiSysTickVal = 0;
	pstrTcb->strCpuShare.uiCounter = 0;
	pstrTcb->strCpuShare.uiCpuShare = 0;
}

void REMIX_CpuShareStatistic(REMIX_TCB * pstrOldTcb, REMIX_TCB * pstrNewTcb)
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

U32 REMIX_GetCpuShare(REMIX_TCB * pstrTcb)
{
	return pstrTcb->strCpuShare.uiCpuShare;
}

#endif
