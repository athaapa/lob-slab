#include <assert.h>
#include <stddef.h>
#include "order.h"
#include <stdio.h>

int main() {
  printf("Layout Verification\n");
  printf("Size of LimitOrder struct: %zu bytes\n", sizeof(LimitOrder));
  assert(sizeof(LimitOrder) == 64);
  printf("Offset of order_id: %zu\n", offsetof(LimitOrder, order_id));
  printf("Offset of quantity: %zu\n", offsetof(LimitOrder, quantity));
  printf("Offset of padding: %zu\n", offsetof(LimitOrder, padding));
  assert(offsetof(LimitOrder, padding) == 28);
  printf("LAYOUT VERIFIED\n");
  return 0;
}
