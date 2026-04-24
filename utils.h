#ifndef UTILS_H
#define UTILS_H

#include <time.h>
#include "process.h"

void copy_processes(Process *dest, Process *src, int n);

void sort_by_arrival(Process *p, int n);

void format_virtual_time(time_t base_time, int sec, char *buf);

void log_arrival(Process *p, time_t base, int t);
void log_arrivals(Process *p, int n, int current_time, time_t base);
void log_start(Process *p, time_t base, int t);
void log_complete(Process *p, time_t base, int t);

void compute_metrics(Process *p, int n);
void print_metrics(Process *p, int n);

#endif