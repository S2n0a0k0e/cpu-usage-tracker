#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include <stdbool.h>

#define LOGGER_BUFFER_SIZE 500

enum Message_type
{
    NONE,
    INFO, 
    ERROR
};

enum Message_contex
{
    READ_FILE,
    SEND_DATA_TO_ANALYZER,
    ANALYZE,
    SEND_DATA_TO_PRINTER,
    PRINTER_GET_DATA,
    PRINT,
    READER_STOP,
    ANALYZER_STOP,
    PRINTER_STOP
};


typedef struct Logger_elem
{
    enum Message_type type;
    enum Message_contex contex;
} Logger_elem;

typedef struct Logger
{
    Logger_elem* buffer[LOGGER_BUFFER_SIZE];
    size_t size;
    size_t head;
    size_t tail;
    pthread_cond_t can_produce;
    pthread_cond_t can_consume;
    pthread_mutex_t mutex;
    
} Logger;




Logger* logger_create(void);
void logger_destroy(Logger* logger);
char* message_type_to_str(enum Message_type type);
char* message_contex_to_str(enum Message_contex contex);
void logger_lock(Logger* logger);
void logger_unlock(Logger* logger);
void logger_call_producer(Logger* logger);
void logger_call_consumer(Logger* logger);
void logger_wait_for_producer(Logger* logger);
void logger_wait_for_consumer(Logger* logger);
void logger_put(Logger* logger, Logger_elem* product);
bool logger_is_full(const Logger* logger);
bool logger_is_empty(const Logger* logger);
Logger_elem* create_message_to_send(enum Message_type type, enum Message_contex contex);
Logger_elem* logger_get(Logger* logger);


#endif
