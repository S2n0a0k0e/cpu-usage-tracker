#ifndef WAREHOUSE_AP_H
#define WAREHOUSE_AP_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include "analyzer.h"


#define WAREHOUSE_AP_CAPACITY 6

typedef struct Warehouse_AP
{
    Usage_package* buffer[WAREHOUSE_AP_CAPACITY];
    size_t size;
    size_t head;
    size_t tail;
    pthread_mutex_t mutex;
    pthread_cond_t can_produce;
    pthread_cond_t can_consume;
}   Warehouse_AP;

Warehouse_AP* warehouse_ap_create(void);
void warehouse_ap_products_destroy(Warehouse_AP* wh);
void warehouse_ap_destroy(Warehouse_AP* wh);
bool warehouse_ap_is_full(const Warehouse_AP* wh);
bool warehouse_ap_is_empty(const Warehouse_AP* wh);
void warehouse_ap_put(Warehouse_AP* wh, Usage_package* product);
Usage_package* warehouse_ap_get(Warehouse_AP* wh);
void warehouse_ap_lock(Warehouse_AP* wh);
void warehouse_ap_unlock(Warehouse_AP* wh);
void warehouse_ap_call_producer(Warehouse_AP* wh);
void warehouse_ap_call_consumer(Warehouse_AP* wh);
void warehouse_ap_wait_for_producer(Warehouse_AP* wh);
void warehouse_ap_wait_for_consumer(Warehouse_AP* wh);


#endif
