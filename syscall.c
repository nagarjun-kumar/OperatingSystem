#include "syscall.h"



/* pcb_t* get_active_pcb()
 * Description: A function to get a pointer to the pcb of the active process.
 * Inputs: None
 * Output: Returns a pointer to the pcb of the active process.
 * Returned Value: A pointer to the pcb of the active process.
 * Side Effects: None
 */
 pcb_t* get_active_pcb() {
   return get_pcb(running_process_id); /* Return the pcb corresponding to the pid of active process */
 }

/* pcb_t* get_pcb()
 * Description: A function to get a pointer to the process control block accroding to input proc.id.
 * Inputs: Integer indicating the process id.
 * Output: Returns a pointer to the wanted pcb
 * Returned Value: A pointer to the wanted pcb
 * Side Effects: None
 */
pcb_t* get_pcb(int32_t pid) {
  if (pid >= 0 && pid < MAX_NUM_PROCESSES) { /* Prereq: the pid input is valid */
    uint32_t kernel_stack_start_address; /* The starting address of the kernel stack */
    uint32_t cur_pcb_address; /* Address of current pcb */
    kernel_stack_start_address = KERNEL_START_ADD + KERNEL_PAGE_SIZE; /* place the first task's kernel stak at the bottom of the 4 MB kernel page */
    cur_pcb_address = kernel_stack_start_address - pid * PCB_STACK_SIZE - PCB_STACK_SIZE; /* Stack grows upwards */
    return (pcb_t*)cur_pcb_address; /* Cast the address to a pcb pointer and return the pointer */
  }
  return NULL; /* Prereqs not met, return NULL pointer */
 }

 /* int32_t get_avaialable_pid()
  * Description: A function to get a the next available pid.
  * Inputs: None
  * Output: Returns an integer as the available pid. Mark corr. pcb as existent
  * Returned Value: Int - avaialable pid
  * Side Effects: Mark corr. pcb as existent
  */
  int32_t get_available_pid() {
    int32_t pid_tmp; /* Possibole pid for looping */
    pid_tmp = 0; /* Start at the very first possible pid */
    while (pid_tmp != MAX_NUM_PROCESSES) { /* Loop until reaching max. # processes */
      pcb_t* pcb_tmp = get_pcb(pid_tmp); /* Get corr. pcb */
      if (pcb_tmp -> existent == FALSE_) { /* Check if pcb is already existing */
        pcb_tmp -> existent = TRUE_; /* We'll take it as the new-allocated pcb. Now existent */
        return pid_tmp; /* Return the pid */
      }
      pid_tmp++;
    }
    return INVALID_PID; /* No avaialble seats. Return an invalid pid */
  }


