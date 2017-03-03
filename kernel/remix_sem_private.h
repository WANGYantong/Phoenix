#ifndef __REMIX_SEM_PRIVATE_H__
#define __REMIX_SEM_PRIVATE_H__

#define SEMSCHEDULEMASK             0x00000003
#define SEMTYPEMASK                 0x0000001C
#define SEMPRIORITYINHMASK          0x00000020


extern U32 REMIX_SemFlushValue(REMIX_SEM * pstrSem, U32 uiRtnValue);

#endif