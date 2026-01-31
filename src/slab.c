#include "slab.h"
#include "order.h"
#include <assert.h>

static void* free_list_head;

void slab_init(void *memory_block, size_t mem_size) {
  assert((uintptr_t)memory_block % 8 == 0);
  free_list_head = memory_block;
  size_t total_size = mem_size / sizeof(LimitOrder);
  void** current = free_list_head;
  for (size_t i = 0; i < total_size - 1; i++) {
    void* next = (char*) current + sizeof(LimitOrder);
    *current = next;
    current = (void**)next;
  }
  *current = NULL;
}

LimitOrder *slab_alloc() {
  if (free_list_head == NULL) {
    return NULL;
  }
  void* next = (*(void**)free_list_head);
  LimitOrder* order = (LimitOrder*) (free_list_head);
  free_list_head = next;
  return order;
}

void slab_free(LimitOrder *ptr) {
  void** new_list_head = (void**) ptr;
  *new_list_head = free_list_head;
  free_list_head = new_list_head;
}
