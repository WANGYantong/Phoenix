#include "global.h"

S32 main(void)
{
    REMIXOS_Init();

	DEV_SoftwareInit();

	DEV_HardwareInit();

	(void) REMIX_TaskCreate("Task1", TEST_TestTask1, NULL, NULL, TASKSTACK, 3, NULL);
	(void) REMIX_TaskCreate("Task2", TEST_TestTask2, NULL, NULL, TASKSTACK, 3, NULL);
	(void) REMIX_TaskCreate("Task3", TEST_TestTask3, NULL, NULL, TASKSTACK, 4, NULL);
    (void) REMIX_TaskCreate("Task4", TEST_TestTask4, NULL, NULL, TASKSTACK, 4, NULL);

	gpstrSerialTaskTcb = REMIX_TaskCreate("SrlPrt", TEST_SerialPrintTask, NULL, NULL, TASKSTACK, 6, NULL);

	DEV_PutStrToMem((U8 *) "\r\nREMIX is running! Tick is: %d", REMIX_GetSystemTick());

    REMIXOS_Start();

    return 0;
}
