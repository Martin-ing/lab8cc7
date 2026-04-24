#include <stdio.h>
#include <limits.h>
#include <time.h>

#include "algorithms.h"
#include "queue.h"
#include "utils.h"

/* ================= FIFO ================= */

void simulate_fifo(Process *original, int n, time_t base, int *gantt, int *gantt_len) {
    Process p[64];
    copy_processes(p, original, n);
    sort_by_arrival(p, n);

    printf("\n--- FIFO Scheduling ---\n");

    int current_time = 0;

    for (int i = 0; i < n; i++) {
        if (current_time < p[i].arrival_time) {
            current_time = p[i].arrival_time;
        }

        log_arrivals(p, n, current_time, base);

        p[i].started = 1;
        p[i].start_time = current_time;
        log_start(&p[i], base, current_time);

        for (int j = 0; j < p[i].burst_time; j++) {
            gantt[*gantt_len] = p[i].pid;
            (*gantt_len)++;
        }

        current_time += p[i].burst_time;

        p[i].completed = 1;
        p[i].completion_time = current_time;

        log_arrivals(p, n, current_time, base);
        log_complete(&p[i], base, current_time);
    }

    compute_metrics(p, n);
    print_metrics(p, n);
}

/* ================= Round Robin ================= */

void simulate_rr(Process *original, int n, time_t base, int quantum, int *gantt, int *gantt_len) {
    Process p[64];
    copy_processes(p, original, n);
    sort_by_arrival(p, n);

    printf("\n--- Round Robin Scheduling Quantum = %d ---\n", quantum);

    Queue q;
    queue_init(&q);

    int current_time = 0;
    int completed_count = 0;
    int next = 0;

    while (completed_count < n) {
        while (next < n && p[next].arrival_time <= current_time) {
            if (!p[next].arrival_logged) {
                log_arrival(&p[next], base, p[next].arrival_time);
                p[next].arrival_logged = 1;
            }

            enqueue(&q, next);
            next++;
        }

        if (queue_empty(&q)) {
            if (next < n) {
                current_time = p[next].arrival_time;
                continue;
            }
        }

        if (queue_empty(&q)) {
            break;
        }

        int idx = dequeue(&q);

        if (!p[idx].started) {
            p[idx].started = 1;
            p[idx].start_time = current_time;
            log_start(&p[idx], base, current_time);
        }

        int slice;

        if (p[idx].remaining_time < quantum) {
            slice = p[idx].remaining_time;
        } else {
            slice = quantum;
        }

        for (int j = 0; j < slice; j++) {
            gantt[*gantt_len] = p[idx].pid;
            (*gantt_len)++;
        }

        p[idx].remaining_time -= slice;
        current_time += slice;

        while (next < n && p[next].arrival_time <= current_time) {
            if (!p[next].arrival_logged) {
                log_arrival(&p[next], base, p[next].arrival_time);
                p[next].arrival_logged = 1;
            }

            enqueue(&q, next);
            next++;
        }

        if (p[idx].remaining_time == 0) {
            p[idx].completed = 1;
            p[idx].completion_time = current_time;
            completed_count++;

            log_complete(&p[idx], base, current_time);
        } else {
            enqueue(&q, idx);
        }
    }

    compute_metrics(p, n);
    print_metrics(p, n);
}

/* ================= SJF Non-preemptive ================= */

int select_sjf(Process *p, int n, int current_time) {
    int best = -1;
    int best_burst = INT_MAX;

    for (int i = 0; i < n; i++) {
        if (!p[i].completed && p[i].arrival_time <= current_time) {
            if (p[i].burst_time < best_burst) {
                best_burst = p[i].burst_time;
                best = i;
            } else if (p[i].burst_time == best_burst) {
                if (best == -1 ||
                    p[i].arrival_time < p[best].arrival_time ||
                   (p[i].arrival_time == p[best].arrival_time &&
                    p[i].pid < p[best].pid)) {
                    best = i;
                }
            }
        }
    }

    return best;
}

void simulate_sjf(Process *original, int n, time_t base, int *gantt, int *gantt_len) {
    Process p[64];
    copy_processes(p, original, n);

    printf("\n--- SJF Scheduling ---\n");

    int current_time = 0;
    int completed_count = 0;

    while (completed_count < n) {
        log_arrivals(p, n, current_time, base);

        int idx = select_sjf(p, n, current_time);

        if (idx == -1) {
            int next_arrival = INT_MAX;

            for (int i = 0; i < n; i++) {
                if (!p[i].completed && p[i].arrival_time < next_arrival) {
                    next_arrival = p[i].arrival_time;
                }
            }

            current_time = next_arrival;
            log_arrivals(p, n, current_time, base);

            idx = select_sjf(p, n, current_time);
        }

        p[idx].started = 1;
        p[idx].start_time = current_time;
        log_start(&p[idx], base, current_time);

        for (int j = 0; j < p[idx].burst_time; j++) {
            gantt[*gantt_len] = p[idx].pid;
            (*gantt_len)++;
        }

        current_time += p[idx].burst_time;

        p[idx].remaining_time = 0;
        p[idx].completed = 1;
        p[idx].completion_time = current_time;
        completed_count++;

        log_arrivals(p, n, current_time, base);
        log_complete(&p[idx], base, current_time);
    }

    compute_metrics(p, n);
    print_metrics(p, n);
}

/* ================= SRTF Preemptive ================= */

int select_srtf(Process *p, int n, int current_time) {
    int best = -1;
    int best_remaining = INT_MAX;

    for (int i = 0; i < n; i++) {
        if (!p[i].completed &&
            p[i].arrival_time <= current_time &&
            p[i].remaining_time > 0) {

            if (p[i].remaining_time < best_remaining) {
                best_remaining = p[i].remaining_time;
                best = i;
            } else if (p[i].remaining_time == best_remaining) {
                if (best == -1 ||
                    p[i].arrival_time < p[best].arrival_time ||
                   (p[i].arrival_time == p[best].arrival_time &&
                    p[i].pid < p[best].pid)) {
                    best = i;
                }
            }
        }
    }

    return best;
}

void simulate_srtf(Process *original, int n, time_t base, int *gantt, int *gantt_len) {
    Process p[64];
    copy_processes(p, original, n);

    printf("\n--- SRTF Scheduling ---\n");

    int current_time = 0;
    int completed_count = 0;

    while (completed_count < n) {
        log_arrivals(p, n, current_time, base);

        int idx = select_srtf(p, n, current_time);

        if (idx == -1) {
            current_time++;
            continue;
        }

        if (!p[idx].started) {
            p[idx].started = 1;
            p[idx].start_time = current_time;
            log_start(&p[idx], base, current_time);
        }

        gantt[*gantt_len] = p[idx].pid;
        (*gantt_len)++;

        p[idx].remaining_time--;
        current_time++;

        log_arrivals(p, n, current_time, base);

        if (p[idx].remaining_time == 0) {
            p[idx].completed = 1;
            p[idx].completion_time = current_time;
            completed_count++;

            log_complete(&p[idx], base, current_time);
        }
    }

    compute_metrics(p, n);
    print_metrics(p, n);
}