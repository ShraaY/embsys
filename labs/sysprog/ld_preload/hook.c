#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

// Question 2.1 : write here the buggy function without errors
#define KNOT_TO_KMH 1.852
int iteration = 0;

int knot_to_kmh_str(float not, size_t size, char * format, char * kmh_str)
{
    float kmh = KNOT_TO_KMH * not;
    //snprintf(kmh_str, size, format, kmh);

#ifndef GPS_OK
    iteration++;
    if (iteration == 2)
    {
        puts("ok");
    }
#endif

    return kmh;
}


// Question 12 : write printf with a signal handler
void signals_handler(int signal_number)
{
    printf("Signal catched: SIGINT.\n");
}

int printf (const char *__restrict __format, ...)
{
    struct sigaction action;
    action.sa_handler = signals_handler;
    sigemptyset(& (action.sa_mask));
    action.sa_flags = 0;
    sigaction(SIGINT, & action, NULL);

}
