#include <stdlib.h>
#include <stdio.h>
#include "analyzer.h"
#include <string.h>

Usage_package* cpu_usage_calculate(register Package* cpu)
{
    if(cpu == NULL)
        return NULL;

    Usage_package* package = malloc(sizeof(*package) + sizeof(*package->buffer)*(cpu_count()+1));

    if(package == NULL)
        return NULL;

    const size_t index = cpu_count();
    package->size = cpu_count() + 1;

    for(size_t i = 0; i <= index; i++)
    {
        Cpu_usage* usage = malloc(sizeof(*usage));
        
        if(usage == NULL)
            return NULL;

        const size_t PrevIdle = cpu->buffer[i]->idle + cpu->buffer[i]->iowait;
        const size_t Idle = cpu->buffer[i+index+1]->idle + cpu->buffer[i+index+1]->iowait;

        const size_t PrevNonIdle = cpu->buffer[i]->user + cpu->buffer[i]->nice + cpu->buffer[i]->system + cpu->buffer[i]->irq + cpu->buffer[i]->softirq + cpu->buffer[i]->steal;
        const size_t NonIdle = cpu->buffer[i+index+1]->user + cpu->buffer[i+index+1]->nice + cpu->buffer[i+index+1]->system + cpu->buffer[i+index+1]->irq + cpu->buffer[i+index+1]->softirq + cpu->buffer[i+index]->steal;

        const size_t PrevTotal = PrevIdle + PrevNonIdle;
        const size_t Total = Idle + NonIdle;

        const size_t totald = Total - PrevTotal;
        const size_t idled = Idle - PrevIdle;

        // totald is a divisor so it shouldn't be equal 0
        if(totald == 0)
            return NULL;

        const double CPU_Percentage = ((double)(totald - idled)/(double)totald)*100;
       
        usage->percentage = CPU_Percentage;

        memcpy(usage->name,cpu->buffer[i]->name,sizeof(cpu->buffer[i]->name));
        package->buffer[i] = usage;
    }
   
    return package;
}
