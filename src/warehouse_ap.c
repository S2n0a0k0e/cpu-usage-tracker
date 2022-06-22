#include <pthread.h>
#include "warehouse_ap.h"



Warehouse_AP* warehouse_ap_create(void)
{
    Warehouse_AP* warehouse = malloc(sizeof(*warehouse) + sizeof(*warehouse->buffer));
    if(warehouse == NULL)
        return NULL;
    
    *warehouse = (Warehouse_AP){.buffer = {0},
                                .size = 0,
                                .head = 0,
                                .tail = 0,
                                .mutex = PTHREAD_MUTEX_INITIALIZER,
                                .can_produce = PTHREAD_COND_INITIALIZER,
                                .can_consume = PTHREAD_COND_INITIALIZER
                                };
    return warehouse;
}

void warehouse_ap_products_destroy(Warehouse_AP* wh)
{
    while(wh->size > 0)
    {
        Usage_package* product = warehouse_ap_get(wh);
        for(size_t j = 0; j < product->size; j++)
            free(product->buffer[j]);
            
        free(product);
    }
}

void warehouse_ap_destroy(Warehouse_AP* wh)
{

    pthread_cond_destroy(&wh->can_consume);
    pthread_cond_destroy(&wh->can_produce);
    pthread_mutex_destroy(&wh ->mutex);
    
    free(wh);
}

bool warehouse_ap_is_full(const Warehouse_AP* wh)
{
    return wh->size == WAREHOUSE_AP_CAPACITY;
}

bool warehouse_ap_is_empty(const Warehouse_AP* wh)
{
    return wh->size == 0;
}

void warehouse_ap_put(Warehouse_AP* wh, Usage_package* product)
{
    if(product == NULL)
        return;
    if (warehouse_ap_is_full(wh))
        return;
    
    wh->buffer[wh->head] = product;
    wh->head = (wh->head + 1) % WAREHOUSE_AP_CAPACITY;
    ++wh->size;
}

Usage_package* warehouse_ap_get(Warehouse_AP* wh)
{
    if(warehouse_ap_is_empty(wh))
        return NULL;
    
    Usage_package* product = wh->buffer[wh->tail];
    wh->tail = (wh->tail + 1) % WAREHOUSE_AP_CAPACITY;
    --wh->size;

    return product;
}

void warehouse_ap_lock(Warehouse_AP* wh)
{
    pthread_mutex_lock(&wh->mutex);
}

void warehouse_ap_unlock(Warehouse_AP* wh)
{
    pthread_mutex_unlock(&wh->mutex);
}

void warehouse_ap_call_producer(Warehouse_AP* wh)
{
    pthread_cond_signal(&wh->can_produce);
}

void warehouse_ap_call_consumer(Warehouse_AP* wh)
{
    pthread_cond_signal(&wh->can_consume);
}

void warehouse_ap_wait_for_producer(Warehouse_AP* wh)
{
    pthread_cond_wait(&wh->can_consume, &wh->mutex);
}

void warehouse_ap_wait_for_consumer(Warehouse_AP* wh)
{
    pthread_cond_wait(&wh->can_produce, &wh->mutex);
}
