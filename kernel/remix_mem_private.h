#ifndef __REMIX_MEM_PRIVATE_H__
#define __REMIX_MEM_PRIVATE_H__

#ifdef REMIX_MEMSTATIC

extern REMIX_MEM* gpstrMemFreeList;
extern REMIX_MEM  gstrMemTable[REMIX_MEMPARTNUM];

extern void REMIX_MemInit(void);

#endif

#endif
