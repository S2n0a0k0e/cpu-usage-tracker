#include <stdlib.h>
#include <stdio.h>
#include "analyzer.h"
#include "reader.h"
#include <string.h>

Usage_package* cpu_usage_calculate(Package* cpu)
{
    Usage_package* package = malloc(sizeof(*package) + sizeof(*package->buffer)*(cpu_count()+1));
    size_t index = cpu_count();
    package->size = cpu_count() + 1;
    for(size_t i = 0; i <= index; i++)
    {
        Cpu_usage* usage = malloc(sizeof(*usage));
    
        size_t PrevIdle = cpu->buffer[i]->idle + cpu->buffer[i]->iowait;
        size_t Idle = cpu->buffer[i+index+1]->idle + cpu->buffer[i+index+1]->iowait;

        size_t PrevNonIdle = cpu->buffer[i]->user + cpu->buffer[i]->nice + cpu->buffer[i]->system + cpu->buffer[i]->irq + cpu->buffer[i]->softirq + cpu->buffer[i]->steal;
        size_t NonIdle = cpu->buffer[i+index+1]->user + cpu->buffer[i+index+1]->nice + cpu->buffer[i+index+1]->system + cpu->buffer[i+index+1]->irq + cpu->buffer[i+index+1]->softirq + cpu->buffer[i+index]->steal;

        size_t PrevTotal = PrevIdle + PrevNonIdle;
        size_t Total = Idle + NonIdle;

        size_t totald = Total - PrevTotal;
        size_t idled = Idle - PrevIdle;

        double CPU_Percentage = ((double)(totald - idled)/(double)totald)*100;

        usage->percentage = CPU_Percentage;

        memcpy(usage->name,cpu->buffer[i]->name,sizeof(cpu->buffer[i]->name));
        package->buffer[i] = usage;
        // printf("%f\n", usage->percentage);
        // printf("%s\n", usage->name);
    }
   
    return package;
}
