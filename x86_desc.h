/* x86_desc.h - Defines for various x86 descriptors, descriptor tables,
 * and selectors
 * vim:ts=4 noexpandtab
 */

#ifndef _X86_DESC_H
#define _X86_DESC_H

#include "types.h"

/* Constants referenced when paging */
#define NUM_PDE_ENTRIES  1024  /** 10 address bits so 2^10 entries */
#define NUM_PTE_ENTRIES  1024  /** 10 address bits so 2^10 entries */
#define BYTES_PER_ENTRY  4     /** 32 bits = 4 bytes */
#define PDE_SIZE         NUM_PDE_ENTRIES * BYTES_PER_ENTRY
#define PTE_SIZE         NUM_PTE_ENTRIES * BYTES_PER_ENTRY
/* Segment selector values */
#define KERNEL_CS   0x0010
#define KERNEL_DS   0x0018
#define USER_CS     0x0023
#define USER_DS     0x002B
#define KERNEL_TSS  0x0030
#define KERNEL_LDT  0x0038

/* Size of the task state segment (TSS) */
#define TSS_SIZE    104

/* Number of vectors in the interrupt descriptor table (IDT) */
#define NUM_VEC     256

#ifndef ASM

/* This structure is used to load descriptor base registers
 * like the GDTR and IDTR */
typedef struct x86_desc {
    uint16_t padding;
    uint16_t size;
    uint32_t addr;
} x86_desc_t;

/* This is a segment descriptor.  It goes in the GDT. */
typedef struct seg_desc {
    union {
        uint32_t val[2];
        struct {
            uint16_t seg_lim_15_00;
            uint16_t base_15_00;
            uint8_t  base_23_16;
            uint32_t type          : 4;
            uint32_t sys           : 1;
            uint32_t dpl           : 2;
            uint32_t present       : 1;
            uint32_t seg_lim_19_16 : 4;
            uint32_t avail         : 1;
            uint32_t reserved      : 1;
            uint32_t opsize        : 1;
            uint32_t granularity   : 1;
            uint8_t  base_31_24;
        } __attribute__ ((packed));
    };
} seg_desc_t;

/* TSS structure */
typedef struct __attribute__((packed)) tss_t {
    uint16_t prev_task_link;
    uint16_t prev_task_link_pad;

    uint32_t esp0;
    uint16_t ss0;
    uint16_t ss0_pad;

    uint32_t esp1;
    uint16_t ss1;
    uint16_t ss1_pad;

    uint32_t esp2;
    uint16_t ss2;
    uint16_t ss2_pad;

    uint32_t cr3;

    uint32_t eip;
    uint32_t eflags;

    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t esp;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;

    uint16_t es;
    uint16_t es_pad;

    uint16_t cs;
    uint16_t cs_pad;

    uint16_t ss;
    uint16_t ss_pad;

    uint16_t ds;
    uint16_t ds_pad;

    uint16_t fs;
    uint16_t fs_pad;

    uint16_t gs;
    uint16_t gs_pad;

    uint16_t ldt_segment_selector;
    uint16_t ldt_pad;

    uint16_t debug_trap : 1;
    uint16_t io_pad     : 15;
    uint16_t io_base_addr;
} tss_t;

/* Some external descriptors declared in .S files */
extern x86_desc_t gdt_desc;

extern uint16_t ldt_desc;
extern uint32_t ldt_size;
extern seg_desc_t ldt_desc_ptr;
extern seg_desc_t gdt_ptr;
extern uint32_t ldt;

extern uint32_t tss_size;
extern seg_desc_t tss_desc_ptr;
extern tss_t tss;

