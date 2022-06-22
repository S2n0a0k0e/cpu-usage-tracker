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
#include <signal.h>
#include <stdbool.h>

bool done = false;

void term(int signum)
{
    printf("Signum: %d\n", signum);
    done = true;
    
}

int main (void)
{   
    Warehouse_RA* wh_ra = warehouse_ra_create();
    Warehouse_AP* wh_ap = warehouse_ap_create();
    Control* control = control_create();
    Logger* PCPlogger = logger_create();
    
    Warehouse* wh = malloc(sizeof(*wh));
    *wh =   (Warehouse){.warehouse_ra = *wh_ra,
                        .warehouse_ap = *wh_ap,
                        .control = *control,
                        .logger = *PCPlogger
                        };

    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = term;
    sigaction(SIGTERM, &action, NULL);


    pthread_t reader, analyzer, printer, watchdog, logger;
    pthread_create(&reader, NULL, thread_reader, (void*)&wh);
    pthread_create(&analyzer, NULL, thread_analyzer, (void*)&wh);
    pthread_create(&printer, NULL, thread_printer, (void*)&wh);
    pthread_create(&watchdog, NULL, thread_watchdog, (void*)&wh);
    pthread_create(&logger, NULL, thread_logger, (void*)&wh);

    

    warehouse_ra_destroy(wh_ra);
    warehouse_ap_destroy(wh_ap);
    control_destroy(control);
    logger_destroy(PCPlogger);

    
    pthread_join(reader, NULL);   
    pthread_join(analyzer, NULL);
    pthread_join(printer, NULL);
    pthread_join(logger, NULL);
    pthread_join(watchdog, NULL);
    
    
    
    
    warehouse_ap_products_destroy(&wh->warehouse_ap);
    warehouse_ra_products_destroy(&wh->warehouse_ra);
    

    free(wh);
    
    
    abort();
    

    return 0;
}
