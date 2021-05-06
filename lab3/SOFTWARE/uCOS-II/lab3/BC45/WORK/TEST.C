/*
*********************************************************************************************************
*                                                uC/OS-II
*                                          The Real-Time Kernel
*
*                           (c) Copyright 1992-2002, Jean J. Labrosse, Weston, FL
*                                           All Rights Reserved
*
*                                               EXAMPLE #1
*********************************************************************************************************
*/

#include "includes.h"

/*
*********************************************************************************************************
*                                               CONSTANTS
*********************************************************************************************************
*/

#define  TASK_STK_SIZE                 512       /* Size of each task's stacks (# of WORDs)            */
#define  N_TASKS                        2       /* Number of identical tasks                          */
//#define debug

/*
*********************************************************************************************************
*                                               VARIABLES
*********************************************************************************************************
*/
OS_EVENT* R1,*R2;
struct period{
    int exeTime;
    int period;
};
char CtxSwMessage[CtxSwMessageSize][50];
int CtxSwMessageTop = 0;
OS_EVENT     *printCtxSwMbox;                               /* Message mailboxes for Tasks #4 and #5         */

OS_STK        TaskStk[N_TASKS][TASK_STK_SIZE];        /* Tasks stacks                                  */
OS_STK        TaskStartStk[TASK_STK_SIZE];
OS_STK        TaskPrintCtxSwStk[TASK_STK_SIZE];
struct period TaskData[N_TASKS];                      /* Parameters to pass to each task               */

//#define taskcase1

/*
*********************************************************************************************************
*                                           FUNCTION PROTOTYPES
*********************************************************************************************************
*/

        void printCtxSwMessage();
        void  Task1(void *data);                       /* Function prototypes of tasks                  */
        void  Task2(void *data);                       /* Function prototypes of tasks                  */
        void  Task3(void *data);                       /* Function prototypes of tasks                  */
        void  TaskStart(void *data);                  /* Function prototypes of Startup task           */
static  void  TaskStartCreateTasks(void);
static  void  TaskStartDispInit(void);
static  void  TaskStartDisp(void);
///

/*$PAGE*/
/*
*********************************************************************************************************
*                                                MAIN
*********************************************************************************************************
*/

void  main (void)
{
    INT8U err1,err2;

    OSInit();                                              /* Initialize uC/OS-II                      */

    PC_DOSSaveReturn();                                    /* Save environment to return to DOS        */
    PC_VectSet(uCOS, OSCtxSw);                             /* Install uC/OS-II's context switch vector */


    R1 = OSMutexCreate(1,&err1);
    R2 = OSMutexCreate(2,&err2);
    OSTaskCreate(TaskStart, (void *)0, &TaskStartStk[TASK_STK_SIZE - 1], 0);
    OSStart();                                             /* Start multitasking                       */
}


/*
*********************************************************************************************************
*                                              STARTUP TASK
*********************************************************************************************************
*/
void  TaskStart (void *pdata)
{
#if OS_CRITICAL_METHOD == 3                                /* Allocate storage for CPU status register */
    OS_CPU_SR  cpu_sr;
#endif
    char       s[100];
    INT16S     key;


    pdata = pdata;                                         /* Prevent compiler warning                 */

    //TaskStartDispInit();                                   /* Initialize the display                   */

    OS_ENTER_CRITICAL();
    PC_VectSet(0x08, OSTickISR);                           /* Install uC/OS-II's clock tick ISR        */
    PC_SetTickRate(1u);                      /* Reprogram tick rate                      */
    OS_EXIT_CRITICAL();

    //OSStatInit();                                          /* Initialize uC/OS-II's statistics         */
    printCtxSwMbox= OSMboxCreate((void *)0);                     /* Create 2 message mailboxes               */

    TaskStartCreateTasks();                                /* Create all the application tasks         */
    OSTimeSet(0);
    for (;;) {
        #ifdef debug
        printf("--- enter taskStart\n");
        #endif
        //TaskStartDisp();                                  /* Update the display                       */
        printCtxSwMessage();


        if (PC_GetKey(&key) == TRUE) {                     /* See if key has been pressed              */
            if (key == 0x1B) {                             /* Yes, see if it's the ESCAPE key          */
                PC_DOSReturn();                            /* Return to DOS                            */
            }
        }

        OSCtxSwCtr = 0;                                    /* Clear context switch counter             */
        OSTimeDlyHMSM(0, 0, 1, 0);                         /* Wait one second                          */
    }
}




