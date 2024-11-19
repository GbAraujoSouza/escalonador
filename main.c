#include "process.h"
#include "queue.h"
#include "read.h"
#include <stdio.h>
#include <unistd.h>

#define QUANTUM 4


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
      Process processoRetornado;
      dequeue(qIOFita, &processoRetornado);
      enqueue(qAltaPrioridade, processoRetornado);
      printf("Processo PID: %d saiu da fila de Fita em %du.t.\n",
             processoRetornado.pid, tempoFitaConcluido + io.tempoIO);
    } else {
      break;
    }
  }

  while (!isEmpty(qIOImpressora)) {
    Process *p = peek(qIOImpressora);
    IO io;

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
      Process processoRetornado;
      dequeue(qIOImpressora, &processoRetornado);
      enqueue(qAltaPrioridade, processoRetornado);
      printf("Processo PID: %d saiu da fila de Impressao em %du.t.\n",
             processoRetornado.pid, tempoImpressoraConcluido + io.tempoIO);
    } else {
      break;
    }
  }

  while (!isEmpty(qIODisco)) {
    Process *p = peek(qIODisco);
    IO io;

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
      Process processoRetornado;
      dequeue(qIODisco, &processoRetornado);
      enqueue(qBaixaPrioridade, processoRetornado);
      printf("Processo PID: %d saiu da fila de Disco em %du.t.\n",
             processoRetornado.pid, tempoDiscoConcluido + io.tempoIO);
    } else {
      break;
    }
  }
}

void alocaInicial() {
  qNovosProcessos = alocaQueue();
  qAltaPrioridade = alocaQueue();
  qBaixaPrioridade = alocaQueue();

  qIODisco = alocaQueue();
  qIOFita = alocaQueue();
  qIOImpressora = alocaQueue();
}

void leProcessos() {
  printf("Digite o nome do arquivo a ser lido: ");
  char nomeArquivo[100];
  scanf("%s", nomeArquivo);
  Process *processos = leitura(nomeArquivo, &numProcessos);
  for (int i = 0; i < numProcessos; i++) {
    enqueue(qNovosProcessos, processos[i]);
  }
  sleep(1);
}

int main() {

  alocaInicial();

  leProcessos();

  int numProcessosRestantes = 0;

  int pidProcessoFinalizados[numProcessos];
  int tempoProcessoFinalizados[numProcessos];

  puts("=-=-=-=-=-=-=-=-=-=-=-=-=INICIO=-=-=-=-=-=-=-=-=-=-=-=-=");
  while (processosTerminados < numProcessos) {
    printf("Time: %d\n", time);
    // tratar novos processos
    alocaNovosProcessos();

    // tratar processos vindos da fila de IO
    liberaProcessosDoIO();

    printf("PIDs na fila de alta: ");
    printQueue(qAltaPrioridade);

    printf("PIDs na fila de baixa: ");
    printQueue(qBaixaPrioridade);

    sleep(1);

    numProcessosRestantes = qAltaPrioridade->size + qBaixaPrioridade->size;

    if (numProcessosRestantes == 0) {
      puts("Sem processos a executar\n");
      time += 1;
      continue;
    }

    // Pegar processo com a prioridade mais alta
    Process processoParaExecutar;
    if (qAltaPrioridade->size > 0) {
      dequeue(qAltaPrioridade, &processoParaExecutar);
      printf("Peguei o pid %d da fila de alta\n", processoParaExecutar.pid);
    } else {
      dequeue(qBaixaPrioridade, &processoParaExecutar);
      printf("Peguei o pid %d da fila de baixa\n", processoParaExecutar.pid);
    }

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
        processoParaExecutar.tempoDeServico -= instanteProximoIO;

        // armazenar o processo na respectiva fila de IO;
        processoParaExecutar.io[indiceProximoIO].tempoInicio =
            instanteProximoIO + time;
        IO proximoIO = processoParaExecutar.io[indiceProximoIO];

        time += instanteProximoIO;
        printf("Processo PID: %d executou por %du.t.\n",
               processoParaExecutar.pid, instanteProximoIO);

        printf("tipo io: %d\n", proximoIO.tipoIO);
        switch (proximoIO.tipoIO) {
        case DISCO:
          enqueue(qIODisco, processoParaExecutar);
          printf("Processo PID: %d pediu operacao de IO Disco em %du.t.\n",
                 processoParaExecutar.pid, time);
          break;
        case IMPRESSORA:
          enqueue(qIOImpressora, processoParaExecutar);
          printf("Processo PID: %d pediu operacao de IO Impressora em %du.t.\n",
                 processoParaExecutar.pid, time);
          break;
        case FITA:
          enqueue(qIOFita, processoParaExecutar);
          printf("Processo PID: %d pediu operacao de IO Fita em %du.t.\n",
                 processoParaExecutar.pid, time);
          break;
        }
      } else {
        // pedido de IO NAO sera na janela de tempo atual
        // processo sofrera preampcao
        processoParaExecutar.tempoDeServico -= QUANTUM;
        printf("Processo PID: %d executou por %du.t.\n",
               processoParaExecutar.pid, QUANTUM);

        enqueue(qBaixaPrioridade, processoParaExecutar);

        time += QUANTUM;
      }
    } else if (processoParaExecutar.tempoDeServico <= QUANTUM) {
      // executar processo na CPU
      // processo pode ser finalizado sem preampcao
      printf("Processo PID: %d executou por %du.t.\n", processoParaExecutar.pid,
             processoParaExecutar.tempoDeServico);
      time += processoParaExecutar.tempoDeServico;
      printf("Processo PID: %d Terminou em %du.t.\n", processoParaExecutar.pid,
             time);

      pidProcessoFinalizados[processosTerminados] = processoParaExecutar.pid;

      tempoProcessoFinalizados[processosTerminados] = time;

      processosTerminados++;
    } else {
      // processo ira sofrer preampcao
      processoParaExecutar.tempoDeServico -= QUANTUM;
      printf("Processo PID: %d executou por %du.t.\n", processoParaExecutar.pid,
             QUANTUM);

      enqueue(qBaixaPrioridade, processoParaExecutar);

      time += QUANTUM;
    }

    if (!isEmpty(qIODisco)) {
      printf("PIDs na fila de IO do Disco: ");
      printQueue(qIODisco);
    }

    if (!isEmpty(qIOFita)) {
      printf("PIDs na fila de IO da Fita: ");
      printQueue(qIOFita);
    }

    if (!isEmpty(qIOImpressora)) {
      printf("PIDs na fila de IO da Impressora: ");
      printQueue(qIOImpressora);
    }

    puts("");
  }

  puts("=-=-=-=-=-=-=-=-=-=-=-=-=-FIM-=-=-=-=-=-=-=-=-=-=-=-=-=");
  printf("Tempo Final: %d\n", time);

  printf("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n");
  for (int i = 0; i < numProcessos; i++) {
    printf("P%d foi finalizado no tempo %d u.t.\n", pidProcessoFinalizados[i],
           tempoProcessoFinalizados[i]);
    printf("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n");
  }

  return 0;
}
