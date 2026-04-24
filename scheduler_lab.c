#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <limits.h>

#define MIN_PROCESSES 5
#define MAX_PROCESSES 15
#define MIN_BURST 1
#define MAX_BURST 10
#define MIN_ARRIVAL 0
#define MAX_ARRIVAL 30
#define RR_QUANTUM 2

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

typedef struct {
    int data[256];
    int front;
    int rear;
} Queue;

/* =========================
   Queue helpers
   ========================= */
void queue_init(Queue *q) {
    q->front = 0;
    q->rear = 0;
}

int queue_empty(Queue *q) {
    return q->front == q->rear;
}

void enqueue(Queue *q, int value) {
    q->data[q->rear++] = value;
}

int dequeue(Queue *q) {
    return q->data[q->front++];
}

/* =========================
   Thread function
   ========================= */
void *process_thread_func(void *arg) {
    Process *p = (Process *)arg;
    /* El thread existe para representar el proceso en Linux.
       La planificación real se hace con reloj virtual en main. */
    (void)p;
    return NULL;
}

/* =========================
   Utility functions
   ========================= */
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

void sort_by_arrival_then_pid(Process *p, int n) {
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (p[j].arrival_time > p[j + 1].arrival_time ||
               (p[j].arrival_time == p[j + 1].arrival_time && p[j].pid > p[j + 1].pid)) {
                Process tmp = p[j];
                p[j] = p[j + 1];
                p[j + 1] = tmp;
            }
        }
    }
}

void format_virtual_time(time_t base_time, int virtual_seconds, char *buffer, size_t size) {
    time_t t = base_time + virtual_seconds;
    struct tm tm_info;
    localtime_r(&t, &tm_info);
    strftime(buffer, size, "[%a %b %d %H:%M:%S %Y]", &tm_info);
}

void log_arrivals(Process *p, int n, int current_time, time_t base_time) {
    char timebuf[64];

    for (int i = 0; i < n; i++) {
        if (!p[i].arrival_logged && p[i].arrival_time <= current_time) {
            format_virtual_time(base_time, p[i].arrival_time, timebuf, sizeof(timebuf));
            printf("%s Process %d (Burst %d): Arrived\n",
                   timebuf, p[i].pid, p[i].burst_time);
            p[i].arrival_logged = 1;
        }
    }
}

void log_start(Process *p, int current_time, time_t base_time) {
    char timebuf[64];
    format_virtual_time(base_time, current_time, timebuf, sizeof(timebuf));

    double waited = (double)(current_time - p->arrival_time);
    printf("%s Process %d (Burst %d): Started (waited %.2f seconds)\n",
           timebuf, p->pid, p->burst_time, waited);
}

void log_complete(Process *p, int current_time, time_t base_time) {
    char timebuf[64];
    format_virtual_time(base_time, current_time, timebuf, sizeof(timebuf));

    printf("%s Process %d (Burst %d): Completed\n",
           timebuf, p->pid, p->burst_time);
}

void compute_metrics(Process *p, int n) {
    for (int i = 0; i < n; i++) {
        p[i].turnaround_time = (double)(p[i].completion_time - p[i].arrival_time);
        p[i].waiting_time = p[i].turnaround_time - p[i].burst_time;
    }
}

void print_metrics(Process *p, int n) {
    double total_wait = 0.0;
    double total_turnaround = 0.0;

    printf("Waiting Times: [");
    for (int i = 0; i < n; i++) {
        printf("%.2f", p[i].waiting_time);
        total_wait += p[i].waiting_time;
        if (i != n - 1) printf(", ");
    }
    printf("]\n");

    printf("Avg Waiting Time: %.2f seconds\n", total_wait / n);

    printf("Turnaround Times: [");
    for (int i = 0; i < n; i++) {
        printf("%.2f", p[i].turnaround_time);
        total_turnaround += p[i].turnaround_time;
        if (i != n - 1) printf(", ");
    }
    printf("]\n");

    printf("Avg Turnaround Time: %.2f seconds\n", total_turnaround / n);
}

void print_initial_processes(Process *p, int n) {
    printf("Generated Processes:\n");
    printf("PID\tArrival\tBurst\n");
    for (int i = 0; i < n; i++) {
        printf("%d\t%d\t%d\n", p[i].pid, p[i].arrival_time, p[i].burst_time);
    }
    printf("\n");
}

/* =========================
   FIFO / FCFS
   ========================= */
