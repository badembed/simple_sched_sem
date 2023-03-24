#include "os.h"
#include <stddef.h>

void sem_init(sem_t *sem)
{
  if (sem == NULL)
    return;
  sem->sem_cnt = 0;
  sem->waited_cnt = 0;
}

void sem_wait(sem_t *sem)
{
  if (sem == NULL)
    return;

  __disable_irq();
  sem->sem_cnt--;
  if (sem->sem_cnt < 0) {
    sem->waited[sem->waited_cnt] = sched_current_task;
    sem->waited_cnt++;
    sched_current_task = sched_next_task;
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
  }
  __enable_irq();
}

void sem_signal(sem_t *sem)
{
  if (sem == NULL)
    return;

  __disable_irq();
  sem->sem_cnt++;
  if (sem->waited_cnt > 0) {
    sem->waited_cnt--;
    sched_next_task = sem->waited[sem->waited_cnt];
    SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
  }
  __enable_irq();
}
