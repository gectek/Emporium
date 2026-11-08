#include "scheduler.h"
#include "task.h"
#include "../../cpu/gdt.h"
#include "heap.h"
#include "stdfun.h"
#include "serial.h"
pcb_t *current_task;
pcb_t *task_queue_head;
pcb_t *task_queue_tail;
pcb_t* idle_task;
void idle_task_fn()
{
    while(1) __asm__ volatile("sti; hlt");
}

void scheduler_init()
{
    current_task = NULL;    
    task_queue_head = NULL;

    pcb_t *primary_task = (pcb_t*)k_malloc(sizeof(pcb_t));  //Allocating memory for primary task pcb
    //mem_set(primary_task, 0, sizeof(pcb_t));    //Emptying out the pcb ***this here is a problem*
    uint32_t cr3;       //cr3 contains the page_directory address of current task
    __asm__ volatile("mov %%cr3, %0":"=r"(cr3));    //accessing cr3 register

    primary_task->PID = 0;                  
    primary_task->state = TASK_RUNNING;
    primary_task->pd_addr = (uintptr_t)cr3;
    primary_task->kernel_stack = (uintptr_t)k_malloc(16384);//Allocating space for kernel_stack
    uint32_t *canary = (uint32_t*)primary_task->kernel_stack;
    *canary = 0xDEADBEEF;
    print("kernel_stack: %x\n", primary_task->kernel_stack);

    current_task = primary_task;
    idle_task = create_task(idle_task_fn, cr3);
    idle_task->state = TASK_READY;
    print("Scheduler_init completed\n");
}
void add_task(pcb_t *task)
{   
    //__asm__ volatile("cli");    //Clearing the interrupt bits to avoid collisions
    if(task_queue_head == NULL)
    {
        task_queue_head = task;
    }
    else {
        task_queue_tail->next = task;
    }
    task_queue_tail = task;
    task_queue_tail->next = NULL;
    //__asm__ volatile("sti");
}
pcb_t *dequeue_task()
{
    pcb_t *t = task_queue_head; 
    if(t){
        task_queue_head = t->next;
        if(!task_queue_head) task_queue_tail=NULL;
        t->next = NULL; 
    }
    return t;
}
registers_t *schedule(registers_t *regs)
{
    if(current_task == NULL)
        return regs;
/*
    //CHECKING FOR BUFFER OVERFLOW(FOR DEBUGGING)
    uint32_t *canary = (uint32_t*)current_task->kernel_stack;
    if(*canary != 0xDEADBEEF)
    {
        serial_write(SERIAL_COM1_BASE, "STACK OVERFLOW DETECTED...!!!\n");
        print("STACK_OVERFLOW...! PID: %d\n\n\n", current_task->PID);
        for(;;);
    }
    //CHECKING FOR BUFFER OVERFLOW(DEBUGGING)
*/
    //SAVING THE CONTEXT AND CHANGING THE TASK
    current_task->regs = regs; //Saving the current context to pcb of current task
    if(current_task->state == TASK_RUNNING)
    {
        current_task->state = TASK_READY;
        add_task(current_task);
    }
    
    pcb_t *next = dequeue_task();
    if(next == NULL) next = idle_task;
    next->state = TASK_RUNNING;
    current_task = next;

    tss_set_esp0(current_task->kernel_stack + 16384); //Setting the esp0 in tss so that CPU can access the kernel_stack 
    __asm__ volatile("mov %0, %%cr3": : "r"(current_task->pd_addr)); //Reloading the value in cr3 wiz, New page directory (No need to do tlb flush as reloading to cr3 will automatically flush it)
    regs = current_task->regs; //Restoring the context
    return regs;
}
