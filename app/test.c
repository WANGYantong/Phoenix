#include "test.h"
#include <stdlib.h>

REMIX_QUEUE *gpstrSerialMsgQue;	/* 串口打印消息队列指针 */
REMIX_TCB *gpstrSerialTaskTcb;	/* 串口打印任务TCB指针 */
REMIX_SEM *gpstrSemMut;
U32 number;

// void TEST_TestTask1(void *pvPara)
// {
//     U32 uiDelayTime;
//     uiDelayTime=350;

//     while(1){
//         DEV_PutStrToMem((U8*)"\r\nTask1 is running! Tick is: %d",
//                         REMIX_GetSystemTick());
//         TEST_TaskRun(2000);
//         (void)REMIX_TaskDelay(uiDelayTime);

//         if((350==uiDelayTime)&&(REMIX_GetSystemTick()>2000)){
//             uiDelayTime=300;
//         }

// 		if(REMIX_GetSystemTick()>12000){
//             break;
//         }
//     }
// }

// void TEST_TestTask2(void *pvPara)
// {
//     while(1){
//         DEV_PutStrToMem((U8*)"\r\nTask2 is running! Tick is: %d",
//                         REMIX_GetSystemTick());
//         TEST_TaskRun(2000);
//         (void)REMIX_TaskDelay(350);

//         if(REMIX_GetSystemTick()>16000){
//             break;
//         }
//     }
// }

// void TEST_TestTask3(void *pvPara)
// {
//     U32 uiDelayTime;
//     uiDelayTime=350;

//     while(1){
//         DEV_PutStrToMem((U8*)"\r\nTask3 is running! Tick is: %d",
//                         REMIX_GetSystemTick());
//         TEST_TaskRun(1500);
//         (void)REMIX_TaskDelay(uiDelayTime);

//         if((350==uiDelayTime)&&(REMIX_GetSystemTick()>2000)){
//             uiDelayTime=200;
//         }

//         if(REMIX_GetSystemTick()>14000){
//             break;
//         }
//     }
// }

// void TEST_TestTask4(void *pvPara)
// {
//     while(1){
//         DEV_PutStrToMem((U8*)"\r\nTask4 is running! Tick is: %d",
//                         REMIX_GetSystemTick());
//         TEST_TaskRun(1500);
//         (void)REMIX_TaskDelay(200);

//         if(REMIX_GetSystemTick()>18000){
//             break;
//         }
//     }
// }

/**********************************************/
//函数功能:测试任务1
//输入参数:pvPara:任务入口指针
//返回值  :none
/**********************************************/
void TEST_TestTask1(void *pvPara)
{
	while (1) {

		if (RTN_SUCD == REMIX_SemTake(gpstrSemMut, SEMWAITFOREVER)) {
			/* 任务打印 */
			DEV_PutStrToMem((U8 *) "\r\nTeacher will release a question! Tick is: %d",
					REMIX_GetSystemTick());

			DEV_PutStrToMem((U8 *) "\r\nTeacher is thinking...");

			TEST_TaskRun(1000);

			number = REMIX_GetSystemTick() % 39;

			/* 任务打印 */
			DEV_PutStrToMem((U8 *) "\r\nTeacher ask students to count the fibonacci: %d", number);

			REMIX_SemGive(gpstrSemMut);

		}

		DEV_PutStrToMem((U8 *) "\r\nTeacher is sleeping! Tick is: %d", REMIX_GetSystemTick());

		(void) REMIX_TaskDelay(300);

		DEV_PutStrToMem((U8 *) "\r\nTeacher wakes up! Tick is: %d", REMIX_GetSystemTick());

		/* 超过70秒后退出该任务 */
		if (REMIX_GetSystemTick() > 7000) {
			DEV_PutStrToMem((U8 *) "\r\nTeacher says class over Tick is: %d", REMIX_GetSystemTick());
			break;
		}
	}
}

