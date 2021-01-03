/*
 * Header File to Support File System Abstraction
 */

#ifndef _FS_ABSTRACTION_H_
#define _FS_ABSTRACTION_H_

#include "lib.h"
#include "types.h"

#define FS_ABSTRACTION_SUCCESS 0
#define FS_ABSTRACTION_FAILURE -1
#define MAX_OPENED_FILES 8  /* Each task can have up to 8 open files. */
#define STDIN_IDX 0 /* stdin corresponds to file descriptor 0 */
#define STDOUT_IDX 1  /* stout corresponds to file descriptor 1 */
#define STRLEN_STDIN 5 /* String "stdin" has 5 chars */
#define STRLEN_STDOUT 6 /* String "stdout" has 6 chars */



/* Functions that operates file descriptor array */
int32_t fs_abs_init(file_arr_struct_t* file_array);
int32_t fs_abs_open(file_arr_struct_t* file_array, const char* filename);
int32_t fs_abs_read(file_arr_struct_t* file_array, int32_t id, void* buf, int32_t len);
int32_t fs_abs_write(file_arr_struct_t* file_array, int32_t id, const void* buf, int32_t len);
int32_t fs_abs_close(file_arr_struct_t* file_array, int32_t id);

#endif