/* pcb_t* file_desc_array_init()
 * Description: A function to initialize the fd array in pcb.
 * Inputs: pcb_t* cur_pcb (Pointer to the referenced pcb)
 * Output: Returns a pointer to the pcb with cur_process -> file_desc_array initialized.
 * Returned Value: A pointer to the pcb with cur_process -> file_desc_array initialized.
 * Side Effects: Initialize the cur_process -> file_desc_array in specified pcb.
 */
 pcb_t* file_desc_array_init(pcb_t* cur_pcb) {
   if (fs_abs_init(cur_pcb -> file_desc_array) == FS_ABSTRACTION_FAILURE) {
     return NULL;  /* Initialize the file descriptor array of current pcb. If fails, return nullptr */
   }
   return cur_pcb; /* If succeed, just return the pcb with cur_process -> file_desc_array initialized */
 }
 
 /* int32_t execute()
  * Description: A syscall that attempts to load and exeute a new program, handing off the
  *               processor to the new program until it terminates.
  * Inputs: const uint8_t* command (command of the syscall)
  * Output: modifies data structures related to syscall. Returned integer signifies success/failure
  * Returned Value: An integer to signify success or failure
  * Side Effects: Loads and executes programs, handing off processor to such new programs
  */
  int32_t execute(const uint8_t* command) {
    int32_t result; /* Store the returned value of helper func */
    cli(); /* During the process starting routine, no interrupts are allowed */
    result = execute_helper(command);
    return result;
  }
 /* int32_t execute_helper()
  * Description: A helper to the syscall that attempts to load and exeute a new program, handing off the
  *               processor to the new program until it terminates.
  * Inputs: const uint8_t* command (command of the syscall)
  * Output: modifies data structures related to syscall. Returned integer signifies success/failure
  * Returned Value: An integer to signify success or failure
  * Side Effects: Loads and executes programs, handing off processor to such new programs
  */
  int32_t execute_helper (const uint8_t* command) {
    uint8_t filename[MAX_FILENAME_LENGTH + 1]; /* The filename buffer */
    uint8_t argument[MAX_ARG_LENGTH + 1]; /* The argument buffer */
    int name_parse_idx; /* Index for filename parsing */
    int arg_parse_idx; /* Index of argument parsing */
    int arg_idx; /* Index in the argument buffer */
    int command_idx; /* Index in original command */
    int32_t fd; /* The descriptor # in the desc. array */
    char buf[FILE_HEADER_LENGTH]; /* The buf to read file header */
    int unmatched_magic; /* Boolean to check if unmatched magic numbers presented */
    int buf_idx; /* Index in buf */
    uint32_t dir_idx; /* The entry in page directory for input */
    uint32_t esp; /* Current esp */
    uint32_t ebp; /* Current ebp */
    uint32_t kernel_stack_start_address; /* The starting address of the kernel stack */
    uint32_t entry_point; /* Bytes 24-27 of the executable */
    uint32_t kmode_stack; /* Process's kernel-mode stack address */
    int32_t cur_pid; /* The pid allocated for current process */
    pcb_t* cur_process; /* The pointer to the pcb of current process */
    uint32_t vm_idx; /* Video memory index in page tables */
    /* Step 0: Sanity Check */
    if (command == NULL || command[0] == '\0') { /* Check if command is valid */
      printf("Error: System Call - execute(): Command is Empty or Null");
      return SYSCALL_FAILURE;
    }

    /*---------------------------------------------Step 1: Parsing-----------------------------------------------*/
    memset(argument, 0, MAX_ARG_LENGTH + 1); /* Initialize the argument buffer to hold all empty values */
    memset(filename, 0, MAX_FILENAME_LENGTH + 1); /* Initialize the filename buffer to hold all empty values */
    name_parse_idx = 0; /* Start from very first index */
    command_idx = 0; /* Set start of command index as 0 */
    while (command[command_idx] == ' ') { /* Disregard starting blank spaces */
      command_idx++;
    }
    while(TRUE_) { /* Parsing Loop for Filename */
      if (name_parse_idx == MAX_FILENAME_LENGTH) {
        break; /* If we go beyond the length limit of filename, just use the truncated filename */
      }
      if (command[command_idx] == ' ') { /* Command is space-separated */
        break; /* The first word is the filename, so stop iterating when reaching space */
      }
      filename[name_parse_idx] = command[command_idx]; /* Copy current character */
      command_idx++;
      name_parse_idx++; /* Move to next character */
    }
    if (command[name_parse_idx] == ' ') { /* The case when the filename length is not over the limit : Parse arguments */
      arg_parse_idx = command_idx + 1; /* Move the next index: Start of arguments */
      while (command[arg_parse_idx] == ' ') { /* Disregard spaces */
        arg_parse_idx++;
      }
      arg_idx = 0; /* The index in the argument buffer */
      while(TRUE_) { /* Parsing Loop for Argument */
        if (command[arg_parse_idx] == '\0') {
          break; /* Break when we reach the end of the argument string */
        }
        argument[arg_idx] = command[arg_parse_idx]; /* Copy current character */
        arg_idx++;
        arg_parse_idx++; /* Increment Indices */
      }
    }

    /*-----------------------------------Step 2: Check if Prereqs are Met for EXE-------------------------------------------*/
    cur_pid = get_available_pid(); /* Allocate the pid for cur. process */
    if (cur_pid == INVALID_PID) { /* Check if no more pid are avaialble for allocation */
      printf("Error: System Call - execute(): Reached Maximum Number of Running Process %s\n", filename);
      return SYSCALL_TOO_MANY_PROCESSES;
    }
    cur_process = get_pcb(cur_pid); /* Get the pcb of current process */
    cur_process -> cur_pid = cur_pid; /* Store pid */
    if (fs_abs_init(cur_process -> file_desc_array) == FS_ABSTRACTION_FAILURE) { /* Initialize file sys abstraction */
      printf("Error: System Call - execute(): FS Abstraction Failed to Initialize");
      get_pcb(cur_pid) -> existent = FALSE_;
      return SYSCALL_FAILURE; /* Return failure if unable to initialize */
    }
    fd = fs_abs_open(cur_process -> file_desc_array, (char*)filename); /* Open the file, fd stores desc. # */
    if (fd == FS_ABSTRACTION_FAILURE) { /* Check if file is opened correctly */
      printf("Error: System Call - execute(): Opening File Failed %s\n", filename);
      get_pcb(cur_pid) -> existent = FALSE_;
      return SYSCALL_FAILURE; /* If not, return failure */
    }
    if (fs_abs_read(cur_process -> file_desc_array, fd, buf, FILE_HEADER_LENGTH) == FS_ABSTRACTION_FAILURE) { /* Read header to buf */
      printf("Error: System Call - execute(): Reading File Header Failed %s\n", filename);
      get_pcb(cur_pid) -> existent = FALSE_;
      return SYSCALL_FAILURE; /* Return failure if unable to read header */
    }
    if (cur_process -> file_desc_array[fd].file_position != FILE_HEADER_LENGTH) { /* Check if header length is correct */
      printf("Error: System Call - execute(): File Header Length is Incorrect %s\n", filename);
      get_pcb(cur_pid) -> existent = FALSE_;
      return SYSCALL_FAILURE;
    }
    buf_idx = 0; /* Start at the very first byte of buf */
    unmatched_magic = FALSE_; /* Check if unmatched magic # presented */
    while (TRUE_) { /* Loop to check magic numbers */
      if (buf[buf_idx] != EXE_HEADER_MAGIC_0) { /* Check 1st byte */
        unmatched_magic = TRUE_;
        break;
      }
      buf_idx++;
      if (buf[buf_idx] != EXE_HEADER_MAGIC_1) { /* Check 2nd byte */
        unmatched_magic = TRUE_;
        break;
      }
      buf_idx++;
      if (buf[buf_idx] != EXE_HEADER_MAGIC_2) { /* Check 3rd byte */
        unmatched_magic = TRUE_;
        break;
      }
      buf_idx++;
      if (buf[buf_idx] != EXE_HEADER_MAGIC_3) { /* Check 4st byte */
        unmatched_magic = TRUE_;
        break;
      }
      break;
    }
    if (unmatched_magic == TRUE_) { /* If true, file is not executable */
      printf("Error: System Call - execute(): File is not Executable\n", filename);
      get_pcb(cur_pid) -> existent = FALSE_;
      return SYSCALL_FAILURE;
    }
    cur_process -> parent_pid = running_process_id; /* Current running proc. becomes parent */
    cur_process -> terminal_id = running_terminal;
    term[running_terminal].running_process = cur_pid;

    /*----------------------------------------------Step 3: Deal with Paging----------------------------------------------*/
    dir_idx = (uint32_t) PROGRAM_IMG_ADDRESS >> DIR_OFFSET; /* Get the current page_dir entry */
    page_directory[dir_idx].present_4mb = 1; /* Mark Presense */
    page_directory[dir_idx].read_write_4mb = 1; /* Read and write */
    page_directory[dir_idx].user_supervisor_4mb = 1; /* Mark User-Accessible */
    page_directory[dir_idx].write_through_4mb = 0; /* Write-through Caching Disabled */
    page_directory[dir_idx].cache_disabled_4mb = 0; /* Page is cached */
    page_directory[dir_idx].accessed_4mb = 0; /* Not read/ written to */
    page_directory[dir_idx].dirty_4mb = 0; /* Not dirty */
    page_directory[dir_idx].page_size_4mb = 1; /* 4 MB page */
    page_directory[dir_idx].global_page_4mb = 0; /* Page is local */
    page_directory[dir_idx].avail_4mb = 0; /* Free bits not initialized */
    page_directory[dir_idx].pat_memory_type_4mb = 0; /* Not modified */
    page_directory[dir_idx].reserved_4mb = 0; /* Not modified */
    page_directory[dir_idx].page_start_add_4mb = PROCESS_BASE_ADDRESS + cur_pid; /* Physical address */
    vm_idx = ((VID_MEM_ADD & MASK_21_12) >> TBL_OFFSET);
    if (running_terminal == visible_terminal) { /* When process is displayed */
      page_table[vm_idx].page_start_add = vm_idx; /* Put it on vid. mem. */
    } else { /* When process is running in another terminal */
      page_table[vm_idx].page_start_add = vm_idx + 1 + cur_process -> terminal_id; /* Put it in subsequent mem. */
    }
    asm volatile ( /* Flush the TLB by writing to register cr3 */
      "movl %%cr3, %%eax     ;"
      "movl %%eax, %%cr3     ;"
      : /* No outputs */
      : /* No inputs */
      : "eax", "cc"  /* Clobbers */
      );

    /*-----------------------------------------Step 4: User Level Program Loader--------------------------------------------*/
    cur_process -> file_desc_array[fd].file_position = 0; /* Reset starting position to 0 to read the whole file */
    if (fs_abs_read(cur_process -> file_desc_array, fd, (char*) PROGRAM_IMG_ADDRESS, REF_EXE_FILE_LEN) == FS_ABSTRACTION_FAILURE) { /* Read to prog. img. */
      printf("Error: System Call - execute(): Load File to Memory Failed\n", filename);
      get_pcb(cur_pid) -> existent = FALSE_;
      return SYSCALL_FAILURE; /* Failed to load file to memory */
    }
    if (fs_abs_close(cur_process -> file_desc_array, fd) == FS_ABSTRACTION_FAILURE) { /* Close the file */
      printf("Error: System Call - execute(): Closing File Failed\n", filename);
      get_pcb(cur_pid) -> existent = FALSE_;
      return SYSCALL_FAILURE; /* Failed to Close File */
    }

    /*-----------------------------------------------Step 5: Create pcb-------------------------------------------------------*/
    term[running_terminal].pcb = cur_process;
    if (strncmp("shell", (int8_t*)filename, 5) == 0) {
      term[running_terminal].pcb->shell_flag = 1;
    }
    asm volatile ( /* Get current esp */
      "movl %%esp, %0     ;"
      : "=r" (esp) /* Output : esp. No inputs. Not clobbering */
      );
    cur_process -> parent_esp = esp; /* Load esp */
    asm volatile ( /* Get current ebp */
      "movl %%ebp, %0     ;"
      : "=r" (ebp) /* Output : ebp. No inputs. Not clobbering */
      );
    cur_process -> parent_ebp = ebp; /* Load ebp */
    kernel_stack_start_address = KERNEL_START_ADD + KERNEL_PAGE_SIZE; /* place the first task's kernel stak at the bottom of the 4 MB kernel page */
    memcpy(cur_process -> arg, argument, MAX_ARG_LENGTH + 1); /* Store current argument in pcb */
    /* Step 6: Context Switch */
    running_process_id = cur_pid; /* Initialized process becomes cur. running process */
    entry_point = *((uint32_t*)(PROGRAM_IMG_ADDRESS + ENTRY_PT_OFFSET));
    kmode_stack = kernel_stack_start_address - cur_pid * PCB_STACK_SIZE; /* Stack add. */
    tss.esp0 = kmode_stack - FOUR_BYTES; /* Modify esp0 of task state segment */
    tss.ss0 = KERNEL_DS;
    asm volatile ( /* Set up stack and set DS to point to entry in GDT for user data segment */
      "andl    $0x00FF, %%ebx    ;"
      "pushl   %%ebx             ;"
      "pushl   %%edx             ;"
      "pushfl                    ;"
      "popl    %%edx             ;"
      "orl     $0x200, %%edx     ;"
      "pushl   %%edx             ;"
      "pushl   %%ecx             ;"
      "pushl   %%eax             ;"
      "iret                      ;"
      : /* No outputs */
      : "a"(entry_point), "b"(USER_DS), "c"(USER_CS), "d"(USER_STACK_ADDRESS-FOUR_BYTES) /* Inputs */
      : "cc","memory"  /* Clobbers */
      );
    return SYSCALL_SUCCESS; /* Finished all tasks and return success */

  }

