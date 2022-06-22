#ifndef WAREHOUSE_RA_H
#define WAREHOUSE_RA_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "reader.h"


#define WAREHOUSE_RA_CAPACITY 6

typedef struct Warehouse_RA
{
    Package* buffer[WAREHOUSE_RA_CAPACITY];
    size_t size;
    size_t head;
    size_t tail;
    pthread_mutex_t mutex;
    pthread_cond_t can_produce;
    pthread_cond_t can_consume;
}   Warehouse_RA;

Warehouse_RA* warehouse_ra_create(void);
void warehouse_ra_destroy(Warehouse_RA* wh);
void warehouse_ra_products_destroy(Warehouse_RA* wh);
bool warehouse_ra_is_full(const Warehouse_RA* wh);
bool warehouse_ra_is_empty(const Warehouse_RA* wh);
void warehouse_ra_put(Warehouse_RA* wh, Package* product);
Package* warehouse_ra_get(Warehouse_RA* wh);
void warehouse_ra_lock(Warehouse_RA* wh);
void warehouse_ra_unlock(Warehouse_RA* wh);
void warehouse_ra_call_producer(Warehouse_RA* wh);
void warehouse_ra_call_consumer(Warehouse_RA* wh);
void warehouse_ra_wait_for_producer(Warehouse_RA* wh);
void warehouse_ra_wait_for_consumer(Warehouse_RA* wh);


#endif
