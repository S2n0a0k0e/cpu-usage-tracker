#ifndef ANALYZER_H
#define ANALYZER_H

#include <stdio.h>
#include <stddef.h>
#include "reader.h"

typedef struct Cpu_usage
{
    char name[8];
    double percentage;
} Cpu_usage;

typedef struct Usage_package
{
    size_t size;
    Cpu_usage* buffer[];
} Usage_package;

Usage_package* cpu_usage_calculate(Package* cpu);

#endif