/**********************************************/
//函数功能:测试任务2
//输入参数:pvPara:任务入口指针
//返回值  :none
/**********************************************/
void TEST_TestTask2(void *pvPara)
{
	U32 uiStackRemainLen;

	uiStackRemainLen = REMIX_TaskStackCheck(REMIX_GetCurrentTcb());
	DEV_PutStrToMem((U8 *) "\r\nStudent1 stack remain %d bytes, Tick is: %d", uiStackRemainLen,
			REMIX_GetSystemTick());

	while (1) {

		if (RTN_SUCD == REMIX_SemTake(gpstrSemMut, SEMNOWAIT)) {

			DEV_PutStrToMem((U8 *) "\r\nStudent1 answers the question! Tick is: %d", REMIX_GetSystemTick());

			DEV_PutStrToMem((U8 *) "\r\nStudent1 is thinking...");

			TEST_TaskRun(1000);

			DEV_PutStrToMem((U8 *) "\r\nStudent1 says the %d fibonacci number is %d !", number,
					TEST_Fibonacci(number));

			uiStackRemainLen = REMIX_TaskStackCheck(REMIX_GetCurrentTcb());
			DEV_PutStrToMem((U8 *) "\r\nStudent1 stack remain %d bytes, Tick is: %d", uiStackRemainLen,
					REMIX_GetSystemTick());

			REMIX_SemGive(gpstrSemMut);

		}

		DEV_PutStrToMem((U8 *) "\r\nStudent1 is sleeping! Tick is: %d", REMIX_GetSystemTick());

		(void) REMIX_TaskDelay(200);

		DEV_PutStrToMem((U8 *) "\r\nStudent1 wakes up! Tick is: %d", REMIX_GetSystemTick());

		/* 超过70秒后退出该任务 */
		if (REMIX_GetSystemTick() > 7000) {
			DEV_PutStrToMem((U8 *) "\r\nStudent1 knows class over");

			uiStackRemainLen = REMIX_TaskStackCheck(REMIX_GetCurrentTcb());
			DEV_PutStrToMem((U8 *) "\r\nStudent1 stack remain %d bytes, Tick is: %d", uiStackRemainLen,
					REMIX_GetSystemTick());
			break;
		}
	}
}

/**********************************************/
//函数功能:测试任务3
//输入参数:pvPara:任务入口指针
//返回值  :none
/**********************************************/
void TEST_TestTask3(void *pvPara)
{
	U32 uiStackRemainLen;

	uiStackRemainLen = REMIX_TaskStackCheck(REMIX_GetCurrentTcb());
	DEV_PutStrToMem((U8 *) "\r\nStudent2 stack remain %d bytes, Tick is: %d", uiStackRemainLen,
			REMIX_GetSystemTick());

	while (1) {

		if (RTN_SUCD == REMIX_SemTake(gpstrSemMut, SEMNOWAIT)) {

			DEV_PutStrToMem((U8 *) "\r\nStudent2 answers the question! Tick is: %d", REMIX_GetSystemTick());

			DEV_PutStrToMem((U8 *) "\r\nStudent2 is thinking...");

			TEST_TaskRun(2000);

			DEV_PutStrToMem((U8 *) "\r\nStudent2 says the %d fibonacci number is %d !", number,
					TEST_Fibonacci(number));

			uiStackRemainLen = REMIX_TaskStackCheck(REMIX_GetCurrentTcb());
			DEV_PutStrToMem((U8 *) "\r\nStudent2 stack remain %d bytes, Tick is: %d", uiStackRemainLen,
					REMIX_GetSystemTick());

			REMIX_SemGive(gpstrSemMut);

		}

		/* 任务打印 */
		DEV_PutStrToMem((U8 *) "\r\nStudent2 is sleeping! Tick is: %d", REMIX_GetSystemTick());

		/* 任务延迟1秒 */
		(void) REMIX_TaskDelay(100);

		DEV_PutStrToMem((U8 *) "\r\nStudent2 wakes up! Tick is: %d", REMIX_GetSystemTick());

		/* 超过70秒后退出该任务 */
		if (REMIX_GetSystemTick() > 7000) {
			DEV_PutStrToMem((U8 *) "\r\nStudent2 knows class over");

			uiStackRemainLen = REMIX_TaskStackCheck(REMIX_GetCurrentTcb());
			DEV_PutStrToMem((U8 *) "\r\nStudent2 stack remain %d bytes, Tick is: %d", uiStackRemainLen,
					REMIX_GetSystemTick());
			break;
		}
	}

}

U32 TEST_Fibonacci(U32 num)
{
	U32 index;

//      DYNAMIC_ARRAY *f = (DYNAMIC_ARRAY *) malloc(sizeof(DYNAMIC_ARRAY) + num);
//
//      f->data[0] = 0;
//      f->data[1] = 1;
//      if (num > 1) {
//              for (index = 2; index <= num; index++)
//                      f->data[index] = f->data[index - 1] + f->data[index - 2];
//      }

//      index = f->data[num];

//      free(f);

	U32 fibonacci[39];

	fibonacci[0] = 0;
	fibonacci[1] = 1;

	if (num > 1) {
		for (index = 2; index <= num; index++)
			fibonacci[index] = fibonacci[index - 1] + fibonacci[index - 2];
	}

	return fibonacci[num];
}

