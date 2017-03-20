#ifndef __REMIX_FLAG_PUBLIC_H__
#define __REMIX_FLAG_PUBLIC_H__

#ifdef REMIX_SEMGROUPFLAG

#ifndef RTN_SUCD
    #define RTN_SUCD                0
#endif

#ifdef RTN_FAIL
    #define RTN_FAIL                1
#endif

#define RTN_FLAGTASKTIMEOUT         2
#define RTN_FLAGTASKRETURN          3
#define RTN_FLAGTASKDELETE          4

#define REMIXFLAGWAITCLRAND         0x00000001
#define REMIXFLAGWAITCLROR          0x00000002
#define REMIXFLAGWAITSETAND         0x00000004
#define REMIXFLAGWAITSETOR          0x00000008

#define REMIXFLAGCONSUME            0x00000010

#define REMIXFLAGSCHEDFIFO           0x00000020
#define REMIXFLAGSCHEDPRIO           0x00000040

#define REMIXFLAGCLR                0
#define REMIXFLAGSET                1

#define FLAGNOWAIT                  0
#define FLAGWAITFOREVER             0xFFFFFFFF

#endif

#endif