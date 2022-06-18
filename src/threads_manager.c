#include "threads_manager.h"
#include "printer.h"
#include "reader.h"
#include "time.h"
#include <sys/time.h>



void* thread_reader(void *arg)
{
    Warehouse* wh = *(Warehouse**)arg;

    for(size_t i = 0; i < 21; i++)
    {
        
        
            // Cpu_info* product = cpu_read_info(f);
            
            Package* product = cpu_info_create_package();
            warehouse_ra_lock(&wh->warehouse_ra);
            if (warehouse_ra_is_full(&wh->warehouse_ra))
                warehouse_ra_wait_for_consumer(&wh->warehouse_ra);
            
            warehouse_ra_put(&wh->warehouse_ra, product);

            warehouse_ra_call_consumer(&wh->warehouse_ra);
            warehouse_ra_unlock(&wh->warehouse_ra);
        
        
        
    }
    printf("r");
    return NULL;
}

void* thread_analyzer(void *arg)
{
    Warehouse* wh = *(Warehouse**)arg;
    size_t iterator = 1;
    for(size_t i = 0; i < 21; i++)
    {
        warehouse_ra_lock(&wh->warehouse_ra);

        if(warehouse_ra_is_empty(&wh->warehouse_ra))
        {
            warehouse_ra_wait_for_producer(&wh->warehouse_ra);
        }

        Package* product = warehouse_ra_get(&wh->warehouse_ra);

        warehouse_ra_call_producer(&wh->warehouse_ra);
        warehouse_ra_unlock(&wh->warehouse_ra);

        Usage_package* product_calc = cpu_usage_calculate(product);
        
        warehouse_ap_lock(&wh->warehouse_ap);

        if(warehouse_ap_is_full(&wh->warehouse_ap))
        {
            warehouse_ap_wait_for_consumer(&wh->warehouse_ap);
        }

        warehouse_ap_put(&wh->warehouse_ap, product_calc);
        printf("grrrrrrrrrrrrrrr %zu %zu\n",iterator, wh->warehouse_ap.size);
        iterator++;

        warehouse_ap_call_consumer(&wh->warehouse_ap);
        warehouse_ap_unlock(&wh->warehouse_ap);
        for(size_t j = 0; j < product->size; j++)
        {
            free(product->buffer[j]);
        }
        free(product);
       
    }
     printf("a");
    return NULL;
}

void* thread_printer(void *arg)
{
    Warehouse* wh = *(Warehouse**)arg;
    double msec = 0;
    double trigger = 1000;
    struct timeval te;
    gettimeofday(&te, NULL);
    long long miliseconds = te.tv_sec*1000LL + te.tv_usec/1000;
    double* tab = malloc(sizeof(*tab)*(cpu_count() + 1));
    for(size_t i = 0; i < cpu_count() + 1; i++)
    {
        tab[i] = 0;
    }
    size_t iterator = 0;
    for(size_t i = 0; i < 2; i++)
    {
        while(msec <= trigger)
        {
            warehouse_ap_lock(&wh->warehouse_ap);
        
            if(warehouse_ap_is_empty(&wh->warehouse_ap))
            {
                warehouse_ap_wait_for_producer(&wh->warehouse_ap);
            }

            Usage_package* product = warehouse_ap_get(&wh->warehouse_ap);


            warehouse_ap_call_producer(&wh->warehouse_ap);
            warehouse_ap_unlock(&wh->warehouse_ap);


            gettimeofday(&te, NULL);
            long long miliseconds2 = te.tv_sec*1000LL + te.tv_usec/1000;
            msec = (double)(miliseconds2-miliseconds);
            tab = sum_usage_table(tab , product);
            iterator++;
            cpu_usage_print(product);
            for(size_t j = 0; j < product->size; j++)
            {
                free(product->buffer[j]);
            }
            free(product);
            printf("frrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr %zu\n",iterator);
            
        }
        average_usage_print(tab, iterator);
        for(size_t j = 0; j < cpu_count() + 1; j++)
        {
            tab[j] = 0;
        }

        iterator = 0;
        msec = 0;
        gettimeofday(&te, NULL);
        miliseconds = te.tv_sec*1000LL + te.tv_usec/1000;
             
    }
    free(tab);
    
    printf("p");

    return NULL;

}
