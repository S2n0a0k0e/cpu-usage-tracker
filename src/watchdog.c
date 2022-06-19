#include "watchdog.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>


Control* control_create(void)
{
    Control* control = malloc(sizeof(*control));
    if(control == NULL)
        return NULL;
    
    *control = (Control){       .reader_stop = PTHREAD_MUTEX_INITIALIZER,
                                .analyzer_stop = PTHREAD_MUTEX_INITIALIZER,
                                .printer_stop = PTHREAD_MUTEX_INITIALIZER,
                                .end_mutex = PTHREAD_MUTEX_INITIALIZER,
                                .reader_check = true,
                                .analyzer_check = true,
                                .printer_check = true,
                                .end = false
                                };
    return control;
}


void control_destroy(Control* control)
{
    pthread_mutex_destroy(&control ->reader_stop);
    pthread_mutex_destroy(&control ->analyzer_stop);
    pthread_mutex_destroy(&control ->printer_stop);

    free(control);
}

void control_reader_lock(Control* control)
{
    pthread_mutex_lock(&control->reader_stop);
}

void control_reader_unlock(Control* control)
{
    pthread_mutex_unlock(&control->reader_stop);
}

void control_analyzer_lock(Control* control)
{
    pthread_mutex_lock(&control->analyzer_stop);
}

void control_analyzer_unlock(Control* control)
{
    pthread_mutex_unlock(&control->analyzer_stop);
}

void control_printer_lock(Control* control)
{
    pthread_mutex_lock(&control->printer_stop);
}

void control_printer_unlock(Control* control)
{
    pthread_mutex_unlock(&control->printer_stop);
}

void control_end_lock(Control* control)
{
    pthread_mutex_lock(&control->end_mutex);
}

void control_end_unlock(Control* control)
{
    pthread_mutex_unlock(&control->end_mutex);
}