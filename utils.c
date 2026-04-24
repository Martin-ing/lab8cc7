#include <stdio.h>
#include <time.h>

#include "utils.h"

void copy_processes(Process *dest, Process *src, int n) {
    for (int i = 0; i < n; i++) {
        dest[i] = src[i];
        dest[i].remaining_time = src[i].burst_time;
        dest[i].started = 0;
        dest[i].completed = 0;
        dest[i].arrival_logged = 0;
        dest[i].start_time = -1;
        dest[i].completion_time = -1;
        dest[i].waiting_time = 0.0;
        dest[i].turnaround_time = 0.0;
    }
}

void sort_by_arrival(Process *p, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (p[j].arrival_time > p[j + 1].arrival_time ||
               (p[j].arrival_time == p[j + 1].arrival_time &&
                p[j].pid > p[j + 1].pid)) {

                Process temp = p[j];
                p[j] = p[j + 1];
                p[j + 1] = temp;
            }
        }
    }
}

void format_virtual_time(time_t base_time, int sec, char *buf) {
    time_t t = base_time + sec;
    struct tm *tm_info = localtime(&t);
    strftime(buf, 64, "[%a %b %d %H:%M:%S %Y]", tm_info);
}

void log_arrival(Process *p, time_t base, int t) {
    char buf[64];
    format_virtual_time(base, t, buf);

    printf("%s Process %d (Burst %d): Arrived\n",
           buf, p->pid, p->burst_time);
}

void log_arrivals(Process *p, int n, int current_time, time_t base) {
    for (int i = 0; i < n; i++) {
        if (!p[i].arrival_logged &&
             p[i].arrival_time <= current_time) {

            log_arrival(&p[i], base, p[i].arrival_time);
            p[i].arrival_logged = 1;
        }
    }
}

void log_start(Process *p, time_t base, int t) {
    char buf[64];
    format_virtual_time(base, t, buf);

    double waited = (double)(t - p->arrival_time);

    printf("%s Process %d (Burst %d): Started (waited %.2f seconds)\n",
           buf, p->pid, p->burst_time, waited);
}

void log_complete(Process *p, time_t base, int t) {
    char buf[64];
    format_virtual_time(base, t, buf);

    printf("%s Process %d (Burst %d): Completed\n",
           buf, p->pid, p->burst_time);
}

void compute_metrics(Process *p, int n) {
    for (int i = 0; i < n; i++) {
        p[i].turnaround_time =
            p[i].completion_time - p[i].arrival_time;

        p[i].waiting_time =
            p[i].turnaround_time - p[i].burst_time;
    }
}

void print_metrics(Process *p, int n) {
    double total_waiting = 0.0;
    double total_turnaround = 0.0;

    printf("Waiting Times: [");

    for (int i = 0; i < n; i++) {
        printf("%.2f", p[i].waiting_time);
        total_waiting += p[i].waiting_time;

        if (i < n - 1)
            printf(", ");
    }

    printf("]\n");

    printf("Avg Waiting Time: %.2f seconds\n",
           total_waiting / n);

    printf("Turnaround Times: [");

    for (int i = 0; i < n; i++) {
        printf("%.2f", p[i].turnaround_time);
        total_turnaround += p[i].turnaround_time;

        if (i < n - 1)
            printf(", ");
    }

    printf("]\n");

    printf("Avg Turnaround Time: %.2f seconds\n",
           total_turnaround / n);
}