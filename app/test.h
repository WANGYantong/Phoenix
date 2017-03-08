#ifndef __TEST_H__
#define __TEST_H__

#include "global.h"

// typedef struct dynamic_array{
//     U32 data[1];
// }DYNAMIC_ARRAY;

extern void TEST_TaskRun(U32 uiMs);
extern void TEST_PrintCpuUtilize(void);
extern U32 TEST_Fibonacci(U32 num);

#endif
