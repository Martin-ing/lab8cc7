#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <time.h>
#include "process.h"

void simulate_fifo(Process *original, int n, time_t base_time);
void simulate_rr(Process *original, int n, time_t base_time, int quantum);
void simulate_sjf(Process *original, int n, time_t base_time);
void simulate_srtf(Process *original, int n, time_t base_time);

#endif