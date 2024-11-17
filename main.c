#include "process.h"
#include "queue.h"
#include <stdio.h>
#include<unistd.h>

#define QUANTUM 4

Queue *qNovosProcessos, *qAltaPrioridade, *qBaixaPrioridade;
Queue *qIODisco, *qIOFita, *qIOImpressora;
int numProcessos = 0;
int processosTerminados = 0;
int time = 0;

void alocaNovosProcessos() {
  while(!isEmpty(qNovosProcessos) && peek(qNovosProcessos)->tempoChegada <= time) {
    Process novoProcesso;
    dequeue(qNovosProcessos, &novoProcesso);
    enqueue(qAltaPrioridade, novoProcesso);
    printf("Novo processo com pid: %d\n", novoProcesso.pid);
  }
}

int main() {

  qNovosProcessos = alocaQueue();
  qAltaPrioridade = alocaQueue();
  qBaixaPrioridade = alocaQueue();

  qIODisco = alocaQueue();
  qIOFita = alocaQueue();
  qIOImpressora = alocaQueue();

  Process p1, p2, p3;
  p1.pid = 1;
  p1.estado = PRONTO;
  p1.tempoChegada = 0;
  p1.tempoDeServico = 5;

  p2.pid = 2;
  p2.estado = PRONTO;
  p2.tempoChegada = 1;
  p2.tempoDeServico = 2;

  p3.pid = 3;
  p3.estado = PRONTO;
  p3.tempoChegada = 10;
  p3.tempoDeServico = 4;

  enqueue(qNovosProcessos, p1);
  numProcessos++;
  enqueue(qNovosProcessos, p2);
  numProcessos++;
  enqueue(qNovosProcessos, p3);
  numProcessos++;

  int numProcessosRestantes;

  puts("----------INICIO----------");
  while(processosTerminados < numProcessos) {
    alocaNovosProcessos();
    printf("PIDs na fila de alta ");
    printQueue(qAltaPrioridade);

    printf("PIDs na fila de baixa ");
    printQueue(qBaixaPrioridade);

    sleep(2);

    numProcessosRestantes = qAltaPrioridade->size + qBaixaPrioridade->size;

    if (numProcessosRestantes == 0) {
      puts("Sem processos a executar");
      time++;
      continue;
    }

    // Pegar processo com a prioridade mais alta
    Process processoParaExecutar;
    if (qAltaPrioridade->size > 0)
      dequeue(qAltaPrioridade, &processoParaExecutar);
    else
      dequeue(qBaixaPrioridade, &processoParaExecutar);

    // checar se processo precisa de IO


    // executar processo na CPU
    if (processoParaExecutar.tempoDeServico <= QUANTUM) {
      // processo pode ser finalizado sem preampcao
      printf("Processo PID: %d executou por %du.t\n", processoParaExecutar.pid, processoParaExecutar.tempoDeServico);
      time += processoParaExecutar.tempoDeServico;
      printf("Processo PID: %d Terminou em %du.t\n",processoParaExecutar.pid, time);

      processosTerminados++;
    } else {
      // processo ira sofrer preampcao
      // int tempoDeServicoExecutado = processoParaExecutar.tempoDeServico;
      processoParaExecutar.tempoDeServico -= QUANTUM;
      printf("Processo PID: %d executou por %du.t\n", processoParaExecutar.pid, QUANTUM);

      enqueue(qBaixaPrioridade, processoParaExecutar);

      time += QUANTUM;
    }

    puts("");

  }

  puts("----------FIM----------");
  // while (tem processos) {
  //
  //   tratar novos processos
  //
  //   tratar processos saindo do io
  //
  //   if (! fila alta vazia)
  //      executa fila de alta
  //   else if (! fila de baixa vazia)
  //      executa fila de baixa
  //   
  //   incrementa clock
  // }


  return 0;
}
