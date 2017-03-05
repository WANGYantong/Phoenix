#include "remix_private.h"

void REMIX_DlistInit(REMIX_DLIST * pstrList)
{
	pstrList->pstrHead = (REMIX_DLIST *) NULL;
	pstrList->pstrTail = (REMIX_DLIST *) NULL;
}

void REMIX_DlistNodeAdd(REMIX_DLIST * pstrList, REMIX_DLIST * pstrNode)
{
	if (NULL != pstrList->pstrTail) {
		pstrNode->pstrHead = pstrList->pstrHead;
		pstrNode->pstrTail = pstrList;
		pstrList->pstrHead->pstrTail = pstrNode;
		pstrList->pstrHead = pstrNode;
	} else {
		pstrNode->pstrHead = pstrList;
		pstrNode->pstrTail = pstrList;
		pstrList->pstrHead = pstrNode;
		pstrList->pstrTail = pstrNode;
	}
}

REMIX_DLIST *REMIX_DlistNodeDelete(REMIX_DLIST * pstrList)
{
	REMIX_DLIST *pstrTempNode;
	pstrTempNode = pstrList->pstrTail;

	if (NULL != pstrTempNode) {
		if (pstrList->pstrHead != pstrList->pstrTail) {
			pstrList->pstrTail = pstrTempNode->pstrTail;
			pstrTempNode->pstrTail->pstrHead = pstrList;
		} else {
			pstrList->pstrHead = (REMIX_DLIST *) NULL;
			pstrList->pstrTail = (REMIX_DLIST *) NULL;
		}

		return pstrTempNode;
	} else {
		return (REMIX_DLIST *) NULL;
	}
}

void REMIX_DlistCurNodeInsert(REMIX_DLIST * pstrList, REMIX_DLIST * pstrNode, REMIX_DLIST * pstrNewNode)
{
	if (pstrList != pstrNode) {
		pstrNode->pstrHead->pstrTail = pstrNewNode;
		pstrNewNode->pstrHead = pstrNode->pstrHead;
		pstrNewNode->pstrTail = pstrNode;
		pstrNode->pstrHead = pstrNewNode;
	} else {
		REMIX_DlistNodeAdd(pstrList, pstrNewNode);
	}
}


REMIX_DLIST *REMIX_DlistCurNodeDelete(REMIX_DLIST * pstrList, REMIX_DLIST * pstrNode)
{
	if (pstrList != pstrNode) {
		if ((pstrNode->pstrHead != pstrList) || (pstrNode->pstrTail != pstrList)) {
			pstrNode->pstrHead->pstrTail = pstrNode->pstrTail;
			pstrNode->pstrTail->pstrHead = pstrNode->pstrHead;
			return pstrNode->pstrTail;
		} else {
			(void) REMIX_DlistNodeDelete(pstrList);
			return (REMIX_DLIST *) NULL;
		}
	} else {
		return (REMIX_DLIST *) NULL;
	}
}

REMIX_DLIST *REMIX_DlistEmpInq(REMIX_DLIST * pstrList)
{
	return pstrList->pstrTail;
}

REMIX_DLIST *REMIX_DlistNextNodeEmpInq(REMIX_DLIST * pstrList, REMIX_DLIST * pstrNode)
{

	if (pstrNode->pstrTail == pstrList) {
		return (REMIX_DLIST *) NULL;
	} else {
		return pstrNode->pstrTail;
	}
}
