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

int total_burst_time(Process *p, int n) {
    int total = 0;

    for (int i = 0; i < n; i++) {
        total += p[i].burst_time;
    }

    return total;
}

void print_gantt_chart(Process *p, int n, int *gantt, int gantt_len, const char *title) {
    printf("\n--- Gantt Chart: %s ---\n", title);

    printf("     ");
    for (int t = 0; t < gantt_len; t++) {
        printf("%2d ", t + 1);
    }
    printf("\n");

    for (int i = 0; i < n; i++) {
        printf("p%d:  ", p[i].pid);

        for (int t = 0; t < gantt_len; t++) {
            if (gantt[t] == p[i].pid) {
                printf(" * ");
            } else {
                printf("   ");
            }
        }

        printf("\n");
    }
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

    int total_time = total_burst_time(p, n);
    int *gantt = malloc(sizeof(int) * total_time);
    int gantt_len = 0;

    simulate_fifo(p, n, base, gantt, &gantt_len);
    print_gantt_chart(p, n, gantt, gantt_len, "FIFO");

    gantt_len = 0;
    simulate_rr(p, n, base, 2, gantt, &gantt_len);
    print_gantt_chart(p, n, gantt, gantt_len, "Round Robin");

    gantt_len = 0;
    simulate_sjf(p, n, base, gantt, &gantt_len);
    print_gantt_chart(p, n, gantt, gantt_len, "SJF");

    gantt_len = 0;
    simulate_srtf(p, n, base, gantt, &gantt_len);
    print_gantt_chart(p, n, gantt, gantt_len, "SRTF");

    free(gantt);

    return 0;
}