#include "threads_manager.h"
#include "printer.h"
#include "reader.h"
#include "time.h"
#include <sys/time.h>
#define ROUNDS 100



void* thread_reader(void *arg)
{

    Warehouse* wh = *(Warehouse**)arg;
    size_t iterator_v2 = 0;
    while( iterator_v2 < ROUNDS && !wh->control.end)
    {
        
            
            Package* product = cpu_info_create_package();
            warehouse_ra_lock(&wh->warehouse_ra);
            if (warehouse_ra_is_full(&wh->warehouse_ra))
                warehouse_ra_wait_for_consumer(&wh->warehouse_ra);

            if(!wh->control.end)
                warehouse_ra_put(&wh->warehouse_ra, product);
            else if(product != NULL)
            {
                for(size_t j = 0; j < product->size; j++)
                {
                    free(product->buffer[j]);
                }
                free(product);
            }
                

            warehouse_ra_call_consumer(&wh->warehouse_ra);
            warehouse_ra_unlock(&wh->warehouse_ra);
            iterator_v2++;
            control_reader_lock(&wh->control);
            wh->control.reader_check = true;

            control_reader_unlock(&wh->control);
        
        
    }
   

    return NULL;
}

void* thread_analyzer(void *arg)
{

    Warehouse* wh = *(Warehouse**)arg;
    size_t iterator = 1;
    size_t iterator_v2 = 0;
    while(!wh->control.end)
    {
        warehouse_ra_lock(&wh->warehouse_ra);

        if(warehouse_ra_is_empty(&wh->warehouse_ra))
        {
            warehouse_ra_wait_for_producer(&wh->warehouse_ra);
        }

        Package* product = NULL;
        if(!wh->control.end)
            product = warehouse_ra_get(&wh->warehouse_ra);

        warehouse_ra_call_producer(&wh->warehouse_ra);
        warehouse_ra_unlock(&wh->warehouse_ra);

        Usage_package* product_calc = NULL;
        if(!wh->control.end)
            product_calc = cpu_usage_calculate(product);
        
        warehouse_ap_lock(&wh->warehouse_ap);

        if(warehouse_ap_is_full(&wh->warehouse_ap))
        {
            warehouse_ap_wait_for_consumer(&wh->warehouse_ap);
        }
        if(!wh->control.end)
            warehouse_ap_put(&wh->warehouse_ap, product_calc);
        else if(product_calc != NULL)
        {
            for(size_t j = 0; j < product_calc->size; j++)
            {
                free(product_calc->buffer[j]);
            }
            free(product_calc);
        }
        
        iterator++;
        iterator_v2++;

        warehouse_ap_call_consumer(&wh->warehouse_ap);
        warehouse_ap_unlock(&wh->warehouse_ap);
        if(product != NULL)
        {
            for(size_t j = 0; j < product->size; j++)
            {
                free(product->buffer[j]);
            }
            free(product);
        }
        
        control_analyzer_lock(&wh->control);
        wh->control.analyzer_check = true;
        control_analyzer_unlock(&wh->control);
       
    }
    
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
    size_t iterator_v2 = 0;
    while(!wh->control.end)
    {
        while(msec <= trigger && !wh->control.end)
        {
            
            warehouse_ap_lock(&wh->warehouse_ap);
        
            if(warehouse_ap_is_empty(&wh->warehouse_ap))
            {
                warehouse_ap_wait_for_producer(&wh->warehouse_ap);
            }

            Usage_package* product = NULL;
            if(!wh->control.end)
                product = warehouse_ap_get(&wh->warehouse_ap);


            warehouse_ap_call_producer(&wh->warehouse_ap);
            warehouse_ap_unlock(&wh->warehouse_ap);


            gettimeofday(&te, NULL);
            long long miliseconds2 = te.tv_sec*1000LL + te.tv_usec/1000;
            msec = (double)(miliseconds2-miliseconds);
            
            iterator++;
            iterator_v2++;
            if(product != NULL)
            {
                tab = sum_usage_table(tab , product);
                cpu_usage_print(product);
                for(size_t j = 0; j < product->size; j++)
                {
                    free(product->buffer[j]);
                }
                free(product);
            }
            

            control_printer_lock(&wh->control);
            wh->control.printer_check = true;
            control_printer_unlock(&wh->control);      
            
        }
        if(!wh->control.end && iterator != 0)
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
    
    return NULL;

}

void* thread_watchdog(void *arg)
{

    Warehouse* wh = *(Warehouse**)arg;
    bool end = false;
    while(!end)
    {
        sleep(2);

        control_reader_lock(&wh->control);
        if(wh->control.reader_check == false)
            end = true;
        else
            wh->control.reader_check = false;
        control_reader_unlock(&wh->control);
        
        control_analyzer_lock(&wh->control);
        if(wh->control.analyzer_check == false)
            end = true;
        else
            wh->control.analyzer_check = false;
        control_analyzer_unlock(&wh->control);

        control_printer_lock(&wh->control);
        if(wh->control.printer_check == false)
            end = true;
        else
            wh->control.printer_check = false;
        control_printer_unlock(&wh->control);

    }
    printf("ERRORr");
    

    wh->control.end = true;
    usleep(0.2);
    pthread_cond_signal(&wh->warehouse_ra.can_produce);
    usleep(0.2);
    pthread_cond_signal(&wh->warehouse_ra.can_consume);
    usleep(0.2);
    pthread_cond_signal(&wh->warehouse_ap.can_produce);
    usleep(0.2);
    pthread_cond_signal(&wh->warehouse_ap.can_consume);
    
    
    return(NULL);
}