/* Entries of a page directory (switches between 4KB / 4 MB page tables) */
typedef struct pde {
    union{
        uint32_t val; /* A general 32-bit value representation of pde. Set to 0 for null initialization */
        struct { /* Case when page size is 4kb and page tables implemented */
            uint32_t present_4kb         : 1; /* If set, page is actually in physical memory at the moment. || Bit 0 */ 
            uint32_t read_write_4kb      : 1; /* If set, read/write; if not, read only || Bit 1 */
            uint32_t user_supervisor_4kb : 1; /* If set, page can be accessed by all || Bit 2 */
            uint32_t write_through_4kb   : 1; /* If set, write-through caching is enabled || Bit 3 */
            uint32_t cache_disabled_4kb  : 1; /* If set, page will not be cached || Bit 4 */
            uint32_t accessed_4kb        : 1; /* Whether a page has been read or written to || Bit 5 */
            uint32_t reserved_4kb        : 1; /* Reserved || Bit 6 */
            uint32_t page_size_4kb       : 1; /* If the bit is set, pages are 4 MiB in size. Otherwise 4 KiB || Bit 7 */
            uint32_t global_page_4kb     : 1; /* Signifies if page is global/local || Bit 8 */
            uint32_t avail_4kb           : 3; /* Free bits for the OS to store own accounting information || Bits 11 ~9 */
            uint32_t tbl_start_add_4kb   : 20; /* Page table 4kb-aligned address || Bits 31~12 */
        } __attribute__ ((packed)); /* Align it to be packed i.e. 32 bits in total */
        struct { /* Case when page size is 4mb */
            uint32_t present_4mb         : 1; /* If set, page is actually in physical memory at the moment. || Bit 0 */ 
            uint32_t read_write_4mb      : 1; /* If set, read/write; if not, read only || Bit 1 */
            uint32_t user_supervisor_4mb : 1; /* If set, page can be accessed by all || Bit 2 */
            uint32_t write_through_4mb   : 1; /* If set, write-through caching is enabled || Bit 3 */
            uint32_t cache_disabled_4mb  : 1; /* If set, page will not be cached || Bit 4 */
            uint32_t accessed_4mb        : 1; /* Whether a page has been read or written to || Bit 5 */
            uint32_t dirty_4mb           : 1; /* Set if such table has been written to  || Bit 6 */
            uint32_t page_size_4mb       : 1; /* If the bit is set, pages are 4 MiB in size. Otherwise 4 KiB || Bit 7 */
            uint32_t global_page_4mb     : 1; /* Signifies if page is global/local || Bit 8 */
            uint32_t avail_4mb           : 3; /* Free bits for the OS to store own accounting information || Bits 11 ~9 */
            uint32_t pat_memory_type_4mb : 1; /* The memory type given by the page attribute table || Bit 12 */
            uint32_t reserved_4mb        : 9; /* Reserved || Bits 21~13 */
            uint32_t page_start_add_4mb  : 10; /* Page starting address || Bits 31~22 */
        } __attribute__ ((packed)); /* Align it to be packed i.e. 32 bits in total */
    };
} pde_instance; /* Finished pde data structure */

/* Entries of a page table */
typedef struct pte {
    union {
        uint32_t val; /* A general 32-bit representation of pte. Set to 0 for null initialization */
        struct { /* Elements of a pte */
            uint32_t present             : 1; /* If set, page is actually in physical memory at the moment. || Bit 0 */ 
            uint32_t read_write          : 1; /* If set, read/write; if not, read only || Bit 1 */
            uint32_t user_supervisor     : 1; /* If set, page can be accessed by all || Bit 2 */
            uint32_t write_through       : 1; /* If set, write-through caching is enabled || Bit 3 */
            uint32_t cache_disabled      : 1; /* If set, page will not be cached || Bit 4 */
            uint32_t accessed            : 1; /* Whether a page has been read or written to || Bit 5 */
            uint32_t dirty               : 1; /* Set if such page has been writtem to || Bit 6 */
            uint32_t pat_memory_type     : 1; /* The memory type given by the page attribute table || Bit 7 */
            uint32_t global_page         : 1; /* Signifies if page is global/local || Bit 8 */
            uint32_t avail               : 3; /* Free bits for the OS to store own accounting information || Bits 11 ~9 */
            uint32_t page_start_add      : 20; /* Page table 4kb-aligned address || Bits 31~12 */
        } __attribute__ ((packed)); /* Align it to be packed i.e. 32 bits in total */
    };
} pte_instance; /* Finished pte data structure */

/* Aligned full page directory */
pde_instance page_directory[NUM_PDE_ENTRIES] __attribute__ ((aligned (PDE_SIZE)));

