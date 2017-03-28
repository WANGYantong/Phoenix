#include "remix_private.h"

#ifdef REMIX_MEMSTATIC

REMIX_MEM *gpstrMemFreeList;
REMIX_MEM gstrMemTable[REMIX_MEMPARTNUM];

void REMIX_MemInit(void)
{
#if REMIX_MEMPARTNUM == 1u
	REMIX_MemClr((U8 *) & gstrMemTable[0], sizeof(gstrMemTable));
	gpstrMemFreeList = (REMIX_MEM *) & gstrMemTable[0];
#endif

#if REMIX_MEMPARTNUM >=2u
	REMIX_MEM *pstrMem;
	U32 i;

	REMIX_MemClr((U8 *) & gstrMemTable[0], sizeof(gstrMemTable));
	for (i = 0u; i < (REMIX_MEMPARTNUM - 1u); i++) {
		pstrMem = &gstrMemTable[i];
		pstrMem->pvMemFreeList = (void *) &gstrMemTable[i + 1u];
	}
	pstrMem = &gstrMemTable[i];
	pstrMem->pvMemFreeList = (void *) 0;

	gpstrMemFreeList = (REMIX_MEM *) & gstrMemTable[0];
#endif
}

REMIX_MEM *REMIX_MemCreate(void *pvAddr, U32 uiNumBlks, U32 uiBlkSize)
{
	REMIX_MEM *pstrMem;
	U8 *pucBlk;
	void **pLink;
	U32 loop;
	U32 i;

	if (NULL == pvAddr) {
		return (REMIX_MEM *) NULL;
	}

	if (2 > uiNumBlks) {
		return (REMIX_MEM *) NULL;
	}

	if (sizeof(void *) > uiBlkSize) {
		return (REMIX_MEM *) NULL;
	}

    (void)REMIX_InterruptLock();

	pstrMem = gpstrMemFreeList;

	if ((REMIX_MEM *) NULL != gpstrMemFreeList) {
		gpstrMemFreeList = (REMIX_MEM *) gpstrMemFreeList->pvMemFreeList;
	}

    (void)REMIX_InterruptUnlock();

	if ((REMIX_MEM *) NULL == pstrMem) {
		return (REMIX_MEM *) NULL;
	}

	pLink = (void **) pvAddr;
	pucBlk = (U8 *) pvAddr;
	loop = uiNumBlks - 1u;
	for (i = 0; i < loop; i++) {
		pucBlk += uiBlkSize;
		*pLink = (void *) pucBlk;
		pLink = (void **) pucBlk;
	}
	*pLink = (void *) 0;

	pstrMem->pvMemAddr = pvAddr;
	pstrMem->pvMemFreeList = pvAddr;
	pstrMem->uiMemBlkSize = uiBlkSize;
	pstrMem->uiMemNumBlk = uiNumBlks;
	pstrMem->uiMemNumFree = uiNumBlks;

	return pstrMem;

}

void *REMIX_MemMalloc(REMIX_MEM * pstrMem, U32 uiSize, U32 * uiSizeBackUp)
{
	void *pvBlk;

	if ((REMIX_MEM *) NULL == pstrMem) {
		return NULL;
	}

	if (uiSize > (pstrMem->uiMemBlkSize)) {
		return NULL;
	}

    (void)REMIX_InterruptLock();

	if (pstrMem->uiMemNumFree > 0u) {
		pvBlk = pstrMem->pvMemFreeList;
		pstrMem->pvMemFreeList = *(void **) pvBlk;
		pstrMem->uiMemNumFree--;
		*uiSizeBackUp = pstrMem->uiMemBlkSize;

        (void)REMIX_InterruptUnlock();

		return (pvBlk);
	}

    (void)REMIX_InterruptUnlock();

	return NULL;

}

U8 REMIX_MemFree(REMIX_MEM * pstrMem, void *pvBlk, U32 * uiSizeBackUp)
{
	if (((REMIX_MEM *) NULL == pstrMem)
	    || (NULL == pvBlk)) {
		return RTN_FAIL;
	}

    (void)REMIX_InterruptLock();

	if ((pstrMem->uiMemNumFree) >= (pstrMem->uiMemNumBlk)) {

        (void)REMIX_InterruptUnlock();

		return RTN_FAIL;
	}

	if (*uiSizeBackUp != (pstrMem->uiMemBlkSize)) {

        (void)REMIX_InterruptUnlock();

		return RTN_FAIL;
	}

	*(void **) pvBlk = pstrMem->pvMemFreeList;
	pstrMem->pvMemFreeList = pvBlk;
	pstrMem->uiMemNumFree++;

    (void)REMIX_InterruptUnlock();

	return RTN_SUCD;
}

#endif
