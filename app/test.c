#include "test.h"
#include <stdlib.h>

REMIX_QUEUE *gpstrSerialMsgQue;
REMIX_TCB *gpstrSerialTaskTcb;
//REMIX_SEM *gpstrSemMut;
//U32 number;
REMIX_FLAG *gpstrFlag;

void TEST_TestTask1(void *pvPara)
{
    DEV_PutStrToMem((U8 *) "\r\nTask1 suspend Task1! Tick is: %d", REMIX_GetSystemTick());
    REMIX_TaskSuspend(gpstrTask1);

	while (1) {
		DEV_PutStrToMem((U8 *) "\r\nTask1 is searching flag! Tick is: %d", REMIX_GetSystemTick());

		if (RTN_SUCD ==
		    REMIX_FlagTake(gpstrFlag, (U32) 0x3, (REMIXFLAGWAITSETAND + REMIXFLAGCONSUME), FLAGWAITFOREVER)) {
			DEV_PutStrToMem((U8 *) "\r\nTask1 gets flag! Tick is: %d", REMIX_GetSystemTick());
		}

		(void) REMIX_TaskDelay(200);

		if (REMIX_GetSystemTick() > 15000) {

           // DEV_PutStrToMem((U8 *) "\r\nTask1 resume Task2! Tick is: %d", REMIX_GetSystemTick());
           // REMIX_TaskResume(gpstrTask2);

			DEV_PutStrToMem((U8 *) "\r\nTask1 delete itself! Tick is: %d", REMIX_GetSystemTick());
			break;
		}
	}
}

void TEST_TestTask2(void *pvPara)
{
	while (1) {
		DEV_PutStrToMem((U8 *) "\r\nTask2 is searching flag! Tick is: %d", REMIX_GetSystemTick());

		if (RTN_SUCD ==
		    REMIX_FlagTake(gpstrFlag, (U32) 0x3, (REMIXFLAGWAITSETAND + REMIXFLAGCONSUME), FLAGWAITFOREVER)) {
			DEV_PutStrToMem((U8 *) "\r\nTask2 gets flag! Tick is: %d", REMIX_GetSystemTick());
		}
		(void) REMIX_TaskDelay(200);
		if (REMIX_GetSystemTick() > 10000) {
            DEV_PutStrToMem((U8 *) "\r\nTask2 resume Task1! Tick is: %d", REMIX_GetSystemTick());
            REMIX_TaskResume(gpstrTask1);

            DEV_PutStrToMem((U8 *) "\r\nTask2 delete itself! Tick is: %d", REMIX_GetSystemTick());
			break;
		}
	}
}

void TEST_TestTask3(void *pvPara)
{
	while (1) {
		DEV_PutStrToMem((U8 *) "\r\nTask3 is releasing first bit! Tick is: %d", REMIX_GetSystemTick());
		TEST_TaskRun(1500);

		DEV_PutStrToMem((U8 *) "\r\nTask3 released first bit, sleeping now! Tick is: %d",
				REMIX_GetSystemTick());

		(void) REMIX_FlagGive(gpstrFlag, (U32) 0x1, REMIXFLAGSET);

		(void) REMIX_TaskDelay(400);

		if (REMIX_GetSystemTick() > 16000) {
			DEV_PutStrToMem((U8 *) "\r\nTask3 delete itself! Tick is: %d", REMIX_GetSystemTick());
			break;
		}
	}

}

void TEST_TestTask4(void *pvPara)
{
	while (1) {
		DEV_PutStrToMem((U8 *) "\r\nTask4 is releasing second bit! Tick is: %d", REMIX_GetSystemTick());
		TEST_TaskRun(2000);

		DEV_PutStrToMem((U8 *) "\r\nTask4 released second bit, sleeping now! Tick is: %d",
				REMIX_GetSystemTick());

		(void) REMIX_FlagGive(gpstrFlag, (U32) 0x2, REMIXFLAGSET);

		(void) REMIX_TaskDelay(300);

		if (REMIX_GetSystemTick() > 16000) {
			DEV_PutStrToMem((U8 *) "\r\nTask4 delete itself! Tick is: %d", REMIX_GetSystemTick());
			break;
		}
	}

}

void TEST_TestTask5(void *pvPara)
{
	while (1) {
		if (REMIX_GetSystemTick() > 18000) {
			DEV_PutStrToMem((U8 *) "\r\nFree Task1 Memory! Tick is: %d", REMIX_GetSystemTick());
			REMIX_MemFree(pstrMemBuf, pucTask1Stack, &uiTask1StackSizeBackUp);
			DEV_PutStrToMem((U8 *) "\r\nFree Task2 Memory! Tick is: %d", REMIX_GetSystemTick());
			REMIX_MemFree(pstrMemBuf, pucTask2Stack, &uiTask2StackSizeBackUp);
			DEV_PutStrToMem((U8 *) "\r\nFree Task3 Memory! Tick is: %d", REMIX_GetSystemTick());
			REMIX_MemFree(pstrMemBuf, pucTask3Stack, &uiTask3StackSizeBackUp);
			DEV_PutStrToMem((U8 *) "\r\nFree Task4 Memory! Tick is: %d", REMIX_GetSystemTick());
			REMIX_MemFree(pstrMemBuf, pucTask4Stack, &uiTask4StackSizeBackUp);
			break;
		}

		(void) REMIX_TaskDelay(500);
	}
}

