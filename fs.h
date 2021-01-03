/* 
 * Header File. Contains structs to reference file system and functions to process file system.
 */

#ifndef _FS_H_
#define _FS_H_ /* Prevent double declaration of identifiers */

#include "lib.h"

#include "fs_abstraction.h" /* Supports file sys abstraction */

/* Constants relative to file system */
#define MAX_FILENAME_LENGTH    32  /* Length of file shall not exceed 4 bytes */
#define MAX_FILE_NUM           63  /* Maximum number of files presented (including the directory itself) */
#define STAT_RESERVED          52  /* 52 bits reserved after statistics */
#define FDE_RESERVED           24  /* 24 bits reserved in each directory entry */ 
#define FS_BLOCK_SIZE          4096  /* Each block has 4 KB */
#define MAX_DB_NUM             1023  /* Maximum number of data blocks in one inode : (4096 - 4) / 4 */
#define MAX_DE_NUM             1024  /* Maximum entries in data blocks: 4906/4 */
#define RTC_TYPE_FILE          0  /* Refer to RTC file */
#define FOLDER_TYPE_FILE       1  /* Refer to folder type */
#define DEFAULT_TYPE_FILE      2  /* Refer to default type files */
#define FS_SUCCESS             0  /* Success */
#define FS_FAILURE             -1  /* Failure */
#define NUM_BOOT_BLOCK         1  /* Always only 1 boot block */
#define NO_BYTES_COPIED        0  /* Zero bytes copied */
/* File directory entry */
typedef struct {
    char     file_name[MAX_FILENAME_LENGTH];
    uint32_t file_type;
    uint32_t inode_num;
    uint8_t  entry_reserved[FDE_RESERVED];
} dentry_t;

/* Boot Block */
typedef struct {
    uint32_t     num_dir_entries;
    uint32_t     num_inodes;
    uint32_t     num_data_blocks;
    uint8_t      boot_block_reserved[STAT_RESERVED];
    dentry_t     files[MAX_FILE_NUM];
} boot_block_t;

/* Inode */
typedef struct {
    uint32_t inode_length;
    uint32_t data_block[MAX_DB_NUM];
} inode_t;

/* Data Block */
typedef struct {
    uint32_t data_entry[MAX_DE_NUM];
} data_block_t;

/* Three Core Routine Functions for File System */
int32_t read_dentry_by_name(const char* fname, dentry_t* file_info);
int32_t read_dentry_by_index(uint32_t index, dentry_t* file_info);
int32_t read_data(uint32_t inode, uint32_t offset, char* buf, uint32_t length);

/* Initialization & Feature Checking Functions */
int32_t init_fs(uint32_t start, uint32_t end);
int32_t fs_initialized();
int32_t fs_inode_length(uint32_t inode_index);
int32_t read_dir(uint32_t offset, char* buf, uint32_t length);

/* Helper Functions for Testing */
//void print_information(dentry_t* dir_entry);

/* Functions dealing with directory and files */
int32_t file_open(int32_t* inode, char* filename);
int32_t file_read(int32_t* inode, uint32_t* offset, char* buf, uint32_t len);
int32_t file_write(int32_t* inode, uint32_t* offset, const char* buf, uint32_t len);
int32_t file_close(int32_t* inode);
int32_t dir_open(int32_t* inode, char* filename);
int32_t dir_read(int32_t* inode, uint32_t* offset, char* buf, uint32_t len);
int32_t dir_write(int32_t* inode, uint32_t* offset, const char* buf, uint32_t len);
int32_t dir_close(int32_t* inode);

/* Jump tables used in fs abstraction */
extern fs_jump_table_t fs_dir_jmptable;
extern fs_jump_table_t fs_file_jmptable;


#endif
