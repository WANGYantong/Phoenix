#include "remix_private.h"

#ifdef REMIX_SEMGROUPFLAG

REMIX_FLAG* REMIX_FlagCreate(REMIX_FLAG* pstrFlag, U32 uiFlagOpt, U32 uiFlagBit)
{
    U8* pucFlagMemAddr;

    if(REMIXFLAGSCHEDFIFO!=(FlagOpt & REMIXFLAGSCHEDMASK)
        && REMIXFLAGSCHEDPRIO!=(FlagOpt & REMIXFLAGSCHEDMASK)){
        return (REMIX_FLAG*)NULL;
    }

    if(NULL==pstrFlag){

        REMIX_IntLock();

        pucFlagMemAddr=malloc(sizeof(REMIX_FLAG));
        if(NULL==pucFlagMemAddr){

            REMIX_IntUnlock();

            return (REMIX_FLAG*)NULL;
        }

        REMIX_IntUnlock();

        pstrFlag=(REMIX_FLAG*)pucFlagMemAddr;
    }
    else{
        pucFlagMemAddr=(U8*)NULL;
    }

    REMIX_TaskFlagTableInit(&pstrFlag->strFlagTab);
    pstrFlag->uiFlagOpt=uiFlagOpt;
    pstrFlag->uiFlagNowBit=uiFlagBit;
    pstrFlag->pucFlagMem=pucFlagMemAddr;

    return pstrFlag;

}



#endif