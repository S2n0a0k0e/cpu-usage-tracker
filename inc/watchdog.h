#ifndef WATCHDOG_H
#define WATCHDOG_H

#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <stdbool.h>


typedef struct Control
{
    pthread_mutex_t reader_stop;
    pthread_mutex_t analyzer_stop;
    pthread_mutex_t printer_stop;
    pthread_mutex_t end_mutex;
    bool reader_check;
    bool analyzer_check;
    bool printer_check;
    bool end;
} Control;

Control* control_create(void);
void control_destroy(Control* control);
void control_table_destroy(Control* control);
void control_reader_lock(Control* control);
void control_reader_unlock(Control* control);
void control_analyzer_lock(Control* control);
void control_analyzer_unlock(Control* control);
void control_printer_lock(Control* control);
void control_printer_unlock(Control* control);
void control_end_lock(Control* control);
void control_end_unlock(Control* control);

#endif

