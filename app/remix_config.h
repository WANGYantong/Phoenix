#ifndef __REMIX_CONFIG_H__
#define __REMIX_CONFIG_H__

#include "stm32f10x.h"

#define REMIX_INLUCDETASKHOOK
#define REMIX_TASKPRIOINHER
#define REMIX_TASKROUNDROBIN
#define REMIX_DEBUGCONTEXT
#define REMIX_DEBUGSTACKCHECK
#define REMIX_CPUSTATISTIC

#ifdef REMIX_DEBUGCONTEXT
    #define REMIX_CONTEXTADDR       0x2000B000
    #define REMIX_CONTEXTLEN        0x1000
#endif

#define TICK                        10
#define CORECLOCKPMS                72000000

#define ROOTTASKNAME                "ROOT"
#define ROOTIDLENAME                "Idle"

#define ROOTTASKSTACK               600
#define IDLERASKSTACK               600

#define STACKALIGNMASK              ALIGN4MASK
#define TCBALIGNMASK                ALIGN8MASK

#define PRIORITYNUM                 PRIORITY8

#endif
