  .thumb
  .syntax unified
  .cpu cortex-m7

  .extern running_task

  .global ChangeRunningTask
  .global SysTick_Handler
  .global SVC_Handler
  .global SysCallSwitchContext
  .global SysCallSemWait
  .global SysCallSemSignal
  .global PendSV_Handler

  .text
  .align 4

.thumb_func
SVC_Handler:
  mrs	r0, psp   // r0 = psp user mode stack pointer
  B SVC_SysCall

.thumb_func
SysTick_Handler:

.thumb_func
PendSV_Handler:
  cpsid	i // // Disable Interrupts

  mrs	r0, psp   // r0 = psp user mode stack pointer
  sub	r0, #32   // allocate 16 byte on the stack for r4-r11 registers (8 registers)

  // save r4-r11 registers at the stack - stmia  r0!, {r4-r11}
  str r4, [r0]
  str r5, [r0, #4]
  str r6, [r0, #8]
  str r7, [r0, #12]
  str r8, [r0, #16]
  str r9, [r0, #20]
  str r10, [r0, #24]
  str r11, [r0, #28]

  ldr	r2, =running_task // r2 = addess of cur_task pointer
  ldr	r1, [r2]      // r1 = get real task address from cur_task, first value in the task is SP
  str	r0, [r1]      // rewrite/save SP address with new in cur_task

  bl ChangeRunningTask // Update cur_task to new/next task

  ldr	r2, =running_task
  ldr	r1, [r2]
  ldr	r0, [r1]      // r0 = SP for cur_task(new/next task)

  // restore r4-r11 registers from the stack - ldmia	r0!,{r4-r11}
  ldr r4, [r0]
  ldr r5, [r0, #4]
  ldr r6, [r0, #8]
  ldr r7, [r0, #12]
  ldr r8, [r0, #16]
  ldr r9, [r0, #20]
  ldr r10, [r0, #24]
  ldr r11, [r0, #28]
  add r0, #32

  msr	psp, r0

  ldr r0, =0xFFFFFFFD
  cpsie	i // Enable Interrupts
  bx	r0

SysCallSwitchContext:
  svc	#42
  bx lr

SysCallSemWait:
  svc	#1
  bx lr

SysCallSemSignal:
  svc	#2
  bx lr
