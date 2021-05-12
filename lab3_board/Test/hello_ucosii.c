/*************************************************************************
* Copyright (c) 2004 Altera Corporation, San Jose, California, USA.      *
* All rights reserved. All use of this software and documentation is     *
* subject to the License Agreement located at the end of this file below.*
**************************************************************************
* Description:                                                           *
* The following is a simple hello world program running MicroC/OS-II.The *
* purpose of the design is to be a very simple application that just     *
* demonstrates MicroC/OS-II running on NIOS II.The design doesn't account*
* for issues such as checking system call return codes. etc.             *
*                                                                        *
* Requirements:                                                          *
*   -Supported Example Hardware Platforms                                *
*     Standard                                                           *
*     Full Featured                                                      *
*     Low Cost                                                           *
*   -Supported Development Boards                                        *
*     Nios II Development Board, Stratix II Edition                      *
*     Nios Development Board, Stratix Professional Edition               *
*     Nios Development Board, Stratix Edition                            *
*     Nios Development Board, Cyclone Edition                            *
*   -System Library Settings                                             *
*     RTOS Type - MicroC/OS-II                                           *
*     Periodic System Timer                                              *
*   -Know Issues                                                         *
*     If this design is run on the ISS, terminal output will take several*
*     minutes per iteration.                                             *
**************************************************************************/


#include <stdio.h>
#include "includes.h"

#define taskcase1
#define debug
#define  TASK_STK_SIZE                 512       /* Size of each task's stacks (# of WORDs)            */
#define  N_TASKS                        3       /* Number of identical tasks                          */
struct period{
    int exeTime;
    int period;
};
char CtxSwMessage[CtxSwMessageSize][100];
int CtxSwMessageTop = 0;
OS_EVENT     *printCtxSwMbox;                               /* Message mailboxes for Tasks #4 and #5         */
OS_EVENT* R1,*R2;
OS_STK        TaskStk[N_TASKS][TASK_STK_SIZE];        /* Tasks stacks                                  */
OS_STK        TaskStartStk[TASK_STK_SIZE];
OS_STK        TaskPrintCtxSwStk[TASK_STK_SIZE];
struct period TaskData[N_TASKS];                      /* Parameters to pass to each task               */

void  TaskStart (void *pdata);
void Task(void *pdata);
static  void  TaskStartCreateTasks (void);
void printCtxSwMessage();
void  Task1(void *data);                       /* Function prototypes of tasks                  */
void  Task2(void *data);                       /* Function prototypes of tasks                  */
void  Task3(void *data);                       /* Function prototypes of tasks                  */


/* The main function creates two task and starts multi-tasking */
int main(void)
{
   INT8U err1,err2;
  /*
  OSTaskCreateExt(task1,
                  NULL,
                  (void *)&task1_stk[TASK_STACKSIZE-1],
                  TASK1_PRIORITY,
                  TASK1_PRIORITY,
                  task1_stk,
                  TASK_STACKSIZE,
                  NULL,
                  0);
    */
	OSInit();
    R1 = OSMutexCreate(1,&err1);
    R2 = OSMutexCreate(2,&err2);
    OSTaskCreate(TaskStart, (void *)0, &TaskStartStk[TASK_STK_SIZE - 1], 0);
    OSStart();
  return 0;
}

void  TaskStart (void *pdata)
{

    char       s[100];
    INT16S     key;
    int i;

    pdata = pdata;                                         /* Prevent compiler warning                 */



    TaskStartCreateTasks();                                /* Create all the application tasks         */
    OSTimeSet(0);

    for (i=0;;i++) {

        #ifdef debug
    	//printf("for enter taskStart\n");
        #endif
        //TaskStartDisp();                                  /* Update the display                       */
        if(i!=0)printCtxSwMessage();




        OSCtxSwCtr = 0;                                    /* Clear context switch counter             */
        //OSTimeDlyHMSM(0, 0, 3, 0);                         /* Wait one second                          */
        OSTimeDly (1000);  // delay and wait (P-C) times
    }
}

