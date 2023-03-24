#include "os.h"
#include <stdint.h>

volatile task_t taskA;
volatile task_t taskB;

volatile task_t *sched_current_task;
volatile task_t *sched_next_task;
volatile task_t *running_task;

sem_t evt_sem;

int cntA;
int cntB;

void TaskA(void)
{
  SysCallSemWait(&evt_sem);
  while(1) {
    cntA++;
    SysCallSwitchContext();
  }
}

void TaskB(void)
{
  while(1) {
    cntB++;
    SysCallSwitchContext();
    if (cntB > 100)
      SysCallSemSignal(&evt_sem);
  }
}

void ChangeRunningTask(void)
{
  running_task = sched_next_task;

  // task schedulling: round-robin for 2 tasks
  task_t *tmp = sched_current_task;
  sched_current_task = sched_next_task;
  sched_next_task = tmp;
}

void SVC_SysCall(uint32_t *sp)
{
  uint32_t saved_pc = sp[6];
  saved_pc -= 2; // svc opcode is 16 bit, so get it by -2
  uint16_t svc_opcode = *((uint16_t *)saved_pc);
  uint8_t svcnum = (svc_opcode & 0x00ff);
  uint32_t saved_R0 = sp[0]; // it's first arg by ABI

  switch(svcnum) {
  case 1:
    sem_wait((sem_t *)saved_R0);
    break;
  case 2:
    sem_signal((sem_t *)saved_R0);
    break;
  case 42:
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
    break;
  }
}


int main(void)
{
  taskA.stack[STACK_SIZE-1] = 0x01000000UL; // xPSR
  taskB.stack[STACK_SIZE-1] = 0x01000000UL; // xPSR

  taskA.stack[STACK_SIZE-2] = (unsigned int)TaskA | 1; // PC for return from interrupt
  taskB.stack[STACK_SIZE-2] = (unsigned int)TaskB | 1; // PC for return from interrupt

  // ################## Fill registers with stub values only for debug #####################
  //taskA.stack[STACK_SIZE-3] = LR; // old saved LR
  taskA.stack[STACK_SIZE-4] = 12; // saved R12
  taskA.stack[STACK_SIZE-5] = 3; // saved R3
  taskA.stack[STACK_SIZE-6] = 2; // saved R2
  taskA.stack[STACK_SIZE-7] = 1; // saved R1
  taskA.stack[STACK_SIZE-8] = 0; // saved R0
  taskA.stack[STACK_SIZE-9] = 11; // saved R11
  taskA.stack[STACK_SIZE-10] = 10; // saved R10
  taskA.stack[STACK_SIZE-11] = 9; // saved R9
  taskA.stack[STACK_SIZE-12] = 8; // saved R8
  taskA.stack[STACK_SIZE-13] = 7; // saved R7
  taskA.stack[STACK_SIZE-14] = 6; // saved R6
  taskA.stack[STACK_SIZE-15] = 5; // saved R5
  taskA.stack[STACK_SIZE-16] = 4; // saved R4

  //taskB.stack[STACK_SIZE-3] = LR; // old saved LR
  taskB.stack[STACK_SIZE-4] = 12; // saved R12
  taskB.stack[STACK_SIZE-5] = 3; // saved R3
  taskB.stack[STACK_SIZE-6] = 2; // saved R2
  taskB.stack[STACK_SIZE-7] = 1; // saved R1
  taskB.stack[STACK_SIZE-8] = 0; // saved R0
  taskB.stack[STACK_SIZE-9] = 11; // saved R11
  taskB.stack[STACK_SIZE-10] = 10; // saved R10
  taskB.stack[STACK_SIZE-11] = 9; // saved R9
  taskB.stack[STACK_SIZE-12] = 8; // saved R8
  taskB.stack[STACK_SIZE-13] = 7; // saved R7
  taskB.stack[STACK_SIZE-14] = 6; // saved R6
  taskB.stack[STACK_SIZE-15] = 5; // saved R5
  taskB.stack[STACK_SIZE-16] = 4; // saved R4
  // ######################################################################################

  taskA.sp = &taskA.stack[STACK_SIZE-16];
  taskB.sp = &taskB.stack[STACK_SIZE-16];

  sched_next_task = &taskB;
  sched_current_task = &taskA;
  running_task = sched_current_task;

  sem_init(&evt_sem);

  __NVIC_EnableIRQ(SVCall_IRQn);
  NVIC_SetPriority(PendSV_IRQn, 0xff);

  /* Switch to unprivileged mode with PSP stack */
  __set_PSP((uint32_t)(&(taskA.stack[STACK_SIZE-16])));
  __set_CONTROL(0x03);

  TaskA();

  while(1);
}