/**********************************************/
//函数功能:串口打印任务，从队列中获取需要打印的消息缓冲，将缓冲中的数据打印到串口
//输入参数:pvPara:任务入口指针
//返回值  :none
/**********************************************/
void TEST_SerialPrintTask(void *pvPara)
{
	REMIX_DLIST *pstrMsgQueNode;
	MSGBUF *pstrMsgBuf;

	/* 从队列循环获取消息 */
	while (1) {
		/* 从队列中获取到一条需要打印的消息, 向串口打印消息数据 */
		if (REMIX_QueueGetNode(gpstrSerialMsgQue, &pstrMsgQueNode, QUEUEWAITFOREVER) == RTN_SUCD) {
			pstrMsgBuf = (MSGBUF *) pstrMsgQueNode;

			/* 将缓冲中的数据打印到串口 */
			DEV_PrintMsg(pstrMsgBuf->aucBuf, pstrMsgBuf->ucLength);

			/* 缓冲消息中的数据发送完毕, 释放缓冲 */
			DEV_BufferFree(&gstrBufPool, pstrMsgQueNode);
		}
	}

}

/**********************************************/
//函数功能:模拟任务运行函数
//输入参数:uiMs:要延迟的时间，单位ms
//返回值  :none
/**********************************************/
void TEST_TaskRun(U32 uiMs)
{
	DEV_DelayMs(uiMs);
}

/**********************************************/
//函数功能:将任务创建过程打印到内存中
//输入参数:pstrTcb:新创建的任务的TCB指针
//返回值  :none
/**********************************************/
void TEST_TaskCreatePrint(REMIX_TCB * pstrTcb)
{
	if (pstrTcb != (REMIX_TCB *) NULL) {
		DEV_PutStrToMem((U8 *)
				"\r\nTask %s 0x%08X is created! Tick is: %d",
				pstrTcb->pucTaskName, pstrTcb, REMIX_GetSystemTick());
	} else {
		DEV_PutStrToMem((U8 *)
				"\r\nFail to create task! Tick is: %d", REMIX_GetSystemTick());
	}
}

/**********************************************/
//函数功能:将任务切换过程打印到内存中
//输入参数:pstrOldTcb:切换前的任务TCB指针
//         pstrNewTcb:切换后的任务TCB指针
//返回值  :none
/**********************************************/
void TEST_TaskSwitchPrint(REMIX_TCB * pstrOldTcb, REMIX_TCB * pstrNewTcb)
{
	//打印CPU占用率
#ifdef REMIX_CPUSTATISTIC
	TEST_PrintCpuUtilize();
#endif
	//不打印串口打印任务的切换过程,将此任务定义为空闲任务
	if (pstrOldTcb == gpstrSerialTaskTcb) {
		pstrOldTcb = REMIX_GetIdleTcb();
	}

	if (pstrNewTcb == gpstrSerialTaskTcb) {
		pstrNewTcb = REMIX_GetIdleTcb();
	}
	//同一个任务切换不打印信息
	if (pstrNewTcb == pstrOldTcb) {
		return;
	}

	if (pstrOldTcb != (REMIX_TCB *) NULL) {
		DEV_PutStrToMem((U8 *)
				"\r\nTask %s ----> Task %s ! Tick is: %d",
				pstrOldTcb->pucTaskName, pstrNewTcb->pucTaskName, REMIX_GetSystemTick());
	} else {
		DEV_PutStrToMem((U8 *)
				"\r\nTask NULL ----> Task %s ! Tick is: %d",
				pstrNewTcb->pucTaskName, REMIX_GetSystemTick());
	}

}

/**********************************************/
//函数功能:将任务删除过程打印到内存中
//输入参数:pstrTcb:被删除的任务的TCB指针
//返回值  :none
/**********************************************/
void TEST_TaskDeletePrint(REMIX_TCB * pstrTcb)
{
	DEV_PutStrToMem((U8 *) "\r\nTask %s is deleted! Tick is: %d", pstrTcb->pucTaskName, REMIX_GetSystemTick());
}

/**********************************************/
//函数功能:输出各个任务的CPU占用率
//输入参数:none
//返回值  :none
/**********************************************/
#ifdef REMIX_CPUSTATISTIC
void TEST_PrintCpuUtilize(void)
{
	static U32 suiLastTime = 0;
	U32 uiTime;
	REMIX_DLIST *pstrTaskList;
	REMIX_TCB *pstrTcb;

	/* 获取当前时间 */
	uiTime = REMIX_GetSystemTick() / 100;

	/* 经过1秒时间 */
	if (uiTime != suiLastTime) {
		pstrTaskList = REMIX_GetTaskLinkRoot();

		while (NULL != (pstrTaskList = REMIX_DlistNextNodeEmpInq(REMIX_GetTaskLinkRoot(), pstrTaskList))) {
			pstrTcb = ((REMIX_TCBQUE *) pstrTaskList)->pstrTcb;
			DEV_PutStrToMem((U8 *) "\r\nTask %s CPU utilization is %d%%",
					pstrTcb->pucTaskName, REMIX_GetCPUShare(pstrTcb));
		}

		/* 更新时间 */
		suiLastTime = uiTime;
	}
}
#endif