/* int32_t open()
 * Description: A syscall that attempts to open.
 * Inputs: const uint8_t* filename
 * Output: opens a file
 * Returned Value: Same as fs_abs_open()
 * Side Effects: Opens a file.
 */
int32_t open (const uint8_t* filename) {
  pcb_t* cur_pcb = get_active_pcb();
  return fs_abs_open(cur_pcb -> file_desc_array, (const char*)filename);
}

/* int32_t close()
 * Description: A syscall that attempts to close.
 * Inputs: int32_t fd
 * Output: closes a file
 * Returned Value: Same as fs_abs_close()
 * Side Effects: Closes a file.
 */
int32_t close (int32_t fd) {
  pcb_t* cur_pcb = get_active_pcb();
  return fs_abs_close(cur_pcb -> file_desc_array, fd);
}

/* int32_t read()
 * Description: A syscall that attempts to read.
 * Inputs: int32_t fd, void* buf, int32_t nbytes
 * Output: reads a file
 * Returned Value: Same as fs_abs_read()
 * Side Effects: Reads a file.
 */
int32_t read (int32_t fd, void* buf, int32_t nbytes) {
  pcb_t* cur_pcb = get_active_pcb();
  return fs_abs_read(cur_pcb -> file_desc_array, fd, buf, nbytes);
}

