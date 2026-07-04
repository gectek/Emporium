#include "../../cpu/idt.h"
#include "task.h"
void scheduler_init();
void add_task(pcb_t *task);
registers_t* schedule(registers_t *regs);