static  void  TaskStartCreateTasks (void)
{
    INT8U  i;
    INT8U err;

    OSTaskCreate(Task1,(void*)0 , &TaskStk[0][TASK_STK_SIZE - 1],3 );
    OSTaskCreate(Task2, (void *)0, &TaskStk[1][TASK_STK_SIZE - 1],4 );

    OSTaskCreate(Task3, (void *)0, &TaskStk[2][TASK_STK_SIZE - 1],5 );

}
void printCtxSwMessage(){
        static int i=0;
        //OSMboxPend(printCtxSwMbox,0,0);
        for(;i<CtxSwMessageTop;i++)
        printf("%s",CtxSwMessage[i]);
}



///////////////////      case 1
#ifdef taskcase1
void Task1(void *pdata)
{
    int start=8;
    int startDelayTime;
    INT8U err;
    OSTCBCur->period=3;// set the period
    pdata = pdata;
    startDelayTime = start - OSTime;
    if(startDelayTime > 0) OSTimeDly(startDelayTime);


    OSTCBCur->compTime=2;
    while(OSTCBCur->compTime>0){}

    OSTCBCur->compTime=2;
    OSMutexPend(R1,0,&err);
    while(OSTCBCur->compTime>0){}

    OSMutexPend(R2,0,&err);
    OSTCBCur->compTime=2;
    while(OSTCBCur->compTime>0){}
    OSMutexPost(R2);
    OSMutexPost(R1);

    OSTaskDel(OS_PRIO_SELF);
}
void Task2(void *pdata)
{
    int start=4;
    INT8U err;
    //OSTCBCur->period=3;// set the period
    int startDelayTime;
    startDelayTime = start - OSTime;
    if(startDelayTime > 0) OSTimeDly(startDelayTime);

    pdata = pdata;


    OSTCBCur->compTime=2;
    while(OSTCBCur->compTime>0){}

    OSTCBCur->compTime=5;
    OSMutexPend(R2,0,&err);
    while(OSTCBCur->compTime>0){}
    OSMutexPost(R2);


    OSTaskDel(OS_PRIO_SELF);
}

void Task3(void *pdata)
{
    int start=0;
    INT8U err;
    int startDelayTime;
    pdata = pdata;
    startDelayTime = start - OSTime;
    sprintf(&CtxSwMessage[CtxSwMessageTop++],"%5d start  compTime %d  (OSPrioCur=%d, OSTCBCur->OSTCBPrio %d)\n",(int)OSTime,(int)OSTCBCur->compTime,(int)OSPrioCur,(int)OSTCBCur->OSTCBPrio);

    if(startDelayTime > 0) OSTimeDly(startDelayTime);

    OSTimeDly(0);


    OSTCBCur->compTime=2;
    while(OSTCBCur->compTime>0){}

    OSTCBCur->compTime=7;
    OSMutexPend(R1,0,&err);
    while(OSTCBCur->compTime>0){}
    OSMutexPost(R1);


    OSTaskDel(OS_PRIO_SELF);
}

#else

