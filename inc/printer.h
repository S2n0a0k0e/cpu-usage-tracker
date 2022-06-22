#ifndef PRINTER_H
#define PRINTER_H

#include <stdio.h>
#include <stddef.h>
#include "analyzer.h"

Usage_package* sum_usage(Usage_package* cpu_sum,const Usage_package* cpu);
double* sum_usage_table(double* cpu_sum,const  Usage_package* cpu);
void average_usage_print(const double* usage,  size_t counter,const Usage_package* my_package);

#endif
