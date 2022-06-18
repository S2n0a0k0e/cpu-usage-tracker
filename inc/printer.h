#ifndef PRINTER_H
#define PRINTER_H

#include <stdio.h>
#include <stddef.h>
#include "analyzer.h"


void cpu_usage_print(Usage_package* cpu);
Usage_package* sum_usage(Usage_package* cpu_sum, Usage_package* cpu);
double* sum_usage_table(double* cpu_sum, Usage_package* cpu);
void average_usage_print(double* usage,  size_t counter);

#endif
