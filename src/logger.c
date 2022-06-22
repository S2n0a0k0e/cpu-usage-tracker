#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logger.h"






Logger* logger_create(void)
{
    Logger* logger = malloc(sizeof(*logger) + sizeof(*logger->buffer));
    if(logger == NULL)
        return NULL;
    
    *logger = (Logger){         .buffer = {0},
                                .size = 0,
                                .tail = 0,
                                .head = 0,
                                .can_produce = PTHREAD_COND_INITIALIZER,
                                .can_consume = PTHREAD_COND_INITIALIZER,
                                .mutex = PTHREAD_MUTEX_INITIALIZER
                                };
    return logger;
}

void logger_destroy(Logger* logger)
{
    pthread_mutex_destroy(&logger ->mutex);
    pthread_cond_destroy(&logger->can_consume);
    pthread_cond_destroy(&logger->can_produce);

    free(logger);
}

void logger_lock(Logger* logger)
{
    pthread_mutex_lock(&logger->mutex);
}

void logger_unlock(Logger* logger)
{
    pthread_mutex_unlock(&logger->mutex);
}

void logger_call_producer(Logger* logger)
{
    pthread_cond_signal(&logger->can_produce);
}

void logger_call_consumer(Logger* logger)
{
    pthread_cond_signal(&logger->can_consume);
}

void logger_wait_for_producer(Logger* logger)
{
    pthread_cond_wait(&logger->can_consume, &logger->mutex);
}

void logger_wait_for_consumer(Logger* logger)
{
    pthread_cond_wait(&logger->can_produce, &logger->mutex);
}

bool logger_is_full(const Logger* logger)
{
    return logger->size == LOGGER_BUFFER_SIZE;
}

bool logger_is_empty(const Logger* logger)
{
    return logger->size == 0;
}

void logger_put(Logger* logger,Logger_elem* product)
{
    if(product == NULL)
        return;

    if (logger_is_full(logger))
        return;
    
    logger->buffer[logger->head] = product;
    logger->head = (logger->head + 1) % LOGGER_BUFFER_SIZE;
    ++logger->size;
}

Logger_elem* logger_get(Logger* logger)
{
    if(logger_is_empty(logger))
        return NULL;
    
    Logger_elem* product = logger->buffer[logger->tail];
    logger->tail = (logger->tail + 1) % LOGGER_BUFFER_SIZE;
    --logger->size;

    return product;
}

Logger_elem* create_message_to_send(const enum Message_type type,const  enum Message_contex contex)
{
    Logger_elem* logger_element = malloc(sizeof(*logger_element));
    if(logger_element == NULL)
        return NULL;

    logger_element->type = type;
    logger_element->contex = contex;

    return logger_element;
}


char* message_type_to_str(const enum Message_type type)
{
    if(type < INFO || type > ERROR)
        return "None";

    char* message_str[] = {"[None] ","[INFO] ", "[ERROR] "};
    return message_str[type];
}

char* message_contex_to_str(const enum Message_contex contex)
{
    if(contex < READ_FILE || contex > PRINTER_STOP)
        return "NONE";
    char* message_str[] = { "[READER] Read data.",
                            "[READER] Send data.",
                            "[ANALYZER] Analyze data.",
                            "[ANALYZER] Send data.",
                            "[PRINTER] Get average cpu.",
                            "[PRINTER] Print data.",
                            "[WATCHDOG] Reader doesn't response.",
                            "[WATCHDOG] Analyzer doesn't response.",
                            "[WATCHDOG] Printer doesn't response."
                            };
    return message_str[contex];
}