// void TEST_TestTask1(void *pvPara)
// {
//      while (1) {

//              if (RTN_SUCD == REMIX_SemTake(gpstrSemMut, SEMWAITFOREVER)) {
//                      DEV_PutStrToMem((U8 *) "\r\nTeacher will release a question! Tick is: %d",
//                                      REMIX_GetSystemTick());

//                      DEV_PutStrToMem((U8 *) "\r\nTeacher is thinking...");

//                      TEST_TaskRun(1000);

//                      number = REMIX_GetSystemTick() % 39;

//                      DEV_PutStrToMem((U8 *) "\r\nTeacher ask students to count the fibonacci: %d", number);

//                      REMIX_SemGive(gpstrSemMut);

//              }

//              DEV_PutStrToMem((U8 *) "\r\nTeacher is sleeping! Tick is: %d", REMIX_GetSystemTick());

//              (void) REMIX_TaskDelay(300);

//              DEV_PutStrToMem((U8 *) "\r\nTeacher wakes up! Tick is: %d", REMIX_GetSystemTick());

//              if (REMIX_GetSystemTick() > 7000) {
//                      DEV_PutStrToMem((U8 *) "\r\nTeacher says class over Tick is: %d", REMIX_GetSystemTick());
//                      break;
//              }
//      }
// }

// void TEST_TestTask2(void *pvPara)
// {
//      U32 uiStackRemainLen;

//      uiStackRemainLen = REMIX_TaskStackCheck(REMIX_GetCurrentTcb());
//      DEV_PutStrToMem((U8 *) "\r\nStudent1 stack remain %d bytes, Tick is: %d", uiStackRemainLen,
//                      REMIX_GetSystemTick());

//      while (1) {

//              if (RTN_SUCD == REMIX_SemTake(gpstrSemMut, SEMNOWAIT)) {

//                      DEV_PutStrToMem((U8 *) "\r\nStudent1 answers the question! Tick is: %d", REMIX_GetSystemTick());

//                      DEV_PutStrToMem((U8 *) "\r\nStudent1 is thinking...");

//                      TEST_TaskRun(1000);

//                      DEV_PutStrToMem((U8 *) "\r\nStudent1 says the %d fibonacci number is %d !", number,
//                                      TEST_Fibonacci(number));

//                      uiStackRemainLen = REMIX_TaskStackCheck(REMIX_GetCurrentTcb());
//                      DEV_PutStrToMem((U8 *) "\r\nStudent1 stack remain %d bytes, Tick is: %d", uiStackRemainLen,
//                                      REMIX_GetSystemTick());

//                      REMIX_SemGive(gpstrSemMut);

//              }

//              DEV_PutStrToMem((U8 *) "\r\nStudent1 is sleeping! Tick is: %d", REMIX_GetSystemTick());

//              (void) REMIX_TaskDelay(200);

//              DEV_PutStrToMem((U8 *) "\r\nStudent1 wakes up! Tick is: %d", REMIX_GetSystemTick());

//              if (REMIX_GetSystemTick() > 7000) {
//                      DEV_PutStrToMem((U8 *) "\r\nStudent1 knows class over");

//                      uiStackRemainLen = REMIX_TaskStackCheck(REMIX_GetCurrentTcb());
//                      DEV_PutStrToMem((U8 *) "\r\nStudent1 stack remain %d bytes, Tick is: %d", uiStackRemainLen,
//                                      REMIX_GetSystemTick());
//                      break;
//              }
//      }
// }

// void TEST_TestTask3(void *pvPara)
// {
//      U32 uiStackRemainLen;

//      uiStackRemainLen = REMIX_TaskStackCheck(REMIX_GetCurrentTcb());
//      DEV_PutStrToMem((U8 *) "\r\nStudent2 stack remain %d bytes, Tick is: %d", uiStackRemainLen,
//                      REMIX_GetSystemTick());

//      while (1) {

//              if (RTN_SUCD == REMIX_SemTake(gpstrSemMut, SEMNOWAIT)) {

//                      DEV_PutStrToMem((U8 *) "\r\nStudent2 answers the question! Tick is: %d", REMIX_GetSystemTick());

//                      DEV_PutStrToMem((U8 *) "\r\nStudent2 is thinking...");

