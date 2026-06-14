#ifndef GD_MEMORY_H
#define GD_MEMORY_H

#include <PR/ultratypes.h>

struct GMemBlock {
     u8 *ptr;
     u32 size;
     u8 blockType;
     u8 permFlag;
     struct GMemBlock *next;
     struct GMemBlock *prev;
};

enum GMemBlockTypes {
    G_MEM_BLOCK_FREE = 1,
    G_MEM_BLOCK_USED = 2
};

#define PERM_G_MEM_BLOCK 0xF0
#define TEMP_G_MEM_BLOCK 0x0F

extern u32 gd_free_mem(void *ptr);
extern void *gd_request_mem(u32 size, u8 permanence);
extern struct GMemBlock *gd_add_mem_to_heap(u32 size, void *addr, u8 permanence);
extern void init_mem_block_lists(void);
extern void mem_stats(void);

#endif
