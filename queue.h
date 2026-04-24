#ifndef QUEUE_H
#define QUEUE_H

typedef struct {
    int data[256];
    int front;
    int rear;
} Queue;

void queue_init(Queue *q);
int queue_empty(Queue *q);
void enqueue(Queue *q, int value);
int dequeue(Queue *q);

#endif