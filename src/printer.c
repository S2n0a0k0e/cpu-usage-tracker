#include <stdlib.h>
#include <stdio.h>
#include "printer.h"
#include <string.h>

Usage_package* sum_usage(Usage_package* cpu_sum, const Usage_package* cpu)
{
    if(cpu_sum == NULL || cpu == NULL)
        return NULL;

    for(size_t i = 0; i <= cpu_count(); i++)
    {
        cpu_sum->buffer[i]->percentage += cpu->buffer[i]->percentage;
        memcpy(cpu_sum->buffer[i]->name,cpu->buffer[i]->name,sizeof(cpu->buffer[i]->name));
    }

    return cpu_sum;
}


double* sum_usage_table(double* cpu_sum,const Usage_package* cpu)
{
    if(cpu == NULL)
        return NULL;

    for(size_t i = 0; i <= cpu_count(); i++)
        cpu_sum[i] += cpu->buffer[i]->percentage;

    return cpu_sum;
}

void average_usage_print(const double* usage, size_t counter,const Usage_package* my_package)
{   
    printf("-----------------------\n");
    for(size_t i = 0; i < cpu_count() + 1; i++)
    {
        if(counter == 0)
            return;
        printf("%s %f\n",my_package->buffer[i]->name, usage[i]/(double)counter);
    }
    printf("-----------------------\n");
}
