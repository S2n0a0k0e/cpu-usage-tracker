#include <pthread.h>
#include "warehouse_ra.h"



Warehouse_RA* warehouse_ra_create(void)
{
    Warehouse_RA* warehouse = malloc(sizeof(*warehouse) + sizeof(*warehouse->buffer));
    if(warehouse == NULL)
        return NULL;
    
    *warehouse = (Warehouse_RA){.buffer = {0},
                                .size = 0,
                                .head = 0,
                                .tail = 0,
                                .mutex = PTHREAD_MUTEX_INITIALIZER,
                                .can_produce = PTHREAD_COND_INITIALIZER,
                                .can_consume = PTHREAD_COND_INITIALIZER
                                };
    return warehouse;
}

void warehouse_ra_destroy(Warehouse_RA* wh)
{
    pthread_cond_destroy(&wh->can_consume);
    pthread_cond_destroy(&wh->can_produce);
    pthread_mutex_destroy(&wh ->mutex);
    
    free(wh);
}

void warehouse_ra_products_destroy(Warehouse_RA* wh)
{
    while(wh->size > 0)
    {
        Package* product = warehouse_ra_get(wh);
        for(size_t j = 0; j < product->size; j++)   
            free(product->buffer[j]);
            
        free(product);
    }
}

bool warehouse_ra_is_full(const Warehouse_RA* wh)
{
    return wh->size == WAREHOUSE_RA_CAPACITY;
}

bool warehouse_ra_is_empty(const Warehouse_RA* wh)
{
    return wh->size == 0;
}

void warehouse_ra_put(Warehouse_RA* wh, Package* product)
{
    if(product == NULL)
        return;
    if (warehouse_ra_is_full(wh))
        return;
    
    wh->buffer[wh->head] = product;
    wh->head = (wh->head + 1) % WAREHOUSE_RA_CAPACITY;
    ++wh->size;
}

Package* warehouse_ra_get(Warehouse_RA* wh)
{
    if(warehouse_ra_is_empty(wh))
        return NULL;
    
    Package* product = wh->buffer[wh->tail];
    wh->tail = (wh->tail + 1) % WAREHOUSE_RA_CAPACITY;
    --wh->size;

    return product;
}

void warehouse_ra_lock(Warehouse_RA* wh)
{
    pthread_mutex_lock(&wh->mutex);
}

void warehouse_ra_unlock(Warehouse_RA* wh)
{
    pthread_mutex_unlock(&wh->mutex);
}

void warehouse_ra_call_producer(Warehouse_RA* wh)
{
    pthread_cond_signal(&wh->can_produce);
}

void warehouse_ra_call_consumer(Warehouse_RA* wh)
{
    pthread_cond_signal(&wh->can_consume);
}

void warehouse_ra_wait_for_producer(Warehouse_RA* wh)
{
    pthread_cond_wait(&wh->can_consume, &wh->mutex);
}

void warehouse_ra_wait_for_consumer(Warehouse_RA* wh)
{
    pthread_cond_wait(&wh->can_produce, &wh->mutex);
}


