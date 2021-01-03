#include "fs_abstraction.h"
#include "fs.h"
#include "rtc.h"
#include "terminal.h" 

/* Following functions provide a unification of driver calls. Used for syscall */ 

/* int32_t fs_abs_init()
 * Description: A function to initialize the file descriptor array.
 * Inputs: file_arr_struct_t* file_array (pointer to an uninitialized file array)
 * Output: Updated file des. array; returned flag to signify success/failure
 * Returned Value: Integer - Success or Failure
 * Side Effects: Changes the input array.
 */
 int32_t fs_abs_init(file_arr_struct_t* file_array) {
   int loop_idx; /* Loop index to initialize entries */
   if (file_array != NULL) { /* Ensure input pointer is valid */
     loop_idx = 0; /* Start at descriptor 0 */
     while (loop_idx < MAX_OPENED_FILES) { /* Iterate thru entries */
       if (loop_idx == STDIN_IDX) { /* Deal with stdin */
         file_array[loop_idx].jmp_table = &terminal_stdin_jmptable; /* Initialize jmp table*/
       } else if (loop_idx == STDOUT_IDX) { /* Deal with stdout */
         file_array[loop_idx].jmp_table = &terminal_stdout_jmptable; /* Initialize jmp table */
       } else { /* Initialize jump table to null for other entries */
         file_array[loop_idx].jmp_table = NULL;
       }
       /* Initialize other elements in structure to default zero */
       file_array[loop_idx].inode = 0;
       file_array[loop_idx].file_position = 0;
       file_array[loop_idx].flags = 0;
       loop_idx++; /* Move to next entry */
     }
     return FS_ABSTRACTION_SUCCESS; /* Always succeed upon valid input pointer */
   }
   return FS_ABSTRACTION_FAILURE; /* Fails upon invalid pointer */
 }

 /* int32_t fs_abs_open()
  * Description: A function to open file (by specific driver) and modify array.
  * Inputs: file_arr_struct_t* file_array, const char* filename
  * Output: Updated file des. array, or returned flag to signify failure
  * Returned Value: Integer - index in the array or Failure
  * Side Effects: Changes the input array.
  */
  int32_t fs_abs_open(file_arr_struct_t* file_array, const char* filename) {
      int cur_idx; /* Allocated index for opened file in array */
      dentry_t dir_entry; /* Referenced directory entry */
      uint32_t cur_file_type; /* Current file type */
      if (filename[0] == '\0') {
        return FS_ABSTRACTION_FAILURE;
      }
      for (cur_idx = 0; ; cur_idx++) { /* Start at first entry, iterate through */
        if (cur_idx >= MAX_OPENED_FILES) { /* Index out of bounds (no seats in array) */
          return FS_ABSTRACTION_FAILURE; /* Failure */
        }
        if (file_array[cur_idx].jmp_table == NULL) { /* Empty spot found */
          break; /* Take current index and stop iteration */
        }
      }
      if (strncmp("stdin", filename, STRLEN_STDIN + 1) == 0) {
        file_array[cur_idx].jmp_table = &terminal_stdin_jmptable; /* Open stdin */
      } else if (strncmp("stdout", filename, STRLEN_STDOUT + 1) == 0) {
        file_array[cur_idx].jmp_table = &terminal_stdout_jmptable; /* Open stdout */
      } else if (read_dentry_by_name((char*)filename, &dir_entry) == FS_SUCCESS) { /* Try to read file info*/
        cur_file_type = dir_entry.file_type; /* Get type of current file */
        if (cur_file_type == RTC_TYPE_FILE) {
          file_array[cur_idx].jmp_table = &rtc_jmptable; /* Open rtc file */
        } else if (cur_file_type == FOLDER_TYPE_FILE) {
          file_array[cur_idx].jmp_table = &fs_dir_jmptable; /* Open a directory */
        } else if (cur_file_type == DEFAULT_TYPE_FILE) {
          file_array[cur_idx].jmp_table = &fs_file_jmptable; /* Open regular file */
        } else {
          return FS_ABSTRACTION_FAILURE; /* Invalid filetype */
        }
      } else {
        return FS_ABSTRACTION_FAILURE; /* No such file exists / file is invalid */
      }
      /* Prereq 1 : the jump table has a valid "open" function pointer */
      if (file_array[cur_idx].jmp_table -> open != NULL) {
        /* Prereq 2: "open" task opens the file correctly */
        if ((*file_array[cur_idx].jmp_table -> open)(&file_array[cur_idx].inode, (char*)filename) != FS_ABSTRACTION_FAILURE) {
          file_array[cur_idx].file_position = 0; /* Start at the starting point of file */
          return cur_idx; /* Return index that held current descriptor */
        }
      }
      return FS_ABSTRACTION_FAILURE; /* Prereqs not all met. Return failure */
 }

 /* int32_t fs_abs_close()
  * Description: A function to close file (by specific driver) and modify array
  * Inputs: file_arr_struct_t* file_array, int32_t id (Referenced array and descriptor ID)
  * Output: Updated file des. array, or returned flag to signify failure
  * Returned Value: Integer - index in the array or Failure
  * Side Effects: Changes the input array.
  */
  int32_t fs_abs_close(file_arr_struct_t* file_array, int32_t id) {
    if (id >= 0 && id < MAX_OPENED_FILES) { /* Prereq 1 : the id should be a valid index in array */
      if (file_array[id].jmp_table != NULL) { /* Prereq 2: the file has to be opened */
        if (file_array[id].jmp_table -> close != NULL) { /* Prereq 3: current jump table has a valid close func. pointer */
          if ((*file_array[id].jmp_table->close) (&file_array[id].inode) != FS_ABSTRACTION_FAILURE) { /* Prereq 4: Func works */
            file_array[id].jmp_table = NULL; /* Close the file by clearing jump table */
            return FS_ABSTRACTION_SUCCESS; /* Always succeed when prereqs met */
          }
        }
      }
    }
    return FS_ABSTRACTION_FAILURE; /* Fails when prereqs not fully met */
  }

  /* int32_t fs_abs_read()
   * Description: A function to read file (by specific driver) and modify array
   * Inputs: file_arr_struct_t* file_array, int32_t id, void* buf, int32_t len(Referenced array, descriptor ID, buf, length)
   * Output: Updated buf and pos in desc.structure, or returned flag to signify failure
   * Returned Value: Integer - # bytes read or Failure
   * Side Effects: Changes the input array and the buf.
   */
  int32_t fs_abs_read(file_arr_struct_t* file_array, int32_t id, void* buf, int32_t len) {
    if (id >= 0 && id < MAX_OPENED_FILES) { /* Prereq 1 : the id should be a valid index in array */
      if (file_array[id].jmp_table != NULL) { /* Prereq 2: the file has to be opened */
        if (file_array[id].jmp_table -> read != NULL) { /* Prereq 3: current jump table has a valid read func. pointer */
          /* Call appropriate task to perform reading ( might fail in this case) */
           return (*file_array[id].jmp_table -> read) (&file_array[id].inode, &file_array[id].file_position, (char*) buf, len);
        }
      }
    }
    return FS_ABSTRACTION_FAILURE; /* Fails when prereqs not fully met */
  }

  /* int32_t fs_abs_write()
   * Description: A function to write to file (by specific driver) and modify array
   * Inputs: file_arr_struct_t* file_array, int32_t id, const void* buf, int32_t len(Referenced array, descriptor ID, buf, length)
   * Output: Updated buf and pos in desc.structure, or returned flag to signify failure
   * Returned Value: Integer - # bytes read or Failure
   * Side Effects: Changes the input array and the buf.
   */
   int32_t fs_abs_write(file_arr_struct_t* file_array, int32_t id, const void* buf, int32_t len) {
     if (id >= 0 && id < MAX_OPENED_FILES) { /* Prereq 1 : the id should be a valid index in array */
       if (file_array[id].jmp_table != NULL) { /* Prereq 2: the file has to be opened */
         if (file_array[id].jmp_table -> write != NULL) { /* Prereq 3: current jump table has a valid write func. pointer */
           /* Call appropriate task to perform writing ( might fail in this case) */
            return (*file_array[id].jmp_table -> write) (&file_array[id].inode, &file_array[id].file_position, (const char*) buf, len);
         }
       }
     }
     return FS_ABSTRACTION_FAILURE; /* Fails when prereqs not fully met */
   }
