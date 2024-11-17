#ifndef QUEUE_H
#define QUEUE_H
#include <stdlib.h>
#include <stdio.h>
#include "constants.h"
#include "types.h"

typedef struct{
  Processo *processo;
  int inicio;
  int fim;
  int tam;
} Fila;

Fila *criaFila(int tam);
void destroiFila(Fila *fila);
int filaVazia(Fila *fila);
void enfileira(Fila *fila, Processo *processo);
Processo *desenfileira(Fila *fila);


#endif