/* Aligned full page table */
pte_instance page_table[NUM_PTE_ENTRIES] __attribute__ ((aligned (PTE_SIZE)));

/* Aligned full page table for user vidmap */
pte_instance user_vidmap_page_table[NUM_PTE_ENTRIES] __attribute__((aligned (PTE_SIZE)));

/* Sets runtime-settable parameters in the GDT entry for the LDT */
#define SET_LDT_PARAMS(str, addr, lim)                          \
do {                                                            \
    str.base_31_24 = ((uint32_t)(addr) & 0xFF000000) >> 24;     \
    str.base_23_16 = ((uint32_t)(addr) & 0x00FF0000) >> 16;     \
    str.base_15_00 = (uint32_t)(addr) & 0x0000FFFF;             \
    str.seg_lim_19_16 = ((lim) & 0x000F0000) >> 16;             \
    str.seg_lim_15_00 = (lim) & 0x0000FFFF;                     \
} while (0)

/* Sets runtime parameters for the TSS */
#define SET_TSS_PARAMS(str, addr, lim)                          \
do {                                                            \
    str.base_31_24 = ((uint32_t)(addr) & 0xFF000000) >> 24;     \
    str.base_23_16 = ((uint32_t)(addr) & 0x00FF0000) >> 16;     \
    str.base_15_00 = (uint32_t)(addr) & 0x0000FFFF;             \
    str.seg_lim_19_16 = ((lim) & 0x000F0000) >> 16;             \
    str.seg_lim_15_00 = (lim) & 0x0000FFFF;                     \
} while (0)

/* An interrupt descriptor entry (goes into the IDT) */
typedef union idt_desc_t {
    uint32_t val[2];
    struct {
        uint16_t offset_15_00;
        uint16_t seg_selector;
        uint8_t  reserved4;
        uint32_t reserved3 : 1;
        uint32_t reserved2 : 1;
        uint32_t reserved1 : 1;
        uint32_t size      : 1;
        uint32_t reserved0 : 1;
        uint32_t dpl       : 2;
        uint32_t present   : 1;
        uint16_t offset_31_16;
    } __attribute__ ((packed));
} idt_desc_t;

/* The IDT itself (declared in x86_desc.S */
extern idt_desc_t idt[NUM_VEC];
/* The descriptor used to load the IDTR */
extern x86_desc_t idt_desc_ptr;

/* Sets runtime parameters for an IDT entry */
#define SET_IDT_ENTRY(str, handler)                              \
do {                                                             \
    str.offset_31_16 = ((uint32_t)(handler) & 0xFFFF0000) >> 16; \
    str.offset_15_00 = ((uint32_t)(handler) & 0xFFFF);           \
} while (0)

/* Load task register.  This macro takes a 16-bit index into the GDT,
 * which points to the TSS entry.  x86 then reads the GDT's TSS
 * descriptor and loads the base address specified in that descriptor
 * into the task register */
#define ltr(desc)                       \
do {                                    \
    asm volatile ("ltr %w0"             \
            :                           \
            : "r" (desc)                \
            : "memory", "cc"            \
    );                                  \
} while (0)

/* Load the interrupt descriptor table (IDT).  This macro takes a 32-bit
 * address which points to a 6-byte structure.  The 6-byte structure
 * (defined as "struct x86_desc" above) contains a 2-byte size field
 * specifying the size of the IDT, and a 4-byte address field specifying
 * the base address of the IDT. */
#define lidt(desc)                      \
do {                                    \
    asm volatile ("lidt (%0)"           \
            :                           \
            : "g" (desc)                \
            : "memory"                  \
    );                                  \
} while (0)

/* Load the local descriptor table (LDT) register.  This macro takes a
 * 16-bit index into the GDT, which points to the LDT entry.  x86 then
 * reads the GDT's LDT descriptor and loads the base address specified
 * in that descriptor into the LDT register */
#define lldt(desc)                      \
do {                                    \
    asm volatile ("lldt %%ax"           \
            :                           \
            : "a" (desc)                \
            : "memory"                  \
    );                                  \
} while (0)

#endif /* ASM */

#endif /* _x86_DESC_H */
