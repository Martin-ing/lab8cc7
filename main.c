#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#include "process.h"
#include "algorithms.h"

#define MIN_P 5
#define MAX_P 15

void *dummy_thread(void *arg) {
    return NULL;
}

int main() {
    srand(time(NULL));

    int n = rand() % (MAX_P - MIN_P + 1) + MIN_P;

    Process p[64];

    for (int i = 0; i < n; i++) {
        p[i].pid = i;
        p[i].arrival_time = rand() % 31;
        p[i].burst_time = rand() % 10 + 1;
        p[i].remaining_time = p[i].burst_time;

        pthread_create(&p[i].thread, NULL, dummy_thread, NULL);
    }

    for (int i = 0; i < n; i++)
        pthread_join(p[i].thread, NULL);

    printf("Generated Processes:\n");
    printf("PID\tArrival\tBurst\n");

    for (int i = 0; i < n; i++)
        printf("%d\t%d\t%d\n", p[i].pid, p[i].arrival_time, p[i].burst_time);

    time_t base = time(NULL);

    simulate_fifo(p, n, base);
    simulate_rr(p, n, base, 2);
    simulate_sjf(p, n, base);
    simulate_srtf(p, n, base);

    return 0;
}