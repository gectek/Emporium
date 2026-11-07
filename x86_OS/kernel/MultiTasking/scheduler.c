#include "scheduler.h"
#include "task.h"
#include "../../cpu/gdt.h"
#include "heap.h"
#include "stdfun.h"
#include "serial.h"
pcb_t *current_task;
pcb_t *task_list;
void scheduler_init()
{
    current_task = NULL;    
    task_list = NULL;

    pcb_t *primary_task = (pcb_t*)k_malloc(sizeof(pcb_t));  //Allocating memory for primary task pcb
    //mem_set(primary_task, 0, sizeof(pcb_t));    //Emptying out the pcb *this here is a problem*
    uint32_t cr3;       //cr3 contains the page_directory address of current task
    __asm__ volatile("mov %%cr3, %0":"=r"(cr3));    //accessing cr3 register

    primary_task->PID = 0;                  
    primary_task->state = TASK_RUNNING;
    primary_task->pd_addr = (uintptr_t)cr3;
    primary_task->kernel_stack = (uintptr_t)k_malloc(16384);//Allocating space for kernel_stack
    uint32_t *canary = (uint32_t*)primary_task->kernel_stack;
    *canary = 0xDEADBEEF;
    print("kernel_stack: %x\n", primary_task->kernel_stack);
    task_list = primary_task;
    current_task = primary_task;
    primary_task -> next = task_list;
    print("Scheduler_init completed\n");
}
void add_task(pcb_t *task)
{
    if(task_list == NULL)
    {
        task_list = task;
        task->next = task;
    }
    else {
        print("task : %x\n", task);
        pcb_t *track = task_list;
        while(track->next != task_list)
        {
            track = track->next;
        }
        track->next = task;
        task->next = task_list;
    } 
    print("Task : %d(PID) added\n", task->PID);
}
registers_t *schedule(registers_t *regs)
{
    if(current_task == NULL)
        return regs;

    //CHECKING FOR BUFFER OVERFLOW(FOR DEBUGGING)
    uint32_t *canary = (uint32_t*)current_task->kernel_stack;
    if(*canary != 0xDEADBEEF)
    {
        serial_write(SERIAL_COM1_BASE, "STACK OVERFLOW DETECTED...!!!\n");
        print("STACK_OVERFLOW...! PID: %d\n\n\n", current_task->PID);
        for(;;);
    }
    //CHECKING FOR BUFFER OVERFLOW
    
    //SAVING THE CONTEXT AND CHANGING THE TASK
    current_task->regs = regs; //Saving the current context to pcb of current task
    /*
    current_task = current_task->next;  //Changing the task to next one
    */
    if(current_task->state == TASK_READY)
    {
        
    }



    current_task->state = TASK_RUNNING;
    tss_set_esp0(current_task->kernel_stack + 16384); //Setting the esp0 in tss so that CPU can access the kernel_stack 
    __asm__ volatile("mov %0, %%cr3": : "r"(current_task->pd_addr)); //Reloading the value in cr3 wiz, New page directory (No need to do tlb flush as reloading to cr3 will automatically flush it)
    regs = current_task->regs; //Restoring the context
    return regs;
}
