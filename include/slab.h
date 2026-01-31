#ifndef SLAB_H
#define SLAB_H

#include "order.h"
#include <stddef.h>

/**
 * Initializes the allocator with a raw memory block.
 * @param memory_block Pointer to the start of the memory
 * @param mem_size Total size of the block in memory
 */
void slab_init(void *memory_block, size_t mem_size);

/**
 * Returns a pointer to a free 64-byte slot.
 * @return Pointer to a LimitOrder, or NULL if OOM.
 */
LimitOrder *slab_alloc();

/**
 * Returns a used slot to the free list.
 * @param ptr Pointer to the LimitOrder to be freed.
 */
void slab_free(LimitOrder *ptr);

#endif
