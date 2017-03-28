#ifndef __REMIX_CONFIG_H__
#define __REMIX_CONFIG_H__

#include "stm32f10x.h"

//���ȼ����ҷ�ʽ��ֻ��ѡ��һ��
#define REMIX_RBITANDCLZ
//#define REMIX_UNMAP
//#define REMIX_XORANDSWITCH

//��ѡ���ܣ�����ͬʱѡ��
#define REMIX_MEMSTATIC
#define REMIX_SEMGROUPFLAG
#define REMIX_INCLUDETASKHOOK
#define REMIX_TASKPRIOINHER
#define REMIX_TASKROUNDROBIN
#define REMIX_DEBUGSTACKCHECK
#define REMIX_CPUSTATISTIC

#define TICK                        10
#define CORECLOCKPMS                72000000

#define ROOTTASKNAME                "ROOT"
#define ROOTIDLENAME                "Idle"

#define ROOTTASKSTACK               600
#define IDLETASKSTACK               600

#define STACKALIGNMASK              ALIGN4MASK
#define TCBALIGNMASK                ALIGN8MASK

#define PRIORITYNUM                 PRIORITY8

#ifdef REMIX_MEMSTATIC
#define REMIX_MEMPARTNUM           4
#endif

#ifdef __NVIC_PRIO_BITS
	#define REMIX_PRIO_BITS       		__NVIC_PRIO_BITS
#else
	#define REMIX_PRIO_BITS       		4
#endif

//#define REMIX_KERNEL_CRITICAL_ALL
// #define REMIX_IDLETASKHOOK

#define REMIX_MAX_SYSCALL_INTERRUPT_PRIORITY    (0x01 << (8-REMIX_PRIO_BITS))

#endif
