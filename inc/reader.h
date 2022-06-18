#ifndef READER_H
#define READER_H

#include <stddef.h>
#include <stdint.h>

typedef struct Cpu_info
{
    size_t user;
    size_t nice;
    size_t system;
    size_t idle;
    size_t iowait;
    size_t irq;
    size_t softirq;
    size_t steal;
    size_t guest;
    size_t guest_nice;
    char name[8];
} Cpu_info;

typedef struct Package
{
    size_t size;
    Cpu_info* buffer[];
} Package;

size_t cpu_count(void);

Package* cpu_info_create_package(void);

Cpu_info* cpu_read_info(FILE *f);

#endif
