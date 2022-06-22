#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "threads_manager.h"
#include <signal.h>


bool done = false;

void term(int signum)
{
    printf("Signal: %d\n", signum);
    done = true;  
}

int main (void)
{   
    struct sigaction action;
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = term;
    sigaction(SIGTERM, &action, NULL);
    
    thread_create();
    
    return 0;
}
