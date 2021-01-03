/*
 * Header File. Contains neccessary variables and functions to initialize paging
 */
 
#ifndef _PAGING_H 
#define _PAGING_H  /* Prevents double declaration of any identifiers */

#include "types.h"
#include "lib.h"
#include "x86_desc.h" /* Page directory and page table defined there */

#define OFF         0 /* Boolean value "off" to assign to the lower bits of directory and table */
#define ZERO        0 /* Zero value to assign to val to initialize the directory and the table */
#define ON          1 /* Boolean value "on" to assign to the lower bits of directory and table */
#define TBL_OFFSET  12  /* A page can contain 12 address bits so we'll only consider the higher 20 bits of table */
#define VID_MEM_ADD 0x000B8000  /* Address for video memory */
#define MASK_21_12  0x003FF000  /** Bitmask for bits between 21 and 12(inclusive) */ 
#define KER_MEM_ADD 0x00400000  /* Kernal loaded at 4MB */
#define DIR_OFFSET  22  /* Offset ofor page directory */

#define PT_USER_VIDMAP_LOCATION  33  /* The page table for user vidmap is 4 * 33 = 132 MB away from start of PD */

 /* Function to initialize paging */
 extern void init_paging();

 #endif

