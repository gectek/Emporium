#ifndef TASK_H
#define TASK_H
#include "../../cpu/idt.h"
typedef enum {
    TASK_CREATED,
    TASK_READY,
    TASK_RUNNING,
    TASK_WAITING,
    TASK_DEAD
}task_state_t;
typedef struct pcb{
    registers_t *regs;
    struct pcb *next;
    uintptr_t pd_addr;
    uint32_t PID;
    task_state_t state;
    uintptr_t kernel_stack;
}pcb_t;
pcb_t *create_task(void (*task_entry)(), uintptr_t pd_addr);
void task_exit();
void task_reaper();
#endif  
