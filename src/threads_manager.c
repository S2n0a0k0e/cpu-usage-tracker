#include "threads_manager.h"
#include "printer.h"
#include "reader.h"
#include "time.h"
#include "logger.h"
#include <sys/time.h>
#define ROUNDS 100
extern bool done;


void* thread_reader(void *arg)
{

    Warehouse* wh = *(Warehouse**)arg;
    size_t iterator_v2 = 0;
    while( iterator_v2 < ROUNDS && !wh->control.end)
    {
        
            
            Package* product = cpu_info_create_package();
            
            Logger_elem* logger_read_file = NULL;
            if(!wh->control.end)
                logger_read_file = create_message_to_send(INFO, READ_FILE);
            
            logger_lock(&wh->logger);
            if (logger_is_full(&wh->logger))
                logger_wait_for_consumer(&wh->logger);
            if(!wh->control.end)
                logger_put(&wh->logger, logger_read_file);
            else if(logger_read_file != NULL)
                free(logger_read_file);
            logger_call_consumer(&wh->logger);
            logger_unlock(&wh->logger);
            // printf("SIIIIIIIZEE %zu\n", wh->logger.size);

                


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


            Logger_elem* logger_send_data = NULL;
            if(!wh->control.end)
                logger_send_data = create_message_to_send(INFO, SEND_DATA_TO_ANALYZER);

            logger_lock(&wh->logger);
            if (logger_is_full(&wh->logger))
                logger_wait_for_consumer(&wh->logger);
            if(!wh->control.end)
                logger_put(&wh->logger, logger_send_data);
            else if(logger_send_data != NULL)
                free(logger_send_data);
            logger_call_consumer(&wh->logger);
            logger_unlock(&wh->logger);

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

        Logger_elem* logger_calculate = NULL;
        if(!wh->control.end)
            logger_calculate = create_message_to_send(INFO, ANALYZE);


        logger_lock(&wh->logger);
        if (logger_is_full(&wh->logger))
            logger_wait_for_consumer(&wh->logger);
        if(!wh->control.end)
            logger_put(&wh->logger, logger_calculate);
        else if(logger_calculate != NULL)
                free(logger_calculate);
        logger_call_consumer(&wh->logger);
        logger_unlock(&wh->logger); 


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

        Logger_elem* logger_send = NULL;
            if(!wh->control.end)
                logger_send = create_message_to_send(INFO, SEND_DATA_TO_PRINTER);

            logger_lock(&wh->logger);
            if (logger_is_full(&wh->logger))
                logger_wait_for_consumer(&wh->logger);
            if(!wh->control.end)
                logger_put(&wh->logger, logger_send);
            else if(logger_send != NULL)
                free(logger_send);
            logger_call_consumer(&wh->logger);
            logger_unlock(&wh->logger);

        
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


            Logger_elem* logger_get = NULL;
            if(!wh->control.end)
                logger_get = create_message_to_send(INFO, PRINTER_GET_DATA);

            logger_lock(&wh->logger);
            if (logger_is_full(&wh->logger))
                logger_wait_for_consumer(&wh->logger);
            if(!wh->control.end)
                logger_put(&wh->logger, logger_get);
            else if(logger_get != NULL)
                free(logger_get);
            logger_call_consumer(&wh->logger);
            logger_unlock(&wh->logger);


            gettimeofday(&te, NULL);
            long long miliseconds2 = te.tv_sec*1000LL + te.tv_usec/1000;
            msec = (double)(miliseconds2-miliseconds);
            
            iterator++;
            iterator_v2++;
            if(product != NULL)
            {
                tab = sum_usage_table(tab , product);
                // cpu_usage_print(product);
                for(size_t j = 0; j < product->size; j++)
                {
                    free(product->buffer[j]);
                }
                free(product);
            }

            Logger_elem* logger_send = NULL;
            if(!wh->control.end)
                logger_send = create_message_to_send(INFO, PRINT);

            logger_lock(&wh->logger);
            if (logger_is_full(&wh->logger))
                logger_wait_for_consumer(&wh->logger);
            if(!wh->control.end)
                logger_put(&wh->logger, logger_send);
            else if(logger_send != NULL)
                free(logger_send);
            logger_call_consumer(&wh->logger);
            logger_unlock(&wh->logger);
            

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
    while(!end && !done)
    {
        sleep(2);

        control_reader_lock(&wh->control);
        if(wh->control.reader_check == false)
            {
            end = true;
            Logger_elem* logger_error = NULL;

            logger_error = create_message_to_send(ERROR, READER_STOP);

            logger_lock(&wh->logger);
            if (logger_is_full(&wh->logger))
                logger_wait_for_consumer(&wh->logger);
            logger_put(&wh->logger, logger_error);
            logger_call_consumer(&wh->logger);
            logger_unlock(&wh->logger);

            
            end = true;
            }  
            
        else
            wh->control.reader_check = false;
        control_reader_unlock(&wh->control);
        

        control_analyzer_lock(&wh->control);
        if(wh->control.analyzer_check == false)
            {
            
            Logger_elem* logger_error = NULL;

            logger_error = create_message_to_send(ERROR, ANALYZER_STOP);

            logger_lock(&wh->logger);
            if (logger_is_full(&wh->logger))
                logger_wait_for_consumer(&wh->logger);
            logger_put(&wh->logger, logger_error);
            logger_call_consumer(&wh->logger);
            logger_unlock(&wh->logger);

            
            end = true;
            } 
            
        else
            wh->control.analyzer_check = false;
        control_analyzer_unlock(&wh->control);


        control_printer_lock(&wh->control);
        if(wh->control.printer_check == false)
            {
            
            Logger_elem* logger_error = NULL;

            logger_error = create_message_to_send(ERROR, PRINTER_STOP);

            logger_lock(&wh->logger);
            if (logger_is_full(&wh->logger))
                logger_wait_for_consumer(&wh->logger);
            logger_put(&wh->logger, logger_error);
            logger_call_consumer(&wh->logger);
            logger_unlock(&wh->logger);

            
            end = true;
            } 
            
        else
            wh->control.printer_check = false;
        control_printer_unlock(&wh->control);

    }
    

    wh->control.end = true;
    usleep(0.2);
    pthread_cond_signal(&wh->warehouse_ra.can_produce);
    usleep(0.2);
    pthread_cond_signal(&wh->warehouse_ra.can_consume);
    usleep(0.2);
    pthread_cond_signal(&wh->warehouse_ap.can_produce);
    usleep(0.2);
    pthread_cond_signal(&wh->warehouse_ap.can_consume);
    usleep(0.2);
    pthread_cond_signal(&wh->logger.can_produce);
    usleep(0.2);
    pthread_cond_signal(&wh->logger.can_consume);
    
    
    return(NULL);
}

void* thread_logger(void *arg)
{

    Warehouse* wh = *(Warehouse**)arg;
    FILE *log_file = fopen("log.txt", "w");
    while(!wh->control.end)
    {
        
        logger_lock(&wh->logger);

        if(logger_is_empty(&wh->logger))
        {
            logger_wait_for_producer(&wh->logger);
        }

        Logger_elem* product = NULL;
        if(!wh->control.end)
            product = logger_get(&wh->logger);

        logger_call_producer(&wh->logger);
        logger_unlock(&wh->logger);
        if(!wh->control.end)
            fprintf(log_file, "%s %s\n", message_type_to_str(product->type), message_contex_to_str(product->contex));
        free(product);
        
    }

    usleep(0.5);
    
    while(wh->logger.size != 0)
    {
        logger_lock(&wh->logger);
        if(logger_is_empty(&wh->logger))
        {
            logger_wait_for_producer(&wh->logger);
        }

        Logger_elem* product = NULL;
       
            product = logger_get(&wh->logger);

        logger_call_producer(&wh->logger);
        logger_unlock(&wh->logger);
        
            fprintf(log_file, "%s %s\n", message_type_to_str(product->type), message_contex_to_str(product->contex));
        free(product);
    }
    



    fclose(log_file);
   

    return NULL;
}