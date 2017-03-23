#ifndef __REMIX_MEM_PUBLIC_H__
#define __REMIX_MEM_PUBLIC_H__

#ifdef REMIX_MEMSTATIC

extern REMIX_MEM* REMIX_MemCreate(void * pvaddr, U32 uiNumBlks, U32 uiBlkSize);
extern void* REMIX_MemMalloc(REMIX_MEM * pstrMem, U32 uiSize, U32 * uiSizeBackUp);
extern U8 REMIX_MemFree(REMIX_MEM * pstrMem, void * pvBlk, U32 * uiSizeBackUp);

#endif

#endif

