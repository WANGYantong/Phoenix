#ifndef __REMIX_QUEUE_PUBLIC_H__
#define __REMIX_QUEUE_PUBLIC_H__

#define RTN_QUEUEPUTOVERFLOW            RTN_SEMGIVEOVERFLOW

#define RTN_QUEUEGETTIMEOUT             RTN_SEMTASKTIMEOUT
#define RTN_QUEUEGETRETURN              RTN_SEMTASKRETURN
#define RTN_QUEUEGETDELETE              RTN_SEMTASKDELETE

#define QUEUEFIFO                       SEMFIFO
#define QUEUEPRIO                       SEMPRIO

#define QUEUEEMPTY                      SEMEMPTY
#define QUEUEFULL                       SEMFULL

#define QUEUENOWAIT                     SEMNOWAIT
#define QUEUEWAITFOREVER                SEMWAITFOREVER

typedef struct remix_queue
{
    REMIX_DLIST strList;
    REMIX_SEM strSem;
    U8* pucQueueMem;
}REMIX_QUEUE;


extern REMIX_QUEUE* REMIX_QueueCreate(REMIX_QUEUE * pstrQue, U32 uiQueOpt);
extern U32 REMIX_QueuePutNode(REMIX_QUEUE * pstrQue, REMIX_DLIST * pstrQueNode);
extern U32 REMIX_QueueGetNode(REMIX_QUEUE * pstrQue, REMIX_DLIST * * ppstrQueNode, U32 uiDelayTick);
extern U32 REMIX_QueueDelete(REMIX_QUEUE * pstrQue);

#endif