/* int32_t write()
 * Description: A syscall that attempts to write.
 * Inputs: int32_t fd, const void* buf, int32_t nbytes
 * Output: writes a file
 * Returned Value: Same as fs_abs_write()
 * Side Effects: Writes a file.
 */
int32_t write (int32_t fd, const void* buf, int32_t nbytes) {
  pcb_t* cur_pcb = get_active_pcb();
  return fs_abs_write(cur_pcb -> file_desc_array, fd, buf, nbytes);
}

/* int32_t halt()
 * Description: A syscall that attempts to halt.
 * Inputs: uint8_t status (Process Status)
 * Output: Halts a process
 * Returned Value: Integer to signify success/failure. Shall always succeed.
 * Side Effects: Halts a process.
 */
 int32_t halt(uint8_t status) {
   int32_t result; /* Store the returned value of helper func */
   cli(); /* During the process halting routine, no interrupts are allowed */
   result = halt_helper(status);
   return result;
 }

/* int32_t halt_helper()
 * Description: A helper to the syscall that attempts to halt.
 * Inputs: uint8_t status (Process Status)
 * Output: Halts a process
 * Returned Value: Integer to signify success/failure. Shall always succeed.
 * Side Effects: Halts a process.
 */
int32_t halt_helper (uint8_t status) {
   uint32_t status_augmented;
   status_augmented = status;
   if (halt_flag) {
     status_augmented = EXCEPTION_IDX;
     halt_flag = 0;
   }
   pcb_t* cur_pcb; /* Current pcb */
   pcb_t* parent_pcb; /* Parent pcb */
   int fd_array_idx; /* The index in file descriptor array */
   uint32_t ref_parent_pid; /* Parent pid of current process */
   uint32_t kernel_stack_start_address; /* The starting address of the kernel stack */
   uint32_t dir_idx; /* Directory index */
   uint32_t vm_idx; /* Video memory index in page tables */
   cur_pcb = get_active_pcb(); /* Refer to the active process */
   for (fd_array_idx = 0; fd_array_idx < MAX_OPENED_FILES; fd_array_idx++) {
     fs_abs_close(cur_pcb -> file_desc_array, fd_array_idx); /* Close every file */
   }
   if (cur_pcb -> parent_pid == INVALID_PID) { /* If it is the only process */
     cur_pcb -> existent = FALSE_; /* Current process marked as inexistent */
     running_process_id = INVALID_PID; /* No current running process at this moment */
     term[running_terminal].running_process = INVALID_PID; /* Running terminal has no process now */
     execute_helper((uint8_t*)"shell"); /* Creates a new shell */
     return SYSCALL_SUCCESS;
   }
   /* Default situation: process has a parent process */
   cur_pcb -> existent = FALSE_; /* Current process marked as inexistent */
   kernel_stack_start_address = KERNEL_START_ADD + KERNEL_PAGE_SIZE; /* place the first task's kernel stak at the bottom of the 4 MB kernel page */
   tss.esp0 = kernel_stack_start_address - (cur_pcb->parent_pid) * PCB_STACK_SIZE; /* Stack segment*/
   ref_parent_pid = cur_pcb -> parent_pid; /* Load parent pid */
   parent_pcb = get_pcb(ref_parent_pid); /* Load parent pcb */
   dir_idx = (uint32_t) PROGRAM_IMG_ADDRESS >> DIR_OFFSET; /* Get the current page_dir entry */
   page_directory[dir_idx].present_4mb = 1; /* Mark Presense */
   page_directory[dir_idx].read_write_4mb = 1; /* Read and write */
   page_directory[dir_idx].user_supervisor_4mb = 1; /* Mark User-Accessible */
   page_directory[dir_idx].write_through_4mb = 0; /* Write-through Caching Disabled */
   page_directory[dir_idx].cache_disabled_4mb = 0; /* Page is cached */
   page_directory[dir_idx].accessed_4mb = 0; /* Not read/ written to */
   page_directory[dir_idx].dirty_4mb = 0; /* Not dirty */
   page_directory[dir_idx].page_size_4mb = 1; /* 4 MB page */
   page_directory[dir_idx].global_page_4mb = 0; /* Page is local */
   page_directory[dir_idx].avail_4mb = 0; /* Free bits not initialized */
   page_directory[dir_idx].pat_memory_type_4mb = 0; /* Not modified */
   page_directory[dir_idx].reserved_4mb = 0; /* Not modified */
   page_directory[dir_idx].page_start_add_4mb = PROCESS_BASE_ADDRESS + ref_parent_pid; /* Physical add*/
   vm_idx = ((VID_MEM_ADD & MASK_21_12) >> TBL_OFFSET);
   if (running_terminal == visible_terminal) { /* When process is displayed */
     page_table[vm_idx].page_start_add = vm_idx; /* Put it on vid. mem. */
   } else { /* When process is running in another terminal */
     page_table[vm_idx].page_start_add = vm_idx + 1 + parent_pcb -> terminal_id; /* Put it in subsequent mem. */
   }
   asm volatile ( /* Flush the TLB by writing to register cr3 */
      "movl %%cr3, %%eax     ;"
      "movl %%eax, %%cr3     ;"
      : /* No outputs */
      : /* No inputs */
      : "eax", "cc"  /* Clobbers */
      );
   running_process_id = ref_parent_pid; /* PID of running process becomes that of the parent */
   term[running_terminal].running_process = ref_parent_pid; /* Update terminal array */
   term[running_terminal].pcb=get_pcb(ref_parent_pid);
   asm volatile ( /* Store status in register eax */
      "movl %%ecx, %%esp     ;"
      "movl %%edx, %%ebp     ;"
      "movl %%ebx, %%eax     ;"
      "leave                 ;"
      "ret                   ;"
      : /* No outputs */
      : "b"(status_augmented), "c"(cur_pcb->parent_esp), "d"(cur_pcb->parent_ebp)/* Inputs */
      );
   return SYSCALL_SUCCESS; /* Just function custom */
 }