void simulate_fifo(Process *original, int n, time_t base_time) {
    Process p[64];
    copy_processes(p, original, n);
    sort_by_arrival_then_pid(p, n);

    printf("\n--- FIFO Scheduling ---\n");

    int current_time = 0;

    for (int i = 0; i < n; i++) {
        if (current_time < p[i].arrival_time) {
            log_arrivals(p, n, p[i].arrival_time, base_time);
            current_time = p[i].arrival_time;
        } else {
            log_arrivals(p, n, current_time, base_time);
        }

        if (!p[i].started) {
            p[i].started = 1;
            p[i].start_time = current_time;
            log_start(&p[i], current_time, base_time);
        }

        current_time += p[i].burst_time;
        p[i].remaining_time = 0;
        p[i].completed = 1;
        p[i].completion_time = current_time;

        log_arrivals(p, n, current_time, base_time);
        log_complete(&p[i], current_time, base_time);
    }

    compute_metrics(p, n);
    print_metrics(p, n);
}

/* =========================
   Round Robin
   ========================= */
void simulate_rr(Process *original, int n, time_t base_time, int quantum) {
    Process p[64];
    copy_processes(p, original, n);
    sort_by_arrival_then_pid(p, n);

    printf("\n--- Round Robin Scheduling (Quantum = %d) ---\n", quantum);

    Queue q;
    queue_init(&q);

    int current_time = 0;
    int completed_count = 0;
    int next_arrival_index = 0;

    while (completed_count < n) {
        while (next_arrival_index < n && p[next_arrival_index].arrival_time <= current_time) {
            if (!p[next_arrival_index].arrival_logged) {
                char timebuf[64];
                format_virtual_time(base_time, p[next_arrival_index].arrival_time, timebuf, sizeof(timebuf));
                printf("%s Process %d (Burst %d): Arrived\n",
                       timebuf, p[next_arrival_index].pid, p[next_arrival_index].burst_time);
                p[next_arrival_index].arrival_logged = 1;
            }
            enqueue(&q, next_arrival_index);
            next_arrival_index++;
        }

        if (queue_empty(&q)) {
            if (next_arrival_index < n) {
                current_time = p[next_arrival_index].arrival_time;
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
            log_start(&p[idx], current_time, base_time);
        }

        int slice = (p[idx].remaining_time < quantum) ? p[idx].remaining_time : quantum;
        current_time += slice;
        p[idx].remaining_time -= slice;

        while (next_arrival_index < n && p[next_arrival_index].arrival_time <= current_time) {
            if (!p[next_arrival_index].arrival_logged) {
                char timebuf[64];
                format_virtual_time(base_time, p[next_arrival_index].arrival_time, timebuf, sizeof(timebuf));
                printf("%s Process %d (Burst %d): Arrived\n",
                       timebuf, p[next_arrival_index].pid, p[next_arrival_index].burst_time);
                p[next_arrival_index].arrival_logged = 1;
            }
            enqueue(&q, next_arrival_index);
            next_arrival_index++;
        }

        if (p[idx].remaining_time == 0) {
            p[idx].completed = 1;
            p[idx].completion_time = current_time;
            completed_count++;
            log_complete(&p[idx], current_time, base_time);
        } else {
            enqueue(&q, idx);
        }
    }

    compute_metrics(p, n);
    print_metrics(p, n);
}

/* =========================
   SJF Non-preemptive
   ========================= */
int select_sjf(Process *p, int n, int current_time) {
    int best = -1;
    int best_burst = INT_MAX;

    for (int i = 0; i < n; i++) {
        if (!p[i].completed && p[i].arrival_time <= current_time) {
            if (p[i].burst_time < best_burst) {
                best_burst = p[i].burst_time;
                best = i;
            } else if (p[i].burst_time == best_burst) {
                if (best == -1 || p[i].arrival_time < p[best].arrival_time ||
                   (p[i].arrival_time == p[best].arrival_time && p[i].pid < p[best].pid)) {
                    best = i;
                }
            }
        }
    }
    return best;
}

void simulate_sjf(Process *original, int n, time_t base_time) {
    Process p[64];
    copy_processes(p, original, n);

    printf("\n--- SJF Scheduling ---\n");

    int current_time = 0;
    int completed_count = 0;

    while (completed_count < n) {
        log_arrivals(p, n, current_time, base_time);

        int idx = select_sjf(p, n, current_time);

        if (idx == -1) {
            int next_arrival = INT_MAX;
            for (int i = 0; i < n; i++) {
                if (!p[i].completed && p[i].arrival_time < next_arrival) {
                    next_arrival = p[i].arrival_time;
                }
            }
            current_time = next_arrival;
            log_arrivals(p, n, current_time, base_time);
            idx = select_sjf(p, n, current_time);
        }

        if (!p[idx].started) {
            p[idx].started = 1;
            p[idx].start_time = current_time;
            log_start(&p[idx], current_time, base_time);
        }

        current_time += p[idx].burst_time;
        p[idx].remaining_time = 0;
        p[idx].completed = 1;
        p[idx].completion_time = current_time;
        completed_count++;

        log_arrivals(p, n, current_time, base_time);
        log_complete(&p[idx], current_time, base_time);
    }

    compute_metrics(p, n);
    print_metrics(p, n);
}

/* =========================
   SRTF Preemptive
   ========================= */
int select_srtf(Process *p, int n, int current_time) {
    int best = -1;
    int best_remaining = INT_MAX;

    for (int i = 0; i < n; i++) {
        if (!p[i].completed && p[i].arrival_time <= current_time && p[i].remaining_time > 0) {
            if (p[i].remaining_time < best_remaining) {
                best_remaining = p[i].remaining_time;
                best = i;
            } else if (p[i].remaining_time == best_remaining) {
                if (best == -1 || p[i].arrival_time < p[best].arrival_time ||
                   (p[i].arrival_time == p[best].arrival_time && p[i].pid < p[best].pid)) {
                    best = i;
                }
            }
        }
    }
    return best;
}

void simulate_srtf(Process *original, int n, time_t base_time) {
    Process p[64];
    copy_processes(p, original, n);

    printf("\n--- SRTF Scheduling ---\n");

    int current_time = 0;
    int completed_count = 0;
    int last_running = -1;

    while (completed_count < n) {
        log_arrivals(p, n, current_time, base_time);

        int idx = select_srtf(p, n, current_time);

        if (idx == -1) {
            current_time++;
            continue;
        }

        if (!p[idx].started) {
            p[idx].started = 1;
            p[idx].start_time = current_time;
            log_start(&p[idx], current_time, base_time);
        } else if (last_running != idx) {
            /* Solo mostramos Started la primera vez.
               Si quieres, aquí podrías imprimir "Resumed". */
        }

        last_running = idx;

        p[idx].remaining_time--;
        current_time++;

        log_arrivals(p, n, current_time, base_time);

        if (p[idx].remaining_time == 0) {
            p[idx].completed = 1;
            p[idx].completion_time = current_time;
            completed_count++;
            log_complete(&p[idx], current_time, base_time);
            last_running = -1;
        }
    }

    compute_metrics(p, n);
    print_metrics(p, n);
}

/* =========================
   Main
   ========================= */
int main(void) {
    srand((unsigned int)time(NULL));

    int n = (rand() % (MAX_PROCESSES - MIN_PROCESSES + 1)) + MIN_PROCESSES;
    Process processes[64];

    for (int i = 0; i < n; i++) {
        processes[i].pid = i;
        processes[i].arrival_time = (rand() % (MAX_ARRIVAL - MIN_ARRIVAL + 1)) + MIN_ARRIVAL;
        processes[i].burst_time = (rand() % (MAX_BURST - MIN_BURST + 1)) + MIN_BURST;
        processes[i].remaining_time = processes[i].burst_time;

        processes[i].started = 0;
        processes[i].completed = 0;
        processes[i].arrival_logged = 0;
        processes[i].start_time = -1;
        processes[i].completion_time = -1;
        processes[i].waiting_time = 0.0;
        processes[i].turnaround_time = 0.0;
    }

    /* Crear threads POSIX para representar procesos */
    for (int i = 0; i < n; i++) {
        if (pthread_create(&processes[i].thread, NULL, process_thread_func, &processes[i]) != 0) {
            perror("pthread_create");
            return 1;
        }
    }

    for (int i = 0; i < n; i++) {
        pthread_join(processes[i].thread, NULL);
    }

    print_initial_processes(processes, n);

    time_t base_time = time(NULL);

    simulate_fifo(processes, n, base_time);
    simulate_rr(processes, n, base_time, RR_QUANTUM);
    simulate_sjf(processes, n, base_time);
    simulate_srtf(processes, n, base_time);

    return 0;
}