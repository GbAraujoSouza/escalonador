#include "process.h"
#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define QUANTUM 5

Queue *qNovosProcessos, *qAltaPrioridade, *qBaixaPrioridade;
Queue *qIODisco, *qIOFita, *qIOImpressora;
int numProcessos = 0;
int processosTerminados = 0;
int time = 0;

void alocaNovosProcessos() {
  while (!isEmpty(qNovosProcessos) &&
         peek(qNovosProcessos)->tempoChegada <= time) {
    Process novoProcesso;
    dequeue(qNovosProcessos, &novoProcesso);
    enqueue(qAltaPrioridade, novoProcesso);
    printf("Novo processo com pid: %d\n", novoProcesso.pid);
  }
}

int tempoDiscoConcluido = 0;
int tempoFitaConcluido = 0;
int tempoImpressoraConcluido = 0;

void liberaProcessosDoIO() {
  while (!isEmpty(qIOFita)) {
    Process *p = peek(qIOFita);
    IO io;
    // assumimos que um processo pode ter ate uma operacao de IO de cada tipo
    for (int i = 0; i < p->qntIO; i++) {
      if (p->io[i].tipoIO == FITA) {
        io = p->io[i];
        break;
      }
    }
    tempoFitaConcluido = (tempoFitaConcluido > io.tempoInicio)
                             ? tempoFitaConcluido
                             : io.tempoInicio;

    if (tempoFitaConcluido + io.tempoIO <= time) {
      // operacao de IO ja foi concluida
      printf("Processo PID: %d saiu da fila de Fita em %du.t\n", p->pid,
             tempoFitaConcluido + io.tempoIO);

      tempoFitaConcluido += io.tempoIO;
      Process processoRetornado;
      dequeue(qIOFita, &processoRetornado);
      enqueue(qAltaPrioridade, processoRetornado);
    } else {
      break;
    }
  }

  while (!isEmpty(qIOImpressora)) {
    Process *p = peek(qIOImpressora);
    IO io;
    // assumimos que um processo pode ter ate uma operacao de IO de cada tipo
    for (int i = 0; i < p->qntIO; i++) {
      if (p->io[i].tipoIO == IMPRESSORA) {
        io = p->io[i];
        break;
      }
    }
    tempoImpressoraConcluido = (tempoImpressoraConcluido > io.tempoInicio)
                                   ? tempoImpressoraConcluido
                                   : io.tempoInicio;

    if (tempoImpressoraConcluido + io.tempoIO <= time) {
      // operacao de IO ja foi concluida
      printf("Processo PID: %d saiu da fila de Impressao em %du.t\n", p->pid,
             time);

      tempoImpressoraConcluido += io.tempoIO;
      Process processoRetornado;
      dequeue(qIOImpressora, &processoRetornado);
      enqueue(qAltaPrioridade, processoRetornado);
    } else {
      break;
    }
  }

  while (!isEmpty(qIODisco)) {
    Process *p = peek(qIODisco);
    IO io;
    // assumimos que um processo pode ter ate uma operacao de IO de cada tipo
    for (int i = 0; i < p->qntIO; i++) {
      if (p->io[i].tipoIO == DISCO) {
        io = p->io[i];
        break;
      }
    }
    tempoDiscoConcluido = (tempoDiscoConcluido > io.tempoInicio)
                              ? tempoDiscoConcluido
                              : io.tempoInicio;

    if (tempoDiscoConcluido + io.tempoIO <= time) {
      // operacao de IO ja foi concluida
      printf("Processo PID: %d saiu da fila de Disco em %du.t\n", p->pid,
             tempoDiscoConcluido + io.tempoIO);

      tempoDiscoConcluido += io.tempoIO;
      Process processoRetornado;
      dequeue(qIODisco, &processoRetornado);
      enqueue(qBaixaPrioridade, processoRetornado);
    } else {
      break;
    }
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
  p1.tempoDeServico = 31;
  p1.qntIO = 0;

  // p1.io = (IO *)malloc(sizeof(IO) * 1);
  // p1.io[0].tempoChegada = 1;
  // p1.io[0].tipoIO = IMPRESSORA;
  // p1.io[0].tempoIO = 20;

  // p1.io[1].tempoChegada = 30;
  // p1.io[1].tipoIO = FITA;
  // p1.io[1].tempoIO = 12;

  p2.pid = 2;
  p2.estado = PRONTO;
  p2.tempoChegada = 2;
  p2.tempoDeServico = 7;
  p2.qntIO = 0;

  // p2.io = (IO *)malloc(sizeof(IO) * 2);
  // p2.io[0].tempoChegada = 2;
  // p2.io[0].tipoIO = DISCO;
  // p2.io[0].tempoIO = 5;
  //
  p3.pid = 3;
  p3.estado = PRONTO;
  p3.tempoChegada = 3;
  p3.tempoDeServico = 23;
  p3.qntIO = 0;

  // p3.io = (IO *)malloc(sizeof(IO) * 1);
  // p3.io[0].tempoChegada = 1;
  // p3.io[0].tipoIO = IMPRESSORA;
  // p3.io[0].tempoIO = 20;

  enqueue(qNovosProcessos, p1);
  numProcessos++;
  enqueue(qNovosProcessos, p2);
  numProcessos++;
  enqueue(qNovosProcessos, p3);
  numProcessos++;

  int numProcessosRestantes;

  puts("----------INICIO----------");
  while (processosTerminados < numProcessos) {
    printf("Time: %d\n", time);
    // tratar novos processos
    alocaNovosProcessos();

    // tratar processos vindos da fila de IO
    liberaProcessosDoIO();

    printf("PIDs na fila de alta ");
    printQueue(qAltaPrioridade);

    printf("PIDs na fila de baixa ");
    printQueue(qBaixaPrioridade);

    // sleep(1);

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
    int instanteProximoIO = -1;
    int indiceProximoIO = -1;
    for (int i = 0; i < processoParaExecutar.qntIO; i++) {
      // olhar a lista de IO e pegar aquele que nao foi pedido
      if (processoParaExecutar.io[i].tempoChegada != -1) {
        instanteProximoIO = processoParaExecutar.io[i].tempoChegada;

        // -1 no tempo de chegada indica que o IO ja foi pedido
        processoParaExecutar.io[i].tempoChegada = -1;
        indiceProximoIO = i;
        break;
      }
    }

    if (instanteProximoIO != -1) {
      // processo atual precisa de IO
      if (instanteProximoIO <= (time + QUANTUM)) {
        // pedido de IO ocorrera na janela de tempo atual
        processoParaExecutar.tempoDeServico -= (instanteProximoIO - time);

        // armazenar o processo na respectiva fila de IO;
        processoParaExecutar.io[indiceProximoIO].tempoInicio = instanteProximoIO;
        IO proximoIO = processoParaExecutar.io[indiceProximoIO];

        time += instanteProximoIO;
        printf("Processo PID: %d executou por %du.t\n",
               processoParaExecutar.pid, instanteProximoIO);

        switch (proximoIO.tipoIO) {
        case DISCO:
          enqueue(qIODisco, processoParaExecutar);
          printf("Processo PID: %d pediu operacao de IO Disco em %du.t\n",
                 processoParaExecutar.pid, time);
          break;
        case IMPRESSORA:
          enqueue(qIOImpressora, processoParaExecutar);
          printf("Processo PID: %d pediu operacao de IO Impressora em %du.t\n",
                 processoParaExecutar.pid, time);
          break;
        case FITA:
          enqueue(qIOFita, processoParaExecutar);
          printf("Processo PID: %d pediu operacao de IO Fita em %du.t\n",
                 processoParaExecutar.pid, time);
          break;
        }

      } else {
        // pedido de IO NAO sera na janela de tempo atual
        // processo sofrera preampcao
        processoParaExecutar.tempoDeServico -= QUANTUM;
        printf("Processo PID: %d executou por %du.t\n",
               processoParaExecutar.pid, QUANTUM);

        enqueue(qBaixaPrioridade, processoParaExecutar);

        time += QUANTUM;
      }
    } else if (processoParaExecutar.tempoDeServico <= QUANTUM) {
      // executar processo na CPU
      // processo pode ser finalizado sem preampcao
      printf("Processo PID: %d executou por %du.t\n", processoParaExecutar.pid,
             processoParaExecutar.tempoDeServico);
      time += processoParaExecutar.tempoDeServico;
      printf("Processo PID: %d Terminou em %du.t\n", processoParaExecutar.pid,
             time);

      processosTerminados++;
    } else {
      // processo ira sofrer preampcao
      // int tempoDeServicoExecutado = processoParaExecutar.tempoDeServico;
      processoParaExecutar.tempoDeServico -= QUANTUM;
      printf("Processo PID: %d executou por %du.t\n", processoParaExecutar.pid,
             QUANTUM);

      enqueue(qBaixaPrioridade, processoParaExecutar);

      time += QUANTUM;
    }

    puts("");
  }

  puts("----------FIM----------");
  printf("Tempo Final: %d\n", time);
  printQueue(qIOImpressora);
  printQueue(qIOFita);
  printQueue(qIODisco);
  printQueue(qAltaPrioridade);

  return 0;
}
