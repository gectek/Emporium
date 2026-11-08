#include "../../cpu/idt.h"
#include "task.h"
void scheduler_init();
void add_task(pcb_t *task);
pcb_t* dequeue_task();
void enqueue_task();
extern pcb_t *idle_task;
registers_t* schedule(registers_t *regs);
