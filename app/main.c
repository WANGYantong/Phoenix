#include "global.h"

U8 buffer[5][TASKSTACK];
REMIX_MEM *pstrMemBuf;
U8 *pucTask1Stack;
U8 *pucTask2Stack;
U8 *pucTask3Stack;
U8 *pucTask4Stack;

U32 uiTask1StackSizeBackUp;
U32 uiTask2StackSizeBackUp;
U32 uiTask3StackSizeBackUp;
U32 uiTask4StackSizeBackUp;

S32 main(void)
{
	REMIXOS_Init();

	DEV_SoftwareInit();

	DEV_HardwareInit();

	pstrMemBuf = REMIX_MemCreate(buffer, 5, TASKSTACK);

	pucTask1Stack = REMIX_MemMalloc(pstrMemBuf, TASKSTACK, &uiTask1StackSizeBackUp);
	pucTask2Stack = REMIX_MemMalloc(pstrMemBuf, TASKSTACK, &uiTask2StackSizeBackUp);
	pucTask3Stack = REMIX_MemMalloc(pstrMemBuf, TASKSTACK, &uiTask3StackSizeBackUp);
	pucTask4Stack = REMIX_MemMalloc(pstrMemBuf, TASKSTACK, &uiTask4StackSizeBackUp);

	(void) REMIX_TaskCreate("Task1", TEST_TestTask1, NULL, pucTask1Stack, TASKSTACK, 2, NULL);
	(void) REMIX_TaskCreate("Task2", TEST_TestTask2, NULL, pucTask2Stack, TASKSTACK, 3, NULL);
	(void) REMIX_TaskCreate("Task3", TEST_TestTask3, NULL, pucTask3Stack, TASKSTACK, 4, NULL);
	(void) REMIX_TaskCreate("Task4", TEST_TestTask4, NULL, pucTask4Stack, TASKSTACK, 4, NULL);

	(void) REMIX_TaskCreate("Cleaner", TEST_TestTask5, NULL, NULL, TASKSTACK, 5, NULL);

	gpstrSerialTaskTcb = REMIX_TaskCreate("SrlPrt", TEST_SerialPrintTask, NULL, NULL, TASKSTACK, 6, NULL);

	DEV_PutStrToMem((U8 *) "\r\nREMIX is running! Tick is: %d", REMIX_GetSystemTick());

	REMIXOS_Start();

	return 0;
}
