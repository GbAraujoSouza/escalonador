#ifndef QUEUE_H
#define QUEUE_H

#include "process.h"
#include <stdbool.h>

typedef struct Node {
  Process process;
  struct Node *next;
} Node;

typedef struct {
  Node* head;
  Node* tail;
  int size;
} Queue;

Queue* alocaQueue();
bool enqueue(Queue* queue, Process newProcess);
bool dequeue(Queue* queue, Process* process);
Process* peek(Queue* queue);
bool isEmpty(Queue* queue);
void freeQueue(Queue* queue);
void printQueue(Queue* queue);


#endif
