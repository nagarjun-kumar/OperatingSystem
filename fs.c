#include "fs.h"

/* Jump table for a file directory in file sys */
fs_jump_table_t fs_dir_jmptable = {
    .open = dir_open,
    .close = dir_close,
    .read = dir_read,
    .write = dir_write
};
/* Jump table for a regular file in file sys */
fs_jump_table_t fs_file_jmptable = {
    .open = file_open,
    .close = file_close,
    .read = file_read,
    .write = file_write
};

boot_block_t* bootblk = NULL; /* Before initialization, the boot block has default value as 0 */

/* int32_t init_fs()
 * Description: A function to initialize the boot block data structure. Fails if the addresses are not valid.
 * Inputs: uint32_t start, uint32_t end  (Start and end addresses of module)
 * Output: Updated variable bootblk; returned flag to signify success/failure
 * Returned Value: Integer - Success or Failure
 * Side Effects: Changes the variable bootblk upon success.
 */
 int32_t init_fs(uint32_t start, uint32_t end) {
    boot_block_t* tmp; /* Pointer(address got from start of module address) tp a poosible well-defined boot block */
    boot_block_t* tmp_end; /* Pinter to the end of whole fs */
    uint32_t tmp_num_dir_entries; /* Number of directory entries presented regarding to start address */
    uint32_t tmp_num_inodes; /* Number of inodes presented regarding to start address */
    uint32_t tmp_num_data_blocks; /* Number of data blocks presented regarding to start address */
    tmp = (boot_block_t*) start; 
    tmp_end = (boot_block_t*) end; /* Load pointers*/
    tmp_num_dir_entries = tmp -> num_dir_entries;
    tmp_num_inodes = tmp -> num_inodes;
    tmp_num_data_blocks = tmp -> num_data_blocks; /* Load parameters */
    /* Check if the size of the file system data structure is correct */
    if ((tmp_num_dir_entries <= MAX_FILE_NUM) && (NUM_BOOT_BLOCK + tmp_num_inodes + tmp_num_data_blocks == tmp_end - tmp)) {
        bootblk = tmp; /* Load boot block upon success */
        return FS_SUCCESS;
    }
    return FS_FAILURE;
 }

 /* int32_t fs_initialized()
 * Description: Check if file system has been properly initialized.
 * Inputs: None
 * Output: Flag that signifies if file system is initialized.
 * Returned Value: Integer - Success or Failure.
 * Side Effects: None.
 */
 int32_t fs_initialized() {
     if (!bootblk) { /* If bootblk is still NULL, it's not initialized */
         return FS_FAILURE;
     }
     /* Return success elsewise */
     return FS_SUCCESS;
 }

