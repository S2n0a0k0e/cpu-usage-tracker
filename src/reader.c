#define _XOPEN_SOURCE 600
#include <stdlib.h>
#include <stdio.h>
#include "reader.h"
#include <string.h>
#include <unistd.h>
#include <time.h>




Cpu_info* cpu_read_info(register FILE  *f)
{
    if(f == NULL)
        return NULL;

    Cpu_info* const cpu = malloc(sizeof(*cpu));

    if(cpu == NULL)
        return NULL;

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

    // Open file to read data
    f = fopen("/proc/stat", "r");
    if(f == NULL)
        return NULL;

    Package* const package = malloc(sizeof(*package) + sizeof(*package->buffer)*(2*(cpu_count()+1)));

    if(package == NULL)
        return NULL;
    
    package->size = 2*(cpu_count()+1);

    // Read information about cpus
    for(size_t i = 0; i < cpu_count()+1; i++)
    {
        Cpu_info* const product = cpu_read_info(f);
        if(product == NULL)
        {
            for(size_t j = 0; j < package->size; j++)
            {
                if(package->buffer[j] != NULL)
                    free(package->buffer[j]);
            }
            return NULL;
        }
        package->buffer[i] = product;
    }

    fclose(f);
    usleep(100000);

    f = fopen("/proc/stat", "r");
    if(f == NULL)
    {
        free(package);
        return NULL;
    }
        

    // Read information about cpus after 0.1s
    for(size_t i = 0; i < cpu_count() + 1; i++)
    {
        Cpu_info* const product = cpu_read_info(f);
        if(product == NULL)
        {
            for(size_t j = 0; j < package->size; j++)
            {
                if(package->buffer[j] != NULL)
                    free(package->buffer[j]);
            }
            return NULL;
        }
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





