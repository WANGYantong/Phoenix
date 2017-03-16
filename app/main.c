#include "global.h"

S32 main(void)
{
    REMIXOS_Init();

	DEV_SoftwareInit();

	DEV_HardwareInit();

	/* ��������1 */
	(void) REMIX_TaskCreate("Teacher", TEST_TestTask1, NULL, NULL, TASKSTACK, 4, NULL);

	/* ��������2 */
	(void) REMIX_TaskCreate("Student1", TEST_TestTask2, NULL, NULL, TASKSTACK, 4, NULL);

	/* ��������3 */
	(void) REMIX_TaskCreate("Student2", TEST_TestTask3, NULL, NULL, TASKSTACK, 4, NULL);

	/* ������ӡ���� */
	gpstrSerialTaskTcb = REMIX_TaskCreate("SrlPrt", TEST_SerialPrintTask, NULL, NULL, TASKSTACK, 6, NULL);

	DEV_PutStrToMem((U8 *) "\r\nREMIX is running! Tick is: %d", REMIX_GetSystemTick());

    REMIXOS_Start();

    return 0;
}
