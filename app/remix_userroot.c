#include "remix_userroot.h"

/**********************************************/
//函数功能:根任务，用户代码从此执行
//输入参数:none
//返回值  :none
/**********************************************/
void REMIX_RootTask(void)
{
	DEV_SoftwareInit();

	DEV_HardwareInit();

	/* 创建任务1 */
	(void) REMIX_TaskCreate("Teacher", TEST_TestTask1, NULL, NULL, TASKSTACK, 2, NULL);

	/* 创建任务2 */
	(void) REMIX_TaskCreate("Student1", TEST_TestTask2, NULL, NULL, TASKSTACK, 3, NULL);

	/* 创建任务3 */
	(void) REMIX_TaskCreate("Student2", TEST_TestTask3, NULL, NULL, TASKSTACK, 4, NULL);

	/* 创建打印任务 */
	gpstrSerialTaskTcb = REMIX_TaskCreate("SrlPrt", TEST_SerialPrintTask, NULL, NULL, TASKSTACK, 6, NULL);

	DEV_PutStrToMem((U8 *) "\r\nREMIX is running! Tick is: %d", REMIX_GetSystemTick());

}