/* int32_t fs_inode_length()
 * Description: Get the length of the inode given the inode index
 * Inputs: uint32_t inode_index
 * Output: Integer - Length of Inode
 * Returned Value: Integer - Length of Inode, or -1 if index is invalid.
 * Side Effects: None.
 */
 int32_t fs_inode_length(uint32_t inode_index) {
     inode_t* ref; /* The inode referenced */
     uint32_t ref_length; /* The length of specific file on that index */
     if ((bootblk != NULL) && (bootblk -> num_inodes > inode_index)) { /* Check initialization and valid index */
        ref = (inode_t*)bootblk + NUM_BOOT_BLOCK + inode_index; /* Calculate pointer adr=dress */
        ref_length = ref -> inode_length;
        return ref_length; /* Return upon success */
     }
     return FS_FAILURE; /* Otherwise, failure */
 }

 /* int32_t read_dentry_by_name()
 * Description: Read the file information corresponding to the file name given as argument.
 * Inputs: char* fname, dentry_t* file_info  (file name provided and a pointer we'll wrte info to)
 * Output: Updated file_info and a flag to signify success or failure
 * Returned Value: Integer - o upon success,  -1 upon failure.
 * Side Effects: Update the file_info pointer.
 */
 int32_t read_dentry_by_name(const char* fname, dentry_t* file_info) {
     int loop_idx; /* Looping index */
     dentry_t* tmp; /* Pointer to a possible matching dentry */
     int ctr;  /* Index in strings */
     char* tmp_name;  /* File name of temp */
     /* Check if boot block is initialized, if the file_info pointer is valid, and if the filename isn't too long*/
     if ((!bootblk) || (!file_info) || strlen(fname) > MAX_FILENAME_LENGTH) {
         return FS_FAILURE;
     }
     for (loop_idx = 0; loop_idx < MAX_FILE_NUM; loop_idx++) { /* Loop through all poosible files */
         tmp = &(bootblk -> files[loop_idx]); /* Load a temporary file */
         tmp_name = tmp -> file_name; /* Load file name */
         ctr = 0; /* Restore string index to frontmost */
         if (strlen(fname) == MAX_FILENAME_LENGTH) {
	        /* Case when file length is already longest */
	        while(ctr < MAX_FILENAME_LENGTH) { /* Check if characters at matching indices are the same */
	            if(fname[ctr] != tmp_name[ctr]) {
			    break; /* Break when reaching the end of one of the strings */
	            }
	        ctr++; /* increment index */
	        }
	        if (ctr == MAX_FILENAME_LENGTH) {
		        *file_info = *tmp;
		        return FS_SUCCESS;
	        } else {
		        continue;
	        }
        }
         while(fname[ctr] == tmp_name[ctr]) { /* Check if characters at matching indices are the same */
            if(fname[ctr] == '\0' || tmp_name[ctr] == '\0') {
                break; /* Break when reaching the end of one of the strings */
            }
            ctr++; /* increment index */
        }
        if(fname[ctr]=='\0' && tmp_name[ctr]=='\0') { /* If this equals, strings have same length and characters */
            *file_info = *tmp; /* Find the matching file with matching filename */
            return FS_SUCCESS; 
        }

     }
     return FS_FAILURE; /* At this point found no matches so return -1 */
 }

 /* int32_t read_dentry_by_index()
 * Description: Read the file information corresponding to the dir-index given as argument.
 * Inputs: uint32_t index, dentry_t* file_info  (dir_index provided and a pointer we'll wrte info to)
 * Output: Updated file_info and a flag to signify success or failure
 * Returned Value: Integer - o upon success,  -1 upon failure.
 * Side Effects: Update the file_info pointer.
 */
 int32_t read_dentry_by_index(uint32_t index, dentry_t* file_info) { 
     /* Check if boot block is initialized, if the file_info pointer is valid, and the index is within bound */
     if ((bootblk != NULL) && (file_info != NULL) && (index < (bootblk -> num_dir_entries))) {
         *file_info = bootblk -> files[index]; /* Load the corresponding file */
         return FS_SUCCESS;
     }
     return FS_FAILURE; /* Prerequisites not met -  return failure */
 }

 /* int32_t read_data()
 * Description: Read the data in the file from a presented inode.
 * Inputs: uint32_t inode_index, uint32_t offset, char* buf, uint32_t length
 * Output: Updated buf and a returned value to signify # bytes copied or -1 upon failure
 * Returned Value: Integer - # bytes copied upon success,  -1 upon failure.
 * Side Effects: Update the buf.
 */
 int32_t read_data(uint32_t inode, uint32_t offset, char* buf, uint32_t length) {
     inode_t* ref_inode; /* The referenced inode at referenced index */
     uint32_t length_ref; /* The copy of the number of bytes to copy */
     uint32_t front_block_idx; /* The (relative) starting data block index */
     uint32_t end_block_idx;  /* The (relative) ending data block index */
     uint32_t num_bytes_copied;  /* # bytes copied */
     uint32_t loop_idx;  /* Loop index */
     uint32_t datablk_idx; /* The relative place of data block we look through */
     data_block_t* datablk_start; /* Starting point of data blocks */
     uint32_t db_byte_start; /* Place of starting byte of a data block to copy */
     uint32_t db_byte_end;  /* Place of ending byte of a data block to copy */
     data_block_t* ref_datablk; /* The data block we refer to */
     /* Check if boot block is initialized, if the buf pointer is valid, and the index is within bound */
     if ((!bootblk) || (!buf) || (inode >= bootblk -> num_inodes)) {
         return FS_FAILURE;
     }
     length_ref = length; /* Get a copy of length */
     datablk_start = (data_block_t*) bootblk + NUM_BOOT_BLOCK + (bootblk -> num_inodes); /* Starting point of data blocks */
     ref_inode = (inode_t*)bootblk + NUM_BOOT_BLOCK + inode; /* Get the pointer pointing to referenced inode */
     if (offset >= ref_inode -> inode_length) { /* Check if offset goes beyond the inode length */
        return NO_BYTES_COPIED; /* Then no bytes can be copied */
     }
     num_bytes_copied = NO_BYTES_COPIED; /* Initialize the # bytes copied to 0 */
     if (offset + length > (ref_inode -> inode_length)) { /* Check if length is too long that we trace out of bounds */
        length_ref = (ref_inode -> inode_length) - offset; /* Truncate it to the remaining available length */
     }
     front_block_idx = offset / FS_BLOCK_SIZE; /* Get the relative starting data block (not necessarily starting at front) */
     end_block_idx = (offset + length_ref) / FS_BLOCK_SIZE; /* Relative ending data block (not nec. ending at last) */
     for (loop_idx = front_block_idx; loop_idx < end_block_idx + 1; loop_idx++) { /* Loop through data blocks */
        datablk_idx = ref_inode -> data_block[loop_idx]; /* Get the current data block */
        ref_datablk = datablk_start + datablk_idx; /* Get current data block */
        if ((loop_idx * FS_BLOCK_SIZE < offset) && (offset + length_ref < loop_idx * FS_BLOCK_SIZE + FS_BLOCK_SIZE)) {
            /* First situation: Current data block is the only data block referenced */
            db_byte_start = offset - loop_idx * FS_BLOCK_SIZE; /* Calculate starting byte add */
            db_byte_end = (offset + length_ref) - loop_idx * FS_BLOCK_SIZE; /* Caluculate ending byte add */
        } else if (loop_idx * FS_BLOCK_SIZE < offset) {
            /* Second situation : Current data block isn't the only and is the first data block referenced */
            db_byte_start = offset - loop_idx * FS_BLOCK_SIZE; /* Calculate starting byte add */
            db_byte_end = FS_BLOCK_SIZE; /* Ends at the end of block */
        } else if (offset + length_ref < loop_idx * FS_BLOCK_SIZE + FS_BLOCK_SIZE) {
            /* Third situation: Current data block isn't only and is the last data block referenced */
            db_byte_start = 0; /* Start at very front */
            db_byte_end = (offset + length_ref) - loop_idx * FS_BLOCK_SIZE; /* Caluculate ending byte add */
        } else {
            /* Fourth situation: Current data block isn't only and is the middle block referenced */
            db_byte_start = 0; /* Start at front */
            db_byte_end = FS_BLOCK_SIZE; /* Ends at last */
        }
        /* Copy the part of that data block to buf */
        memcpy((char*) buf + num_bytes_copied, (char*) ref_datablk + db_byte_start, db_byte_end - db_byte_start);
        /* Implement # bytes successfully copied */
        num_bytes_copied += (db_byte_end - db_byte_start);
     }
     return num_bytes_copied; /* Finally, return # bytes copied */
 }

