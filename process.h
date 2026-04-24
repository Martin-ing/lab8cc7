#ifndef PROCESS_H
#define PROCESS_H

#include <pthread.h>

typedef struct {
    int pid;
    int arrival_time;
    int burst_time;
    int remaining_time;

    int started;
    int completed;
    int arrival_logged;

    int start_time;
    int completion_time;

    double waiting_time;
    double turnaround_time;

    pthread_t thread;
} Process;

#endif