#ifndef ORDER_H
#define ORDER_H

#include <stdint.h>

typedef struct {
  uint64_t order_id;
  double price;
  uint64_t timestamp;
  uint32_t quantity;
  char padding[36]; // Look into alignas
} LimitOrder;

#endif /* ORDER_H */
