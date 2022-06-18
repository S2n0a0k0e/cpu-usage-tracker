#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include "reader.h"
#include "analyzer.h"
#include "warehouse_ra.h"
#include "warehouse_ap.h"
#include "threads_manager.h"
#include "time.h"
#include <sys/time.h>

int main (void)
{
    
    
    struct timeval te;
    gettimeofday(&te, NULL);
    long long miliseconds = te.tv_sec*1000LL + te.tv_usec/1000;
    
   
    Warehouse_RA* wh_ra = warehouse_ra_create();
    Warehouse_AP* wh_ap = warehouse_ap_create();
    Warehouse* wh = malloc(sizeof(*wh));
    *wh =   (Warehouse){.warehouse_ra = *wh_ra,
                        .warehouse_ap = *wh_ap
                        };
    pthread_t reader, analyzer, printer;
    pthread_create(&reader, NULL, thread_reader, (void*)&wh);
    pthread_create(&analyzer, NULL, thread_analyzer, (void*)&wh);
    pthread_create(&printer, NULL, thread_printer, (void*)&wh);

    pthread_join(reader, NULL);   
    pthread_join(analyzer, NULL); 
    pthread_join(printer, NULL);
    
    warehouse_ra_destroy(wh_ra);
    warehouse_ap_destroy(wh_ap);
    
    warehouse_ap_products_destroy(&wh->warehouse_ap);
    warehouse_ra_products_destroy(&wh->warehouse_ra);
    
    free(wh);
    
    gettimeofday(&te, NULL);
    long long miliseconds2 = te.tv_sec*1000LL + te.tv_usec/1000;
    printf("%lld", (miliseconds2-miliseconds));
    return 0;
}