/* int32_t read_dir()
 * Description: Read the directory (Only one)
 * Inputs: uint32_t offset, char* buf, uint32_t length
 * Output: Updated buf and a returned value to signify # bytes read or -1 upon failure
 * Returned Value: Integer - # bytes read upon success,  -1 upon failure.
 * Side Effects: Update the buf.
 */
int32_t read_dir(uint32_t offset, char* buf, uint32_t length) {
    uint32_t ref_length; /* Length for memory copy */
    dentry_t* ref_file; /* The file we'll refer to */
    ref_length = length; /* Initialize it to hold the same value as input length */
    /* Check if file sys is initialized, if buf pointer is valid, and if offset is out of bounds */
    if ((bootblk != NULL) && (buf != NULL) && (offset < MAX_FILE_NUM)) {
        if (length > MAX_FILENAME_LENGTH) { /* Check if length of filename is too long */
            ref_length = MAX_FILENAME_LENGTH; /* Truncate it to the maximum length */
        }
        ref_file = &(bootblk -> files[offset]); /* Load the file at offset */
        if (strlen(ref_file -> file_name) < ref_length) { /* If the file name has length less than input length */
            ref_length = strlen(ref_file -> file_name); /* Adjust the input length */
        }
        memcpy((char*)buf, (char*)(ref_file -> file_name), ref_length); /* Copy file name to buf */
        return ref_length; /* Return the length of file name copied */
    }
    return FS_FAILURE; /* If prerequisites are not met, return -1 */
}

/* int32_t file_open()
 * Description: Opens a file.
 * Inputs: int32_t* inode, char* filename  (File decriptor and file name)
 * Output: Updated file descriptor and a returned value to signify success or failure
 * Returned Value: Integer - 0 upon success,  -1 upon failure.
 * Side Effects: Update the file descriptor.
 */
int32_t file_open(int32_t* inode, char* filename) {
    dentry_t ref_dentry; /* Referenced directory entry */
    if (bootblk != NULL) { /* Check if file sys is initialized*/
        if (read_dentry_by_name(filename, &ref_dentry) == 0) { /* Read dentry and check if succeeded */
            if (ref_dentry.file_type == DEFAULT_TYPE_FILE) { /* Check type */
                *inode = ref_dentry.inode_num; /* Load the correct inode */
                return FS_SUCCESS; /* Successful */
            }
        }
    }
    return FS_FAILURE; /* Return -1 if one of the prerequisites are not met */
}

