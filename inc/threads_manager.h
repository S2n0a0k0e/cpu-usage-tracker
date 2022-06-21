#ifndef THREADS_MANAGER_H
#define THREADS_MANAGER_H

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
#include "printer.h"
#include "warehouse_ra.h"
#include "warehouse_ap.h"
#include "watchdog.h"
#include "logger.h"
// #define HIT 6

void* thread_reader(void *arg);
void* thread_analyzer(void *arg);
void* thread_printer(void *arg);
void* thread_watchdog(void *arg);
void* thread_logger(void *arg);

typedef struct Warehouse
{
    Warehouse_RA warehouse_ra;
    Warehouse_AP warehouse_ap;
    Control control;
    Logger logger;
} Warehouse;

#endif
