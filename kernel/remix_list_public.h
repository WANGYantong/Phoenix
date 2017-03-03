
#ifndef  __REMIX_LIST_PUBLIC_H__
#define  __REMIX_LIST_PUBLIC_H__

extern void REMIX_DlistInit(REMIX_DLIST* pstrList);
extern void REMIX_DlistNodeAdd(REMIX_DLIST* pstrList, REMIX_DLIST* pstrNode);
extern REMIX_DLIST* REMIX_DlistNodeDelete(REMIX_DLIST* pstrList);
extern void REMIX_DlistCurNodeInsert(REMIX_DLIST* pstrList, REMIX_DLIST* pstrNode,
                                   REMIX_DLIST* pstrNewNode);
extern REMIX_DLIST* REMIX_DlistCurNodeDelete(REMIX_DLIST* pstrList, REMIX_DLIST* pstrNode);
extern REMIX_DLIST* REMIX_DlistEmpInq(REMIX_DLIST* pstrList);
extern REMIX_DLIST* REMIX_DlistNextNodeEmpInq(REMIX_DLIST* pstrList, REMIX_DLIST* pstrNode);


#endif

