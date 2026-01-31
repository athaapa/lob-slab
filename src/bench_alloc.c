#define _POSIX_C_SOURCE 199309L
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "slab.h"

#define NUM_ORDERS 1000000
#define CHURN_BATCH 100
#define CHURN_ITER 10000

int main() {
    struct timespec start, end;
    long long malloc_time, slab_time, malloc_dealloc_time, slab_dealloc_time, malloc_churn_time, slab_churn_time;

    // Benchmark malloc
    clock_gettime(CLOCK_MONOTONIC, &start);
    LimitOrder **malloc_orders = malloc(NUM_ORDERS * sizeof(LimitOrder*));
    for (int i = 0; i < NUM_ORDERS; i++) {
        malloc_orders[i] = malloc(sizeof(LimitOrder));
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    malloc_time = (end.tv_sec - start.tv_sec) * 1000000000LL + (end.tv_nsec - start.tv_nsec);

    // Dealloc malloc
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = 0; i < NUM_ORDERS; i++) {
        free(malloc_orders[i]);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    free(malloc_orders);
    malloc_dealloc_time = (end.tv_sec - start.tv_sec) * 1000000000LL + (end.tv_nsec - start.tv_nsec);

    // Initialize slab allocator
    size_t mem_size = 64 * NUM_ORDERS;  // 64 bytes per LimitOrder
    void *mem_block = malloc(mem_size);
    slab_init(mem_block, mem_size);

    // Benchmark slab_alloc
    clock_gettime(CLOCK_MONOTONIC, &start);
    LimitOrder **slab_orders = malloc(NUM_ORDERS * sizeof(LimitOrder*));
    for (int i = 0; i < NUM_ORDERS; i++) {
        slab_orders[i] = slab_alloc();
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    slab_time = (end.tv_sec - start.tv_sec) * 1000000000LL + (end.tv_nsec - start.tv_nsec);

    // Dealloc slab
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int i = NUM_ORDERS - 1; i >= 0; i--) {
        slab_free(slab_orders[i]);
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    slab_dealloc_time = (end.tv_sec - start.tv_sec) * 1000000000LL + (end.tv_nsec - start.tv_nsec);

    free(slab_orders);
    free(mem_block);

    // Churn benchmark malloc
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int iter = 0; iter < CHURN_ITER; iter++) {
        LimitOrder *batch[CHURN_BATCH];
        for (int i = 0; i < CHURN_BATCH; i++) {
            batch[i] = malloc(sizeof(LimitOrder));
        }
        for (int i = 0; i < CHURN_BATCH; i++) {
            free(batch[i]);
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    malloc_churn_time = (end.tv_sec - start.tv_sec) * 1000000000LL + (end.tv_nsec - start.tv_nsec);

    // Churn benchmark slab
    size_t churn_mem_size = 64 * CHURN_BATCH;
    void *churn_mem_block = malloc(churn_mem_size);
    slab_init(churn_mem_block, churn_mem_size);
    clock_gettime(CLOCK_MONOTONIC, &start);
    for (int iter = 0; iter < CHURN_ITER; iter++) {
        LimitOrder *batch[CHURN_BATCH];
        for (int i = 0; i < CHURN_BATCH; i++) {
            batch[i] = slab_alloc();
        }
        for (int i = CHURN_BATCH - 1; i >= 0; i--) {
            slab_free(batch[i]);
        }
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    slab_churn_time = (end.tv_sec - start.tv_sec) * 1000000000LL + (end.tv_nsec - start.tv_nsec);
    free(churn_mem_block);

    // Calculate Per-Op Latencies for cleaner printing
    double malloc_alloc_op  = (double)malloc_time / NUM_ORDERS;
    double slab_alloc_op    = (double)slab_time / NUM_ORDERS;
    
    double malloc_free_op   = (double)malloc_dealloc_time / NUM_ORDERS;
    double slab_free_op     = (double)slab_dealloc_time / NUM_ORDERS;
    
    double malloc_churn_op  = (double)malloc_churn_time / (CHURN_ITER * CHURN_BATCH * 2);
    double slab_churn_op    = (double)slab_churn_time / (CHURN_ITER * CHURN_BATCH * 2);

    // Print The Executive Summary
    printf("\n");
    printf("================================================================================\n");
    printf("FINAL BENCHMARK REPORT: 64-BYTE SLAB ALLOCATOR\n");
    printf("Platform: x86-64 (attu) | Resolution: Nanoseconds\n");
    printf("================================================================================\n");
    printf("%-24s | %14s | %14s | %14s\n", "Operation", "Malloc (ns/op)", "Slab (ns/op)", "Speedup");
    printf("-------------------------|----------------|----------------|----------------\n");
    
    printf("%-24s | %14.2f | %14.2f | %13.2fx\n", 
           "Allocation (Linear)", malloc_alloc_op, slab_alloc_op, malloc_alloc_op / slab_alloc_op);
           
    printf("%-24s | %14.2f | %14.2f | %13.2fx\n", 
           "Deallocation (Linear)", malloc_free_op, slab_free_op, malloc_free_op / slab_free_op);
           
    printf("%-24s | %14.2f | %14.2f | %13.2fx\n", 
           "Hot Churn (100 batch)", malloc_churn_op, slab_churn_op, malloc_churn_op / slab_churn_op);
           
    printf("================================================================================\n");

    return 0;
}
