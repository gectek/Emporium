/*1. allocate PCB
2. allocate kernel stack
3. set up initial register state
4. set PID
5. set state to TASK_READY
6. add to scheduler's linked list*/
#include "task.h"
#include "../mm/heap.h"
#include "../../libc/stdfun.h"
uintptr_t next_pid = 1;
extern pcb_t *current_task;
extern pcb_t *task_list;



/// HAVE TO CHANGE THE PCB LIST TO DOUBLY LINKED LIST


pcb_t *create_task(void (*task_entry)(), uintptr_t pd_addr) {
    // 1. Allocate the Process Control Block
    pcb_t *new_task = (pcb_t*)k_malloc(sizeof(pcb_t));
    print("PCB allocated: %x\n", (uint32_t)new_task);

    // 2. Allocate the 16KB Stack
    new_task->kernel_stack = (uintptr_t)k_malloc(16384);
    
    // 3. Purge the Stack (The Fix for the 0x1C00 GPF)
    uint8_t *stack_memory = (uint8_t *)new_task->kernel_stack;
    for (int i = 0; i < 16384; i++) {
        stack_memory[i] = 0;
    }

    // Place the canary at the absolute bottom (lowest address) to detect overflow
    uint32_t *canary = (uint32_t*)new_task->kernel_stack;
    *canary = 0xDEADBEEF;

    print("Kernel_stack : %x\n", new_task->kernel_stack);

    // 4. Hang the Mold (The Fix for the 198 GPF Shift)
    // Shift the pointer DOWN by the size of the struct so it fits perfectly inside the 16KB.
    new_task->regs = (registers_t*)(new_task->kernel_stack + 16384 - sizeof(registers_t));

    // 5. Fill the Mold

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

    // Note: EDI, ESI, EBP, EBX, EDX, ECX, EAX are already 0 because of the purge loop!

    print("New task created : %d(PID)\n", new_task->PID);
    
    return new_task;
}
/*void task_exit()
{
    __asm__ volatile("cli");
    pcb_t *prev = task_list;
    while (prev->next != current_task) {
        prev=prev->next;
    }
    if(task_list == current_task)
        task_list = current_task->next;     //Setting the head of list to next one

    if(task_list->next == task_list)
    {
        print("ERROR!!!, ONLY 1 TASK LEFT, CAN'T EXIT\n");
        goto exit;
    }
    prev->next = current_task->next;
    current_task->state = TASK_DEAD;

exit:
    __asm__ volatile("sti");
    while(1)
    {
        __asm__ volatile("hlt");
    }

}*/