/* int32_t file_read()
 * Description: Reads a file.
 * Inputs: int32_t* inode, uint32_t offset, char* buf, uint32_t len  (File decriptor, start pos, buff, length of data)
 * Output: Updated buf, offset, and a returned value to signify bytes written or failure
 * Returned Value: Integer - bytes written upon success,  -1 upon failure.
 * Side Effects: Update buf and offset.
 */
int32_t file_read(int* inode, uint32_t* offset, char* buf, uint32_t len) {
    uint32_t current_bytes; /* Number of bytes read currently */
    if (bootblk != NULL) { /* Check if file system is initialized */
        current_bytes = read_data(*inode, *offset, buf, len); /* Read data and store # bytes read */
        if (current_bytes != FS_FAILURE) { /* If reading succeeded*/
            *offset += current_bytes; /* Update next starting place */
        }
        return current_bytes; /* Return number of bytes read , or failure */
    }
    return FS_FAILURE; /* Return failure otherwise */
}

/* int32_t file_write()
 * Description: Writes to a file. As file system is read only, always fails.
 * Inputs: int32_t* inode, uint32_t offset, char* buf, uint32_t len  (File decriptor, start pos, buff, length of data)
 * Output: Returned value to signify failure
 * Returned Value: Integer - -1 upon failure.
 * Side Effects: None.
 */
int32_t file_write(int32_t* inode, uint32_t* offset, const char* buf, uint32_t len) {
    return FS_FAILURE; /* Always fails */
}

/* int32_t file_close()
 * Description: Closes a file.
 * Inputs: int32_t* inode (File decriptor)
 * Output: Updated inode (refresh it to zero), and a returned value to signify success or failure
 * Returned Value: Integer - signifies success or failure
 * Side Effects: Update inode.
 */
int32_t file_close(int32_t* inode) {
    if (bootblk != NULL) { /* Check if file sys is initialized*/
        *inode = 0; /* Set inode to be 0 */
        return FS_SUCCESS;
    }
    return FS_FAILURE;
}

/* int32_t dir_open()
 * Description: Opens a directory.
 * Inputs: int32_t* inode, char* filename(File decriptor, file name)
 * Output: Returned value to signify success or failure
 * Returned Value: Integer - 0 upon success, -1 upon failure.
 * Side Effects: Update inode.
 */
int32_t dir_open(int32_t* inode, char* filename) {
    dentry_t ref_file; /* Referenced file */
    if (bootblk != NULL) { /* Check if file sys is initialized*/
        if (read_dentry_by_name(filename, &ref_file) == 0) { /* Read dentry and check if succeeded */
            if (ref_file.file_type == FOLDER_TYPE_FILE) { /* Check type */
                *inode = ref_file.inode_num; /* Load the correct inode */
                return FS_SUCCESS; /* Successful */
            }
        }
    }
    return FS_FAILURE; /* Return -1 if one of the prerequisites are not met */   
}

/* int32_t dir_read()
 * Description: Reads a directory.
 * Inputs: int32_t* inode, uint32_t offset, char* buf, uint32_t len  (File decriptor, start pos, buff, length of data)
 * Output: Updated buf, offset, and a returned value to signify bytes written or failure
 * Returned Value: Integer - bytes written upon success,  -1 upon failure.
 * Side Effects: Update buf and offset.
 */
int32_t dir_read(int* inode, uint32_t* offset, char* buf, uint32_t len) {
    int32_t current_length; /* Length of filename read currently */
    if (bootblk != NULL) { /* Check if file system is initialized */
        current_length = read_dir(*offset, buf, len); /* Read filename and store its length */
        if (current_length > 0) { /* If reading succeeded*/
            *offset += 1; /* Move on to next dir */
        }
        return current_length; /* Return length of filename read , or failure */
    }
    return FS_FAILURE; /* Return failure otherwise */
}

/* int32_t dir_write()
 * Description: Writes a directory. As file system is read only, always fails.
 * Inputs: int32_t* inode, uint32_t offset, char* buf, uint32_t len  (File decriptor, start pos, buff, length of data)
 * Output: Returned value to signify failure
 * Returned Value: Integer - -1 upon failure.
 * Side Effects: None.
 */
int32_t dir_write(int32_t* inode, uint32_t* offset, const char* buf, uint32_t len) {
    return FS_FAILURE; /* Always fails */
}

/* int32_t dir_close()
 * Description: Closes a directory.
 * Inputs: int32_t* inode (File decriptor)
 * Output: Returned value to signify success or failure
 * Returned Value: Integer - signifies success or failure
 * Side Effects: None.
 */
int32_t dir_close(int32_t* inode) {
    if (bootblk != NULL) { /* Check if file sys is initialized*/
        return FS_SUCCESS;
    }
    return FS_FAILURE;
}

