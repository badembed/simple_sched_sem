#include "stm32f746xx.h"

#define STACK_SIZE 256
typedef struct {
  unsigned int sp;
  unsigned int stack[STACK_SIZE];
} task_t;

typedef struct {
  int sem_cnt;
  task_t *waited[10];
  int waited_cnt;
} sem_t;

extern volatile task_t *sched_current_task;
extern volatile task_t *sched_next_task;
extern volatile task_t *running_task;

void sem_init(sem_t *sem);
void sem_wait(sem_t *sem);
void sem_signal(sem_t *sem);

void SysCallSwitchContext(void);
void SysCallSemWait(sem_t *sem);
void SysCallSemSignal(sem_t *sem);
