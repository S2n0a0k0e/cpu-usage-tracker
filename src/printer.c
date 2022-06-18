#include <stdlib.h>
#include <stdio.h>
#include "analyzer.h"
#include "printer.h"
#include <string.h>

void cpu_usage_print(Usage_package* cpu)
{
    for(size_t i = 0; i <= cpu_count(); i++)
    {
        printf("%f\n", cpu->buffer[i]->percentage);
        printf("%s\n", cpu->buffer[i]->name);
    }
    
}

Usage_package* sum_usage(Usage_package* cpu_sum, Usage_package* cpu)
{
    for(size_t i = 0; i <= cpu_count(); i++)
    {
        cpu_sum->buffer[i]->percentage += cpu->buffer[i]->percentage;
        memcpy(cpu_sum->buffer[i]->name,cpu->buffer[i]->name,sizeof(cpu->buffer[i]->name));
    }
    return cpu_sum;
}


double* sum_usage_table(double* cpu_sum, Usage_package* cpu)
{
    for(size_t i = 0; i <= cpu_count(); i++)
    {
        cpu_sum[i] += cpu->buffer[i]->percentage;
    }
    return cpu_sum;
}

void average_usage_print(double* usage, size_t counter)
{   
    printf("-----------------------\n");
    for(size_t i = 0; i < cpu_count() + 1; i++)
    {
        printf("%f\n", usage[i]/(double)counter);
    }
    printf("-----------------------\n");
}
