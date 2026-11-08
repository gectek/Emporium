/*1. allocate PCB
2. allocate kernel stack
3. set up initial register state
4. set PID
5. set state to TASK_READY
6. add to scheduler's linked list*/
#include "task.h"
#include "../mm/heap.h"
#include "../../libc/stdfun.h"
#include "../../drivers/timer.h"
#include "../../drivers/serial.h"
#include "scheduler.h"
uintptr_t next_pid = 1;
extern pcb_t *current_task;
extern pcb_t *task_queue_head;
pcb_t *zombie_head;
pcb_t *current_zombie;
pcb_t *sleep_queue_head = NULL;

/// HAVE TO CHANGE THE PCB LIST TO DOUBLY LINKED LIST


pcb_t *create_task(void (*task_entry)(), uintptr_t pd_addr) {
    // 1. Allocate the Process Control Block
    pcb_t *new_task = (pcb_t*)k_malloc(sizeof(pcb_t));
    print("PCB allocated: %x\n", (uint32_t)new_task);

    // 2. Allocate the 16KB Stack
    new_task->kernel_stack = (uintptr_t)k_malloc(16384);
    
    // 3. Purge the Stack (The Fix for the 0x1C00 GPF(FILLING WITH ZEROES))
    uint8_t *stack_memory = (uint8_t *)new_task->kernel_stack;
    for (int i = 0; i < 16384; i++) {
        stack_memory[i] = 0;
    }

    // Place the canary at the absolute bottom (lowest address) to detect overflow
    uint32_t *canary = (uint32_t*)new_task->kernel_stack;
    *canary = 0xDEADBEEF;

    print("Kernel_stack : %x\n", new_task->kernel_stack);

    
    // Shift the pointer DOWN by the size of the struct so it fits perfectly inside the 16KB.
    new_task->regs = (registers_t*)(new_task->kernel_stack + 16384 - sizeof(registers_t));

    // Initialize

    new_task->regs->cs = 0x08;
    new_task->regs->eip = (uint32_t)task_entry;
    
    new_task->PID = next_pid++;
    new_task->state = TASK_READY;
    new_task->pd_addr = pd_addr;
    new_task->next = NULL;
    
    // Enable Interrupts for this task
    new_task->regs->eflags = 0x202; 
    
    // The Data Segment
    new_task->regs->ds = 0x10;

    // These act as padding for the 'add esp, 8' instruction in the assembly
    new_task->regs->err_code = 0;
    new_task->regs->int_no = 0;

    // Note: EDI, ESI, EBP, EBX, EDX, ECX, EAX are already 0

    print("New task created : %d(PID)\n", new_task->PID);
    
    return new_task;
}
void task_exit()
{
    
    if(current_task == idle_task || current_task->PID == 0)
    {
        serial_write(SERIAL_COM1_BASE,"ERROR!!!, ONLY 1 TASK LEFT, CAN'T EXIT\n");
        return;
    }
    __asm__ volatile("cli"); //This was above the last task check, it was a bug

    current_task->state = TASK_DEAD;
    
    //Zombie list
    if(zombie_head == NULL)
    {
        zombie_head = current_task;
        current_zombie = zombie_head;
        zombie_head->next = NULL;
    }
    else {
        current_zombie->next = current_task;
        current_zombie = current_task;
        current_zombie->next = NULL;
    }
    __asm__ volatile("int $0x81");
    while(1) __asm__ volatile("hlt");
}
void task_reaper()
{
    while(1){
        uint32_t flags;
        __asm__ volatile("pushf; pop %0; cli":"=r"(flags) :: "memory");
        pcb_t *tmp = zombie_head;
        if(tmp)
        {
            zombie_head = tmp->next;
            if(!zombie_head) current_zombie = NULL;
        }
        if(flags & 0x200) __asm__ volatile("sti" ::: "memory");
        if(tmp){
        k_free((void*)tmp->kernel_stack);
        k_free((void*)tmp);
        __asm__ volatile("int $0x81");
        }
        else {
            task_sleep(10);
        }
    }
}
void task_sleep(uint32_t ms)
{   
    uint32_t flags;
    __asm__ volatile("pushf; pop %0; cli":"=r"(flags)::"memory");
    current_task->state = TASK_SLEEPING;
    current_task->WAIT_TIME = ms + timer_ticks;     //The timer is ticking with frequency 1000 Hz(1 ms/tick)
    if(sleep_queue_head == NULL){
        sleep_queue_head = current_task;
        current_task->next = NULL;
    }
    else{
        current_task->next = sleep_queue_head;
        sleep_queue_head = current_task;
    }
    __asm__ volatile("int $0x81");
    if(flags & 0x200)__asm__ volatile("sti");
}



