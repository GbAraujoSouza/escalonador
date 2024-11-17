#include <stdlib.h>
#include "queue.h"
#include "constants.h"

Fila *criaFila(int tam) {
    Fila *fila = (Fila *) malloc(sizeof(Fila));
    fila->processo = (Processo *) malloc(sizeof(Processo) * MAX_PROCESS);
    fila->inicio = fila->fim=-1;
    fila->tam = 0;
    return fila;
}

void destroiFila(Fila *fila) {
  if(fila){    
      free(fila->processo);
      free(fila);
  }
}

int filaVazia(Fila *fila) {
    return fila->tam == 0;
}

void enfileira(Fila *fila, Processo *processo) {
    if(fila->tam == MAX_PROCESS) {
        printf("Fila cheia\n");
        return;
    }
    if(fila->fim == -1) {
        fila->inicio = fila->fim = 0;
    } else {
        fila->fim = (fila->fim + 1) % MAX_PROCESS;
    }
    fila->processo[fila->fim] = *processo;
    fila->tam++;
}

Processo *desenfileira(Fila *fila) {
    Processo *processo = (Processo *) malloc(sizeof(Processo));
    if(fila->tam == 0) {
        printf("Fila vazia\n");
        return NULL;
    }
    *processo = fila->processo[fila->inicio];
    if(fila->inicio == fila->fim) {
        fila->inicio = fila->fim = -1;
    } else {
        fila->inicio = (fila->inicio + 1) % MAX_PROCESS;
    }
    fila->tam--;
    return processo;
}
