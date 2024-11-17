#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

Queue* alocaQueue() {
  Queue* newQueue = (Queue*)malloc(sizeof(Queue));
  newQueue->head = NULL;
  newQueue->tail = NULL;
  newQueue->size = 0;
  return newQueue;
}

bool isEmpty(Queue* queue) {
  return queue->head == NULL;
}

bool enqueue(Queue* queue, Process newProcess) {
  if (queue == NULL) return false; 

  Node* newNode = (Node*)malloc(sizeof(Node));
  if (newNode == NULL) return false;

  newNode->process = newProcess;
  newNode->next = NULL;


  if (isEmpty(queue)) {
    // fila vazia
    queue->head = newNode;
  } else {
    queue->tail->next = newNode;
  }

  queue->tail = newNode;
  queue->size++;
  return true;
}

bool dequeue(Queue* queue, Process* process) {
  if (queue == NULL || isEmpty(queue)) {
    return false;
  }

  
  Node* removedNode = queue->head;
  *process = removedNode->process;

  queue->head = removedNode->next;
  if (queue->head == NULL) {
    // fila ficou vazia
    queue->tail = NULL;
  }

  queue->size--;
  free(removedNode);

  return true;
}

Process* peek(Queue* queue) {
  if (isEmpty(queue) || queue == NULL)
    return NULL;

  return &(queue->head->process);
}

void printQueue(Queue* queue) {
  Node* n = queue->head;
  while (n != NULL) {
    printf("%d ", n->process.pid);
    n = n->next;
  }
  puts("");
}

