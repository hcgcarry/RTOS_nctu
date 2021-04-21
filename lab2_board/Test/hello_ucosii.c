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

#define debug
#define  TASK_STK_SIZE                 512       /* Size of each task's stacks (# of WORDs)            */
#define  N_TASKS                        2       /* Number of identical tasks                          */
struct period{
    int exeTime;
    int period;
};
char CtxSwMessage[CtxSwMessageSize][100];
int CtxSwMessageTop = 0;
OS_EVENT     *printCtxSwMbox;                               /* Message mailboxes for Tasks #4 and #5         */

OS_STK        TaskStk[N_TASKS][TASK_STK_SIZE];        /* Tasks stacks                                  */
OS_STK        TaskStartStk[TASK_STK_SIZE];
OS_STK        TaskPrintCtxSwStk[TASK_STK_SIZE];
struct period TaskData[N_TASKS];                      /* Parameters to pass to each task               */

void ArgumentSet(void);
void  TaskStart (void *pdata);
void Task(void *pdata);
static  void  TaskStartCreateTasks (void);
void printCtxSwMessage();


/* The main function creates two task and starts multi-tasking */
int main(void)
{
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
    OSTCBPrioTbl[20]->deadLine = 1002;
	OSTaskCreate(TaskStart, (void *)0, &TaskStartStk[TASK_STK_SIZE - 1], 0);
    OSTCBPrioTbl[0]->deadLine = 1;
    TaskStartCreateTasks();                                /* Create all the application tasks         */
      ArgumentSet();
  OSStart();
  return 0;
}

void  TaskStart (void *pdata)
{

    char       s[100];
    INT16S     key;
    int i;

    pdata = pdata;                                         /* Prevent compiler warning                 */




    OSTimeSet(0);

    for (i=0;;i++) {

        #ifdef debug
    	printf("for enter taskStart\n");
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
    /*
    err=OSTaskCreate(printCtxSwMessage, (void *)0, &TaskPrintCtxSwStk[TASK_STK_SIZE-1], 1);
    if(err!=OS_NO_ERR){
        printf("create task printCtxSwMessage errorno %d \n",err);
    }
    */
    for (i = 0; i < N_TASKS; i++) {                        /* Create N_TASKS identical tasks           */
        err=OSTaskCreate(Task, (void *)&TaskData[i], &TaskStk[i][TASK_STK_SIZE - 1],i+1 );
        if(err!=OS_NO_ERR){
            printf("create task %d errorno %d \n",i,err);
        }
    }
}
void Task(void *pdata)
{
    int start;  //the start time
    int end;   //the end time
    int toDelay;
    struct period *tmpPdata = (struct period*)pdata;
    int c = OSTCBCur->compTime;
    ///OS_ENTER_CRITICAL();

    //OS_EXIT_CRITICAL();

    start=0;
    while(1)
    {
        #ifdef debug

        //printf("---start OSPrioCur: %d , real C %d,period %d\n",OSPrioCur,OSTCBCur->compTime,OSTCBCur->period);
        #endif
        while(OSTCBCur->compTime>0)  //C ticks
        {
        // do nothing
        	//end=OSTimeGet() ;
        	  //printf("time:%d task:%d exceed deadline\n",end,OSTCBCur->OSTCBPrio);
        }
        end=OSTimeGet() ; // end time
        toDelay=(OSTCBCur->period)-(end-start) ;
        if(end > start+OSTCBCur->period){
            printf("time:%d task:%d exceed deadline\n",start+OSTCBCur->period,OSTCBCur->OSTCBPrio);
        }
        #ifdef debug
        //printf(" end task,start time %d,end time %d,to delay %d,prio:%d,deadLine %d\n",(int)start,(int)end,(int)toDelay,(int)OSTCBCur->OSTCBPrio,(int)OSTCBCur->deadLine);
        #endif
        //OS_ENTER_CRITICAL();
        start+=(OSTCBCur->period) ;  // next start time
        OSTCBCur->compTime=c ;// reset the counter (c ticks for computation)
        OSTCBCur->deadLine=start + OSTCBCur->period ;// reset the deadline
        //OS_EXIT_CRITICAL();
        OSTimeDly (toDelay);  // delay and wait (P-C) times
    }
}

void printCtxSwMessage(){
        static int i=0;
        //OSMboxPend(printCtxSwMbox,0,0);
        for(;i<CtxSwMessageTop;i++)
        printf("%s",CtxSwMessage[i]);
}

/******************************************************************************
*                                                                             *
* License Agreement                                                           *
*                                                                             *
* Copyright (c) 2004 Altera Corporation, San Jose, California, USA.           *
* All rights reserved.                                                        *
*                                                                             *
* Permission is hereby granted, free of charge, to any person obtaining a     *
* copy of this software and associated documentation files (the "Software"),  *
* to deal in the Software without restriction, including without limitation   *
* the rights to use, copy, modify, merge, publish, distribute, sublicense,    *
* and/or sell copies of the Software, and to permit persons to whom the       *
* Software is furnished to do so, subject to the following conditions:        *
*                                                                             *
* The above copyright notice and this permission notice shall be included in  *
* all copies or substantial portions of the Software.                         *
*                                                                             *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  *
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,    *
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE *
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER      *
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING     *
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER         *
* DEALINGS IN THE SOFTWARE.                                                   *
*                                                                             *
* This agreement shall be governed in all respects by the laws of the State   *
* of California and by the laws of the United States of America.              *
* Altera does not recommend, suggest or require that this reference design    *
* file be used in conjunction or combination with any other product.          *
******************************************************************************/

void ArgumentSet(void){
    OS_TCB* ptcb;
    ptcb = OSTCBList;
    while(ptcb->OSTCBPrio==1 || ptcb->OSTCBPrio==2 || ptcb->OSTCBPrio==3){
        printf("Priority: %d set argument\n", ptcb->OSTCBPrio);
        if(ptcb->OSTCBPrio==1){
            ptcb->compTime = 1;
            ptcb->period = 3;
            ptcb->deadLine = 3;
        }
        else if(ptcb->OSTCBPrio==2){
            ptcb->compTime = 3;
            ptcb->period = 5;
            ptcb->deadLine = 5;
        }
        else if(ptcb->OSTCBPrio==3){
            ptcb->compTime = 2;
            ptcb->period = 10;
            ptcb->deadLine = 10;
        }
        ptcb = ptcb->OSTCBNext;
    }
}