void Task1(void *pdata)
{
    int start=9;
    int startDelayTime;
    INT8U err;
    pdata = pdata;
    startDelayTime = start - OSTime;
    if(startDelayTime > 0) OSTimeDly(startDelayTime);


    OSTCBCur->compTime=2;
    sprintf(&CtxSwMessage[CtxSwMessageTop++],"%5d start  compTime %d  (OSPrioCur=%d, OSTCBCur->OSTCBPrio %d)\n",(int)OSTime,(int)OSTCBCur->compTime,(int)OSPrioCur,(int)OSTCBCur->OSTCBPrio);
    while(OSTCBCur->compTime>0){}
    sprintf(&CtxSwMessage[CtxSwMessageTop++],"%5d end  compTime  %d (OSPrioCur=%d, OSTCBCur->OSTCBPrio %d)\n",(int)OSTime,(int)OSTCBCur->compTime,(int)OSPrioCur,(int)OSTCBCur->OSTCBPrio);


    OSTCBCur->compTime=2;

    OSMutexPend(R1,0,&err);
    sprintf(&CtxSwMessage[CtxSwMessageTop++],"%5d  start compTime %d  (OSPrioCur=%d, OSTCBCur->OSTCBPrio %d)\n",(int)OSTime,(int)OSTCBCur->compTime,(int)OSPrioCur,(int)OSTCBCur->OSTCBPrio);

    while(OSTCBCur->compTime>0){}
    sprintf(&CtxSwMessage[CtxSwMessageTop++],"%5d end compTime  %d (OSPrioCur=%d, OSTCBCur->OSTCBPrio %d)\n",(int)OSTime,(int)OSTCBCur->compTime,(int)OSPrioCur,(int)OSTCBCur->OSTCBPrio);


    OSTCBCur->compTime=2;

    OSMutexPend(R1,0,&err);
    sprintf(&CtxSwMessage[CtxSwMessageTop++],"%5d start compTime %d  (OSPrioCur=%d, OSTCBCur->OSTCBPrio %d)\n",(int)OSTime,(int)OSTCBCur->compTime,(int)OSPrioCur,(int)OSTCBCur->OSTCBPrio);

    while(OSTCBCur->compTime>0){}

    OSMutexPost(R1);
    sprintf(&CtxSwMessage[CtxSwMessageTop++],"%5d end compTime  %d (OSPrioCur=%d, OSTCBCur->OSTCBPrio %d)\n",(int)OSTime,(int)OSTCBCur->compTime,(int)OSPrioCur,(int)OSTCBCur->OSTCBPrio);

    OSTCBCur->compTime=3;
    sprintf(&CtxSwMessage[CtxSwMessageTop++],"%5d start compTime %d  (OSPrioCur=%d, OSTCBCur->OSTCBPrio %d)\n",(int)OSTime,(int)OSTCBCur->compTime,(int)OSPrioCur,(int)OSTCBCur->OSTCBPrio);

    while(OSTCBCur->compTime>0){}

    OSMutexPost(R2);
    sprintf(&CtxSwMessage[CtxSwMessageTop++],"%5d end compTime  %d (OSPrioCur=%d, OSTCBCur->OSTCBPrio %d)\n",(int)OSTime,(int)OSTCBCur->compTime,(int)OSPrioCur,(int)OSTCBCur->OSTCBPrio);

    OSTaskDel(OS_PRIO_SELF);
}
void Task2(void *pdata)
{
    int start=15;
    INT8U err;
    //OSTCBCur->period=3;// set the period
    int startDelayTime;
    startDelayTime = start - OSTime;
    if(startDelayTime > 0) OSTimeDly(startDelayTime);

    pdata = pdata;


    OSTCBCur->compTime=2;
    sprintf(&CtxSwMessage[CtxSwMessageTop++],"%5d start compTime %d  (OSPrioCur=%d, OSTCBCur->OSTCBPrio %d)\n",(int)OSTime,(int)OSTCBCur->compTime,(int)OSPrioCur,(int)OSTCBCur->OSTCBPrio);

    while(OSTCBCur->compTime>0){}
    sprintf(&CtxSwMessage[CtxSwMessageTop++],"%5d end compTime  %d (OSPrioCur=%d, OSTCBCur->OSTCBPrio %d)\n",(int)OSTime,(int)OSTCBCur->compTime,(int)OSPrioCur,(int)OSTCBCur->OSTCBPrio);

    OSTCBCur->compTime=6;

    OSMutexPend(R1,0,&err);
    sprintf(&CtxSwMessage[CtxSwMessageTop++],"%5d start compTime %d  (OSPrioCur=%d, OSTCBCur->OSTCBPrio %d)\n",(int)OSTime,(int)OSTCBCur->compTime,(int)OSPrioCur,(int)OSTCBCur->OSTCBPrio);

    while(OSTCBCur->compTime>0){}
    sprintf(&CtxSwMessage[CtxSwMessageTop++],"%5d end compTime  %d (OSPrioCur=%d, OSTCBCur->OSTCBPrio %d)\n",(int)OSTime,(int)OSTCBCur->compTime,(int)OSPrioCur,(int)OSTCBCur->OSTCBPrio);


    OSTCBCur->compTime=2;

    OSMutexPend(R2,0,&err);
    sprintf(&CtxSwMessage[CtxSwMessageTop++],"%5d start compTime %d  (OSPrioCur=%d, OSTCBCur->OSTCBPrio %d)\n",(int)OSTime,(int)OSTCBCur->compTime,(int)OSPrioCur,(int)OSTCBCur->OSTCBPrio);

    while(OSTCBCur->compTime>0){}

    OSMutexPost(R2);
    sprintf(&CtxSwMessage[CtxSwMessageTop++],"%5d end compTime  %d (OSPrioCur=%d, OSTCBCur->OSTCBPrio %d)\n",(int)OSTime,(int)OSTCBCur->compTime,(int)OSPrioCur,(int)OSTCBCur->OSTCBPrio);

    OSTCBCur->compTime=2;
    sprintf(&CtxSwMessage[CtxSwMessageTop++],"%5d start compTime %d  (OSPrioCur=%d, OSTCBCur->OSTCBPrio %d)\n",(int)OSTime,(int)OSTCBCur->compTime,(int)OSPrioCur,(int)OSTCBCur->OSTCBPrio);

    while(OSTCBCur->compTime>0){}

    OSMutexPost(R1);
    sprintf(&CtxSwMessage[CtxSwMessageTop++],"%5d end compTime  %d (OSPrioCur=%d, OSTCBCur->OSTCBPrio %d)\n",(int)OSTime,(int)OSTCBCur->compTime,(int)OSPrioCur,(int)OSTCBCur->OSTCBPrio);


    OSTaskDel(OS_PRIO_SELF);
}
void Task3(void *pdata)
{
    int start=0;
    INT8U err;
    //OSTCBCur->period=3;// set the period
    int startDelayTime;
    startDelayTime = start - OSTime;
    if(startDelayTime > 0) OSTimeDly(startDelayTime);

    pdata = pdata;


    OSTCBCur->compTime=2;
    sprintf(&CtxSwMessage[CtxSwMessageTop++],"%5d start compTime %d  (OSPrioCur=%d, OSTCBCur->OSTCBPrio %d)\n",(int)OSTime,(int)OSTCBCur->compTime,(int)OSPrioCur,(int)OSTCBCur->OSTCBPrio);

    while(OSTCBCur->compTime>0){}
    sprintf(&CtxSwMessage[CtxSwMessageTop++],"%5d end compTime  %d (OSPrioCur=%d, OSTCBCur->OSTCBPrio %d)\n",(int)OSTime,(int)OSTCBCur->compTime,(int)OSPrioCur,(int)OSTCBCur->OSTCBPrio);

    OSTCBCur->compTime=7;

    OSMutexPend(R1,0,&err);
    sprintf(&CtxSwMessage[CtxSwMessageTop++],"%5d start compTime %d  (OSPrioCur=%d, OSTCBCur->OSTCBPrio %d)\n",(int)OSTime,(int)OSTCBCur->compTime,(int)OSPrioCur,(int)OSTCBCur->OSTCBPrio);

    while(OSTCBCur->compTime>0){}
    OSMutexPost(R1);

    sprintf(&CtxSwMessage[CtxSwMessageTop++],"%5d end compTime  %d (OSPrioCur=%d, OSTCBCur->OSTCBPrio %d)\n",(int)OSTime,(int)OSTCBCur->compTime,(int)OSPrioCur,(int)OSTCBCur->OSTCBPrio);



    OSTaskDel(OS_PRIO_SELF);
}
#endif
