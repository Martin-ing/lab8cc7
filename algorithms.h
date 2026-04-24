#ifndef ALGORITHMS_H
#define ALGORITHMS_H

#include <time.h>
#include "process.h"

void simulate_fifo(Process *original, int n, time_t base, int *gantt, int *gantt_len);
void simulate_rr(Process *original, int n, time_t base, int quantum, int *gantt, int *gantt_len);
void simulate_sjf(Process *original, int n, time_t base, int *gantt, int *gantt_len);
void simulate_srtf(Process *original, int n, time_t base, int *gantt, int *gantt_len);

#endif