/* int32_t getargs()
 * Description: A syscall that gets the argument of current processing executable. Argument copied to buf
 * Inputs: const uint8_t* buf, int32_t nbytes
 * Output: Copies argument to buf. Returns an integer to signify success/failure
 * Returned Value: Integer. 0 upon success, -1 upon failure
 * Side Effects: Updates the input buf
 */
int32_t getargs (uint8_t* buf, int32_t nbytes) {
  pcb_t* cur_pcb; /* Current pcb referenced */
  int32_t num_bytes_to_copy; /* Number of bytes to copy */
  if (!buf) {
    return SYSCALL_FAILURE; /* Sanity check: The buf should be valid */
  }
  cur_pcb = get_active_pcb(); /* Load current pcb */
  if (cur_pcb -> arg[0] == 0) { /* See if the first byte is a null byte */
    return SYSCALL_FAILURE; /* If so, there are no arguments, so return -1 */
  }
  if (strlen(cur_pcb -> arg) > nbytes) {
    return SYSCALL_FAILURE; /* Check if the length of argument is over the maximum length */
  }
  memset(buf, 0, nbytes); /* Clear the buffer */
  if (nbytes > MAX_ARG_LENGTH) { /* Check the number of bytes to copy to buffer */
    num_bytes_to_copy = MAX_ARG_LENGTH;
  } else {
    num_bytes_to_copy = nbytes;
  }
  memcpy(buf, cur_pcb -> arg, num_bytes_to_copy); /* Copy to buffer */
  return SYSCALL_SUCCESS; /* Return 0 upon success */
}

