/*
 * Source file for paging initialization
 */

#include "paging.h"

/* void init_paging()
 * Description: A function to initialize paging functionality - page directory and page table for virtual memory implementation
 * Inputs: None
 * Output: Initialized page directory and page table.
 * Returned Value: None
 * Side Effects: Initialzies directory and page table to default settings.
 */
void init_paging() {
    uint32_t init_i; /* Loop index for the very first null initialization */
    uint32_t idx; /* Loop index to set specific values in directory and table */
    for (init_i = 0; init_i < NUM_PDE_ENTRIES; init_i++) { /* Loop through page directory */
        page_directory[init_i].val = ZERO; /* Initialize all bits to be 0 at first */
    }
    for (init_i = 0; init_i < NUM_PTE_ENTRIES; init_i++) { /* Loop through page table */
        page_table[init_i].val = ZERO; /* Initialize all bits to be 0 at first */
    }
    for (init_i = 0; init_i < NUM_PTE_ENTRIES; init_i++) { /* Loop through user vidmap page table */
        user_vidmap_page_table[init_i].val = ZERO; /* Initialize all bits to be 0 at first */
    }
    for (idx = 0; idx < NUM_PTE_ENTRIES; idx++) { /* Loop through page directory */
        if (idx == ((VID_MEM_ADD & MASK_21_12) >> TBL_OFFSET)) { /* Get bits 21~12 of Vid. Mem. Add. for corr.table idx */ 
            page_table[idx].present = ON; /* Mark presence of video memory */
            page_table[idx].read_write = ON; /* Enable read/write */
            page_table[idx + 1].present = ON;
            page_table[idx + 1].read_write = ON;
            page_table[idx + 2].present = ON;
            page_table[idx + 2].read_write = ON;
            page_table[idx + 3].present = ON;
            page_table[idx + 3].read_write = ON;
        } 
        page_table[idx].page_start_add = idx; /* Record the default (relative) page starting address */
    }
    for (idx = 0; idx < NUM_PTE_ENTRIES; idx++) { /* Initialize the vidmap page table */
        user_vidmap_page_table[idx].read_write = ON; /* R/W is always enabled */
        user_vidmap_page_table[idx].user_supervisor = ON; /* Always applicable by user */
        user_vidmap_page_table[idx].page_start_add = idx; /* Initialize default address */
    }
    page_directory[0].present_4kb = ON; /* Mark the 0MB-4MB chunk as "present" */
    page_directory[0].read_write_4kb = ON;
    page_directory[0].tbl_start_add_4kb = ((uint32_t)page_table) >> TBL_OFFSET; /* Refer to the first element in the table array */

    page_directory[1].present_4mb = ON; /* Mark the 4MB-8MB chunk as "present" */
    page_directory[1].read_write_4mb = ON; /* Enable R/W for kernal */
    page_directory[1].global_page_4mb =  ON; /* Enable global */
    page_directory[1].page_size_4mb = ON; /* Page size is 4mb */
    page_directory[1].page_start_add_4mb = KER_MEM_ADD >> DIR_OFFSET; /* Get the (relative) page starting address */

    page_directory[PT_USER_VIDMAP_LOCATION].present_4kb = ON; /* Entry for vidmap is always present */
    page_directory[PT_USER_VIDMAP_LOCATION].read_write_4kb = ON; /* Allow r/w */
    page_directory[PT_USER_VIDMAP_LOCATION].user_supervisor_4kb = ON; /* Always enabling user access */
    page_directory[PT_USER_VIDMAP_LOCATION].tbl_start_add_4kb = ((uint32_t)user_vidmap_page_table) >> TBL_OFFSET; /* Address */

    for (idx = 2; idx < NUM_PDE_ENTRIES; idx++) { /* Loop through the rest of page directory */
        if (idx == PT_USER_VIDMAP_LOCATION) { /* Index 33 has already been updated */
            continue;
        }
        page_directory[idx].page_start_add_4mb = idx; /* Default (relative) address */
    }
    /* Set cr registers */
    asm (
	    "movl $page_directory, %%eax      ;"
	    "andl $0xFFFFFC00, %%eax          ;"
	    "movl %%eax, %%cr3                ;" /* Set cr3 to be bit 31~12 of page directory base */
	    "movl %%cr4, %%eax                ;"
	    "orl $0x00000010, %%eax           ;"
	    "movl %%eax, %%cr4                ;" /* Bit 4 of cr4 set to enable 4-mbyte pages with 32 bit paging */
	    "movl %%cr0, %%eax                ;"
	    "orl $0x80000000, %%eax 	      ;"
	    "movl %%eax, %%cr0                ;" /* Set highest bit of cr0 to 1 to  enable paging */
	    : /* No outputs */
        : /* No inputs */
        : "eax", "cc"  /* Clobbers */
        );

  return;
}