//                      TEST_TaskRun(2000);

//                      DEV_PutStrToMem((U8 *) "\r\nStudent2 says the %d fibonacci number is %d !", number,
//                                      TEST_Fibonacci(number));

//                      uiStackRemainLen = REMIX_TaskStackCheck(REMIX_GetCurrentTcb());
//                      DEV_PutStrToMem((U8 *) "\r\nStudent2 stack remain %d bytes, Tick is: %d", uiStackRemainLen,
//                                      REMIX_GetSystemTick());

//                      REMIX_SemGive(gpstrSemMut);

//              }

//              DEV_PutStrToMem((U8 *) "\r\nStudent2 is sleeping! Tick is: %d", REMIX_GetSystemTick());

//              (void) REMIX_TaskDelay(100);

//              DEV_PutStrToMem((U8 *) "\r\nStudent2 wakes up! Tick is: %d", REMIX_GetSystemTick());

//              if (REMIX_GetSystemTick() > 7000) {
//                      DEV_PutStrToMem((U8 *) "\r\nStudent2 knows class over");

//                      uiStackRemainLen = REMIX_TaskStackCheck(REMIX_GetCurrentTcb());
//                      DEV_PutStrToMem((U8 *) "\r\nStudent2 stack remain %d bytes, Tick is: %d", uiStackRemainLen,
//                                      REMIX_GetSystemTick());
//                      break;
//              }
//      }

// }

// U32 TEST_Fibonacci(U32 num)
// {
//      U32 index;

// //      DYNAMIC_ARRAY *f = (DYNAMIC_ARRAY *) malloc(sizeof(DYNAMIC_ARRAY) + num);
// //
// //      f->data[0] = 0;
// //      f->data[1] = 1;
// //      if (num > 1) {
// //              for (index = 2; index <= num; index++)
// //                      f->data[index] = f->data[index - 1] + f->data[index - 2];
// //      }

// //      index = f->data[num];

// //      free(f);

//      U32 fibonacci[39];

//      fibonacci[0] = 0;
//      fibonacci[1] = 1;

//      if (num > 1) {
//              for (index = 2; index <= num; index++)
//                      fibonacci[index] = fibonacci[index - 1] + fibonacci[index - 2];
//      }

//      return fibonacci[num];
// }

void TEST_SerialPrintTask(void *pvPara)
{
	REMIX_DLIST *pstrMsgQueNode;
	MSGBUF *pstrMsgBuf;

	while (1) {
		if (REMIX_QueueGetNode(gpstrSerialMsgQue, &pstrMsgQueNode, QUEUEWAITFOREVER) == RTN_SUCD) {
			pstrMsgBuf = (MSGBUF *) pstrMsgQueNode;

			DEV_PrintMsg(pstrMsgBuf->aucBuf, pstrMsgBuf->ucLength);

			DEV_BufferFree(&gstrBufPool, pstrMsgQueNode);
		}
	}

}

void TEST_TaskRun(U32 uiMs)
{
	DEV_DelayMs(uiMs);
}

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

void TEST_TaskSwitchPrint(REMIX_TCB * pstrOldTcb, REMIX_TCB * pstrNewTcb)
{
#ifdef REMIX_CPUSTATISTIC
	TEST_PrintCpuUtilize();
#endif
	if (pstrOldTcb == gpstrSerialTaskTcb) {
		pstrOldTcb = REMIX_GetIdleTcb();
	}

	if (pstrNewTcb == gpstrSerialTaskTcb) {
		pstrNewTcb = REMIX_GetIdleTcb();
	}
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

void TEST_TaskDeletePrint(REMIX_TCB * pstrTcb)
{
	DEV_PutStrToMem((U8 *) "\r\nTask %s is deleted! Tick is: %d", pstrTcb->pucTaskName, REMIX_GetSystemTick());
}

#ifdef REMIX_CPUSTATISTIC
void TEST_PrintCpuUtilize(void)
{
	static U32 suiLastTime = 0;
	U32 uiTime;
	REMIX_DLIST *pstrTaskList;
	REMIX_TCB *pstrTcb;

	uiTime = REMIX_GetSystemTick() / 100;

	if (uiTime != suiLastTime) {
		pstrTaskList = REMIX_GetTaskLinkRoot();

		while (NULL != (pstrTaskList = REMIX_DlistNextNodeEmpInq(REMIX_GetTaskLinkRoot(), pstrTaskList))) {
			pstrTcb = ((REMIX_TCBQUE *) pstrTaskList)->pstrTcb;
			DEV_PutStrToMem((U8 *) "\r\nTask %s CPU utilization is %d%%",
					pstrTcb->pucTaskName, REMIX_GetCPUShare(pstrTcb));
		}

		suiLastTime = uiTime;
	}
}
#endif
