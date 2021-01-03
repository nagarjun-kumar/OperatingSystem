#include "scheduler.h"
/* void scheduler()
 * Description: A function to switch running process periodically.
 * Inputs: None
 * Output: Switches running process every 15 ms
 * Returned Value: None
 * Side Effects: Performs periodical running process switch
 */

void scheduler() {
  pcb_t* cur_pcb; /* PCB of current process */
  pcb_t* next_pcb; /* PCB in next terminal */
  //pcb_t tmp_pcb; /* Temporary pcb */
  uint32_t esp;
  uint32_t ebp;
  uint32_t dir_idx; /* Index in page directory */
  cur_pcb = term[running_terminal].pcb; /* Retrieve current PID */
  uint32_t kernel_stack_start_address; /* The starting address of the kernel stack */
  uint32_t kmode_stack; /* Process's kernel-mode stack address */
  uint32_t vm_idx; /* Vid. Mem. index */

  /*-------------------------------step 1: get current esp and ebp----------------------------------*/
  asm volatile ( /* Get current esp */
    "movl %%esp, %0     ;"
    : "=r" (esp) /* Output : esp. No inputs. Not clobbering */
  );
  cur_pcb -> cur_esp = esp; /* Load esp */
  asm volatile ( /* Get current ebp */
    "movl %%ebp, %0     ;"
    : "=r" (ebp) /* Output : ebp. No inputs. Not clobbering */
  );
  cur_pcb -> cur_ebp = ebp; /* Load ebp */


  /*---------------------------step 2: deal with base terminal condition------------------------------*/
  running_terminal = (running_terminal + 1) % TERM_NUM; /* Round Robin running terminal switch */
  if (!(term[running_terminal].pcb)) { /* See if new terminal has no process. If so, wait until next interrupt */
    switch_terminal(running_terminal); /* Switch to running terminal for process initialization */
    send_eoi(PIT_IRQ); /* Function in reality doesn't return after execute process */
    running_process_id = INVALID_PID;
    execute((uint8_t *)"shell"); /* Start a new shell program */
  }


  /*-----------------------------------step 3: deal with paging---------------------------------------*/
  running_process_id = term[running_terminal].pcb -> cur_pid; /* Update running pid */
  next_pcb = term[running_terminal].pcb; /* Get next pcb */
  /* Remap Video Memory */
  dir_idx = (uint32_t) PROGRAM_IMG_ADDRESS >> DIR_OFFSET; /* Get the current page_dir entry */
  page_directory[dir_idx].present_4mb = 1; /* Mark Presense */
  page_directory[dir_idx].read_write_4mb = 1; /* Read and write */
  page_directory[dir_idx].user_supervisor_4mb = 1; /* Mark User-Accessible */
  page_directory[dir_idx].page_size_4mb = 1; /* 4 MB page */
  page_directory[dir_idx].page_start_add_4mb = PROCESS_BASE_ADDRESS + running_process_id; /* Physical address */
  vm_idx = ((VID_MEM_ADD & MASK_21_12) >> TBL_OFFSET); /* Get video memory index */
  if (visible_terminal == running_terminal) {/* Process is on screen */
    page_table[vm_idx].page_start_add = vm_idx;
    user_vidmap_page_table[vm_idx].page_start_add = vm_idx; /* Load page address */
    user_vidmap_page_table[vm_idx].present = 1; /* Mark presense */
  } else { /* Process not on screen */
    page_table[vm_idx].page_start_add = vm_idx + 1 + next_pcb -> terminal_id; /* Put it in subsequent mem. */
    user_vidmap_page_table[vm_idx].page_start_add = vm_idx + 1 + next_pcb -> terminal_id; /* Update user video mapping PT */
    user_vidmap_page_table[vm_idx].present = 1; /* Mark Presense */
  }
  asm volatile ( /* Flush the TLB by writing to register cr3 */
    "movl %%cr3, %%eax     ;"
    "movl %%eax, %%cr3     ;"
    : /* No outputs */
    : /* No inputs */
    : "eax", "cc"  /* Clobbers */
    );


  /*------------------------------------step 4: context switch--------------------------------------*/
  tss.ss0 = KERNEL_DS;
  kernel_stack_start_address = KERNEL_START_ADD + KERNEL_PAGE_SIZE; /* place the first task's kernel stak at the bottom of the 4 MB kernel page */
  kmode_stack = kernel_stack_start_address - (next_pcb -> cur_pid) * PCB_STACK_SIZE; /* Stack add. */
  tss.esp0 = kmode_stack - FOUR_BYTES; /* Modify esp0 of task state segment */
  send_eoi(PIT_IRQ); /* Not literally returning, send eoi now */
  asm volatile ( /* Switch to next process */
    "movl %0, %%esp     ;"
    "movl %1, %%ebp     ;"
    : /* No Outputs */
    : "r" (next_pcb -> cur_esp), "r" (next_pcb -> cur_ebp) /* Inputs : esp and ebp */
    : "esp", "ebp" /* Clobbers */
  );
  return;
}


