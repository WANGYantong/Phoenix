#include "remix_private.h"

#ifdef REMIX_SEMGROUPFLAG

REMIX_FLAG* REMIX_FlagCreate(REMIX_FLAG* pstrFlag, U32 uiFlagOpt, U32 uiFlagBit)
{
    U8* pucFlagMemAddr;

    if(REMIXFLAGSCHEDFIFO!=(uiFlagOpt & REMIXFLAGSCHEDMASK)
        && REMIXFLAGSCHEDPRIO!=(uiFlagOpt & REMIXFLAGSCHEDMASK)){
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

U32 REMIX_FlagTake(REMIX_FLAG* pstrFlag, U32 uiFlagWantBit, U32 uiFlagNodeOpt, U32 uiDelayTick)
{
    U32 uiFlagSta;
    U8  ucConsume;

    if(NULL==pstrFlag){
        return RTN_FAIL;
    }

    if((REMIXFLAGWAITCLRAND!=(uiFlagNodeOpt & REMIXFLAGWAITTYPEMASK))
        && (REMIXFLAGWAITSETAND!=(uiFlagNodeOpt & REMIXFLAGWAITTYPEMASK))
        && (REMIXFLAGWAITCLROR !=(uiFlagNodeOpt & REMIXFLAGWAITTYPEMASK))
        && (REMIXFLAGWAITSETOR !=(uiFlagNodeOpt & REMIXFLAGWAITTYPEMASK))){
        return RTN_FAIL;
    }

    if((RTN_SUCD==REMIX_RunInInt())&&(FLAGNOWAIT!=uiDelayTick)){
        return RTN_FAIL;
    }

    if(REMIXFLAGCONSUME==(uiFlagNodeOpt&REMIXFLAGCONSUMEMASK)){
        ucConsume=1;
        uiFlagNodeOpt&=~REMIXFLAGCONSUME;
    }else{
        ucConsume=0;
    }

    (void)REMIX_IntLock();

    switch(uiFlagNodeOpt){

        case REMIXFLAGWAITSETAND:

            uiFlagSta=pstrFlag->uiFlagNowBit&uiFlagWantBit;

            if(uiFlagSta==uiFlagWantBit){

                if(1==ucConsume){
                    pstrFlag->uiFlagNowBit&=~uiFlagSta;
                }

                (void)REMIX_IntUnlock();

                return RTN_SUCD;
            }
            else{

                if(FLAGNOWAIT==uiDelayTick){

                    (void)REMIX_IntUnlock();

                    return RTN_FLAGTASKRETURN;

                }

                if(RTN_FAIL==REMIX_FlagBlock(pstrFlag,uiFlagWantBit,uiFlagNodeOpt,uiDelayTick)){
                    (void)REMIX_IntUnlock();
                    return RTN_FAIL;
                }

                (void)REMIX_IntUnlock();

                REMIX_TaskSwiSched();

                return gpstrCurTcb->strTaskOpt.uiDelayTick;

            }

        case REMIXFLAGWAITSETOR:

            uiFlagSta=pstrFlag->uiFlagNowBit&uiFlagWantBit;

            if(uiFlagSta!=(U32)0){

                if(1==ucConsume){
                    pstrFlag->uiFlagNowBit&=~uiFlagSta;
                }

                (void)REMIX_IntUnlock();

                return RTN_SUCD;
            }
            else{
                if(FLAGNOWAIT==uiDelayTick){

                    (void)REMIX_IntUnlock();

                    return RTN_FLAGTASKRETURN;
                }

                if(RTN_FAIL==REMIX_FlagBlock(pstrFlag, uiFlagWantBit, uiFlagNodeOpt, uiDelayTick)){
                    (void)REMIX_IntUnlock();
                    return RTN_FAIL;
                }

                (void)REMIX_IntUnlock();

                REMIX_TaskSwiSched();

                return gpstrCurTcb->strTaskOpt.uiDelayTick;

            }


        case REMIXFLAGWAITCLRAND:

            uiFlagSta=~pstrFlag->uiFlagNowBit&uiFlagWantBit;

            if(uiFlagSta==uiFlagWantBit){

                if(1==ucConsume){
                    pstrFlag->uiFlagNowBit |= uiFlagSta;
                }

                (void)REMIX_IntUnlock();

                return RTN_SUCD;

            }
            else{
                if(FLAGNOWAIT==uiDelayTick){

                    (void)REMIX_IntUnlock();

                    return RTN_FLAGTASKRETURN;

                }

                if(RTN_FAIL==REMIX_FlagBlock(pstrFlag, uiFlagWantBit, uiFlagNodeOpt, uiDelayTick)){
                    (void)REMIX_IntUnlock();
                    return RTN_FAIL;
                }

                (void)REMIX_IntUnlock();

                REMIX_TaskSwiSched();

                return gpstrCurTcb->strTaskOpt.uiDelayTick;

            }

        case REMIXFLAGWAITCLROR:

            uiFlagSta=~pstrFlag->uiFlagNowBit&uiFlagWantBit;

            if(uiFlagSta!=(U32)0){

                if(1==ucConsume){
                    pstrFlag->uiFlagNowBit |= uiFlagSta;
                }

                (void)REMIX_IntUnlock();

                return RTN_SUCD;

            }
            else{
                if(FLAGNOWAIT==uiDelayTick){

                    (void)REMIX_IntUnlock();

                    return RTN_FLAGTASKRETURN;

                }

                if(RTN_FAIL==REMIX_FlagBlock(pstrFlag, uiFlagWantBit, uiFlagNodeOpt, uiDelayTick)){
                    (void)REMIX_IntUnlock();
                    return RTN_FAIL;
                }

                (void)REMIX_IntUnlock();

                REMIX_TaskSwiSched();

                return gpstrCurTcb->strTaskOpt.uiDelayTick;

            }

        default:

            (void)REMIX_IntUnlock();

            return RTN_FAIL;
    }
}


U32 REMIX_FlagGive(REMIX_FLAG* pstrFlag, U32 uiSetBit, U8 ucOpt)
{



}

REMIX_FlagFlushValue

REMIX_FlagFlush

REMIX_FlagDelete

#endif
