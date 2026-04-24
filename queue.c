#include "queue.h"

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