/*$PAGE*/
/*
*********************************************************************************************************
*                                             CREATE TASKS
*********************************************************************************************************
*/

static  void  TaskStartCreateTasks (void)
{
    INT8U  i;
    INT8U err;

    OSTaskCreate(Task1,(void*)0 , &TaskStk[0][TASK_STK_SIZE - 1],3 );
    OSTaskCreate(Task2, (void *)0, &TaskStk[1][TASK_STK_SIZE - 1],4 );
    #ifdef taskcase1
    OSTaskCreate(Task3, (void *)0, &TaskStk[2][TASK_STK_SIZE - 1],5 );
    #endif
}

/*
*********************************************************************************************************
*                                                  TASKS
*********************************************************************************************************
*/

/*
void Task(void *pdata)
{
    int start;  //the start time 
    int end;   //the end time
    int toDelay;
    struct period *tmpPdata = (struct period*)pdata;
    int c = tmpPdata->exeTime;
    OS_ENTER_CRITICAL();
    OSTCBCur->compTime=c;// set the counter (c ticks for computation)
    OSTCBCur->period=tmpPdata->period;// set the period
    
    OS_EXIT_CRITICAL();
        
    start=OSTimeGet(); 
    while(1)
    {
        #ifdef debug
        printf("---real c: %d ,real period %d\n",OSTCBCur->compTime,OSTCBCur->period);
        printf("---c: %d ,period %d\n",tmpPdata->exeTime,tmpPdata->period);
        printf("---start OSPrioCur: %d , real C %d,period %d\n",OSPrioCur,OSTCBCur->compTime,OSTCBCur->period);
        #endif
        while(OSTCBCur->compTime>0)  //C ticks
        {
        // do nothing
        }
        end=OSTimeGet() ; // end time
        if(end > start+OSTCBCur->period){
            printf("time:%d task:%d exceed deadline\n",end,OSTCBCur->OSTCBPrio);
        }
        toDelay=(OSTCBCur->period)-(end-start) ;
        start=start+(OSTCBCur->period) ;  // next start time
        #ifdef debug
        printf("---end OSPrioCur: %d ,real C: %d,period %d,end time:%d,toDelay:%d,next start:%d\n",OSPrioCur,OSTCBCur->compTime,OSTCBCur->period,end,toDelay,start);
        #endif
        OS_ENTER_CRITICAL();
        OSTCBCur->compTime=c ;// reset the counter (c ticks for computation)
        OS_EXIT_CRITICAL();
        OSTimeDly (toDelay);  // delay and wait (P-C) times
    }
}
*/

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
    int start=5;
    int startDelayTime;
    INT8U err;
    pdata = pdata;
    startDelayTime = start - OSTime;
    if(startDelayTime > 0) OSTimeDly(startDelayTime);
    

    OSTCBCur->compTime=2;
    while(OSTCBCur->compTime>0){}

    OSTCBCur->compTime=3;
    OSMutexPend(R2,0,&err);
    while(OSTCBCur->compTime>0){}

    OSTCBCur->compTime=3;
    OSMutexPend(R1,0,&err);
    while(OSTCBCur->compTime>0){}
    OSMutexPost(R1);

    OSTCBCur->compTime=3;
    while(OSTCBCur->compTime>0){}
    OSMutexPost(R2);

    OSTaskDel(OS_PRIO_SELF);
}
void Task2(void *pdata)
{
    int start=0;
    INT8U err;
    //OSTCBCur->period=3;// set the period
    int startDelayTime;
    startDelayTime = start - OSTime;
    if(startDelayTime > 0) OSTimeDly(startDelayTime);
        
    pdata = pdata;
    

    OSTCBCur->compTime=2;
    while(OSTCBCur->compTime>0){}

    OSTCBCur->compTime=6;
    OSMutexPend(R1,0,&err);
    while(OSTCBCur->compTime>0){}

    OSTCBCur->compTime=2;
    OSMutexPend(R2,0,&err);
    while(OSTCBCur->compTime>0){}
    OSMutexPost(R2);

    OSTCBCur->compTime=2;
    while(OSTCBCur->compTime>0){}
    OSMutexPost(R1);


    OSTaskDel(OS_PRIO_SELF);
}
#endif