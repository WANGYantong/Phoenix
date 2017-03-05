#ifndef __DEVICE_H__
#define __DEVICE_H__

#include "global.h"

extern void DEV_UartInit(void);
extern void DEV_PutChar(U8 ucChar);
extern void DEV_BufferInit(BUFPOOL * pstrBufPool);

#endif