/* int32_t vidmap()
 * Description: A syscall that maps the text-mode video memory into user space at a pre-set virtual address.
 * Inputs: const uint8_t** screen_start
 * Output: Updated input pointer to hold the VM address for user programs. Returned integer to signify Success/Failure.
 * Returned Value: Integer. 0 upon success, -1 upon failure
 * Side Effects: Updates input pointer. Enables paging for vidmap.
 */
 int32_t vidmap (uint8_t** screen_start) {
   pcb_t* cur_pcb; /* Current pcb of running process */
   uint32_t dir_idx; /* Directory index for program (32) */
   uint32_t vm_idx; /* Index in page table for video memory */
   uint32_t user_video_address; /* Address for user video */
   if (!screen_start) { /* Sanity Check: Input pointer has to be valid */
     return SYSCALL_FAILURE;
   }
   if (((uint32_t)screen_start >> DIR_OFFSET) != ((uint32_t) PROGRAM_IMG_ADDRESS >> DIR_OFFSET)) {
     return SYSCALL_FAILURE; /* Sanity check: screen_start has to be within the range of user program img */
   }
   cur_pcb = get_active_pcb();
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
      page_table[vm_idx].page_start_add = vm_idx + 1 + cur_pcb -> terminal_id; /* Put it in subsequent mem. */
      user_vidmap_page_table[vm_idx].page_start_add = vm_idx + 1 + cur_pcb -> terminal_id; /* Update user video mapping PT */
      user_vidmap_page_table[vm_idx].present = 1; /* Mark Presense */
   }
   asm volatile ( /* Flush the TLB by writing to register cr3 */
      "movl %%cr3, %%eax     ;"
      "movl %%eax, %%cr3     ;"
      : /* No outputs */
      : /* No inputs */
      : "eax", "cc"  /* Clobbers */
      );
   user_video_address = PT_USER_VIDMAP_LOCATION * FOUR_MB + VID_MEM_ADD; /* Get user video address (constant) */
   *screen_start = (uint8_t*) user_video_address; /* Cast address to a pointer and store to *screen_start */
   return SYSCALL_SUCCESS; /* Return success upon finish */
 }

