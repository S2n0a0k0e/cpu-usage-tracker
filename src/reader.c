#include <stdlib.h>
#include <stdio.h>
#include "reader.h"
#include <string.h>
#include <unistd.h>

Cpu_info* cpu_read_info(FILE *f)
{
    Cpu_info* cpu = malloc(sizeof(*cpu));

    if(fscanf(f, "%s", cpu->name)){};
    if(fscanf(f, "%zu", &cpu->user)){};
    if(fscanf(f, "%zu", &cpu->nice)){};
    if(fscanf(f, "%zu", &cpu->system)){};
    if(fscanf(f, "%zu", &cpu->idle)){};
    if(fscanf(f, "%zu", &cpu->iowait)){};
    if(fscanf(f, "%zu", &cpu->irq)){};
    if(fscanf(f, "%zu", &cpu->softirq)){};
    if(fscanf(f, "%zu", &cpu->steal)){};
    if(fscanf(f, "%zu", &cpu->guest)){};
    if(fscanf(f, "%zu", &cpu->guest_nice)){};
    
    return cpu;
}



Package* cpu_info_create_package(void)
{
    FILE *f;
    f = fopen("/proc/stat", "r");
    Package* package = malloc(sizeof(*package) + sizeof(*package->buffer)*(2*(cpu_count()+1)));
    if(package == NULL)
    {
        return package;
    }
    package->size = 2*(cpu_count()+1);

    for(size_t i = 0; i < cpu_count() + 1; i++)
    {
        Cpu_info* product = cpu_read_info(f);
        package->buffer[i] = product;
    }
    fclose(f);
    usleep(100000);
    f = fopen("/proc/stat", "r");
    for(size_t i = 0; i < cpu_count() + 1; i++)
    {
        Cpu_info* product = cpu_read_info(f);
        package->buffer[i + cpu_count() + 1] = product;
    }
    fclose(f);

    return package;
}

size_t cpu_count(void)
{
    size_t number_of_cpus = (size_t)sysconf(_SC_NPROCESSORS_ONLN); 
    return number_of_cpus;
}





