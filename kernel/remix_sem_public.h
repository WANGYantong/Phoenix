#ifndef __REMIX_SEM_PUBLIC_H__
#define __REMIX_SEM_PUBLIC_H__

#ifndef RTN_SUCD
    #define RTN_SUCD            0
#endif

#ifndef RTN_FAIL
    #define RTN_FAIL            1
#endif

#define RTN_SEMTASKTIMEOUT      2
#define RTN_SEMTASKRETURN       3
#define RTN_SEMTASKDELETE       4
#define RTN_SEMTASKOVERFLOW     5

#define RTN_SEMGIVEOVERFLOW     2

#define SEMSCHEDULEOFFSET       0
#define SEMTYPEOFFSET           2
#define SEMPRIORITYINHOFFSET    5

#define SEMFIFO                 (1<<SEMSCHEDULEOFFSET)
#define SEMPRIO                 (2<<SEMSCHEDULEOFFSET)

#define SEMBIN                  (1<<SEMTYPEOFFSET)
#define SEMCNT                  (2<<SEMTYPEOFFSET)
#define SEMMUT                  (4<<SEMTYPEOFFSET)

#define SEMPRIINH               (1<<SEMPRIORITYINHOFFSET)

#define SEMEMPTY                0
#define SEMFULL                 0xFFFFFFFF

#define SEMNOWAIT               0
#define SEMWAITFOREVER          0xFFFFFFFF


extern REMIX_SEM* REMIX_SemCreate(REMIX_SEM * pstrSem, U32 uiSemOpt, U32 uiInitVal);
extern U32 REMIX_SemTake(REMIX_SEM * pstrSem, U32 uiDelayTick);
extern U32 REMIX_SemGive(REMIX_SEM * pstrSem);
extern U32 REMIX_SemFlush(REMIX_SEM * pstrSem);
extern U32 REMIX_SemDelete(REMIX_SEM * pstrSem);

#endif

