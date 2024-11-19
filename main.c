#include "process.h"
#include "queue.c"
#include "read.c"
#include "process.h"
#include <stdio.h>
#include <unistd.h>
#define MAX_PROCESS 10
#define TIME_SLICE 4
#define MAX_SERVICE_TIME 20
#define MAX_IO_TIME 5

#define QUANTUM 5

//Tipos de IO e suas respectivas unidades de tempo
#define TEMPO_IMPRESSORA 8
#define TEMPO_FITA 3
#define TEMPO_DISCO 7


Queue *qNovosProcessos, *qAltaPrioridade, *qBaixaPrioridade;
Queue *qIODisco, *qIOFita, *qIOImpressora;
int numProcessos = 0;
int processosTerminados = 0;
int time = 0;

void alocaNovosProcessos()
{
  while (!isEmpty(qNovosProcessos) &&
         peek(qNovosProcessos)->tempoChegada <= time)
  {
    Process novoProcesso;
    dequeue(qNovosProcessos, &novoProcesso);
    if(contains(qAltaPrioridade, novoProcesso.pid) == 0)
      enqueue(qAltaPrioridade, novoProcesso);
    printf("Novo processo com pid: %d\n", novoProcesso.pid);
  }
}

int tempoDiscoConcluido = 0;
int tempoFitaConcluido = 0;
int tempoImpressoraConcluido = 0;

void liberaProcessosDoIO()
{
  while (!isEmpty(qIOFita))
  {
    Process *p = peek(qIOFita);
    int ioConcluido = 0;
    for (int i = 0; i < p->qntIO; i++)
    {
      if (p->io[i].tipoIO == FITA && p->io[i].tempoInicio + p->io[i].tempoIO <= time)
      {
        ioConcluido = 1;
        break;
      }
    }
    if (ioConcluido)
    {
      Process processoRetornado;
      dequeue(qIOFita, &processoRetornado);
      if(contains(qAltaPrioridade, processoRetornado.pid) == 0)
        enqueue(qAltaPrioridade, processoRetornado);
      printf("Processo PID: %d saiu da fila de Fita em %du.t.\n",
             processoRetornado.pid, time);
    }
    else
    {
      break;
    }
  }

  while (!isEmpty(qIOImpressora))
  {
    Process *p = peek(qIOImpressora);
    int ioConcluido = 0;
    for (int i = 0; i < p->qntIO; i++)
    {
      if (p->io[i].tipoIO == IMPRESSORA && p->io[i].tempoInicio + p->io[i].tempoIO <= time)
      {
        ioConcluido = 1;
        break;
      }
    }
    if (ioConcluido)
    {
      Process processoRetornado;
      dequeue(qIOImpressora, &processoRetornado);
      if(contains(qAltaPrioridade, processoRetornado.pid) == 0)
        enqueue(qAltaPrioridade, processoRetornado);
      printf("Processo PID: %d saiu da fila de Impressora em %du.t.\n",
             processoRetornado.pid, time);
    }
    else
    {
      break;
    }
  }

  while (!isEmpty(qIODisco))
  {
    Process *p = peek(qIODisco);
    int ioConcluido = 0;
    for (int i = 0; i < p->qntIO; i++)
    {
      if (p->io[i].tipoIO == DISCO && p->io[i].tempoInicio + p->io[i].tempoIO <= time)
      {
        ioConcluido = 1;
        break;
      }
    }
    if (ioConcluido)
    {
      Process processoRetornado;
      dequeue(qIODisco, &processoRetornado);
      if(contains(qAltaPrioridade, processoRetornado.pid) == 0)
        enqueue(qBaixaPrioridade, processoRetornado);
      printf("Processo PID: %d saiu da fila de Disco em %du.t.\n",
             processoRetornado.pid, time);
    }
    else
    {
      break;
    }
  }
}

void alocaInicial(){
  qNovosProcessos = alocaQueue();
  qAltaPrioridade = alocaQueue();
  qBaixaPrioridade = alocaQueue();

  qIODisco = alocaQueue();
  qIOFita = alocaQueue();
  qIOImpressora = alocaQueue();
}

void leProcessos(){
  printf("Digite o nome do arquivo a ser lido: ");
  char nomeArquivo[100];
  scanf("%s", nomeArquivo);
  Process *processos = leitura(nomeArquivo);
  for (int i = 0; i < numeroProcessos; i++)
  {
    if(contains(qNovosProcessos, processos[i].pid) == 0)
      enqueue(qNovosProcessos, processos[i]);
    numProcessos++;
  }
  sleep(1);
}

int main()
{

  alocaInicial();

  leProcessos();

  int numProcessosRestantes = 0;

  int pidProcessoFinalizados[numProcessos];
  int tempoProcessoFinalizados[numProcessos];

  puts("=-=-=-=-=-=-=-=-=-=-=-=-=INICIO=-=-=-=-=-=-=-=-=-=-=-=-=");
  while (processosTerminados < numProcessos)
  {
    printf("Time: %d\n", time);
    // tratar novos processos
    alocaNovosProcessos();

    // tratar processos vindos da fila de IO
    liberaProcessosDoIO();

    printf("PIDs na fila de alta: ");
    printQueue(qAltaPrioridade);

    printf("PIDs na fila de baixa: ");
    qBaixaPrioridade = removeDuplicatas(qBaixaPrioridade);
    printQueue(qBaixaPrioridade);

    if (!isEmpty(qIODisco))
    {
      printf("PIDs na fila de IO do Disco: ");
      printQueue(qIODisco);
    }

    if (!isEmpty(qIOFita))
    {
      printf("PIDs na fila de IO da Fita: ");
      printQueue(qIOFita);
    }

    if (!isEmpty(qIOImpressora))
    {
      printf("PIDs na fila de IO da Impressora: ");
      printQueue(qIOImpressora);
    }
    sleep(1);
    int numProcessosIO = qIODisco->size + qIOFita->size + qIOImpressora->size;
    numProcessosRestantes = qAltaPrioridade->size + qBaixaPrioridade->size + numProcessosIO;

    if (numProcessosRestantes == 0)
    {
      puts("Sem processos a executar\n");
      time+= QUANTUM;
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
    for (int i = 0; i < processoParaExecutar.qntIO; i++)
    {
      // olhar a lista de IO e pegar aquele que nao foi pedido
      if (processoParaExecutar.io[i].tempoChegada != -1)
      {
        instanteProximoIO = processoParaExecutar.io[i].tempoChegada;

        // -1 no tempo de chegada indica que o IO ja foi pedido
        processoParaExecutar.io[i].tempoChegada = -1;
        indiceProximoIO = i;
        break;
      }
    }

    if (instanteProximoIO != -1)
    {
      // processo atual precisa de IO
      if (instanteProximoIO <= (time + QUANTUM))
      {
        // pedido de IO ocorrera na janela de tempo atual
        processoParaExecutar.tempoDeServico -= instanteProximoIO;

        // armazenar o processo na respectiva fila de IO;
        processoParaExecutar.io[indiceProximoIO].tempoInicio = instanteProximoIO+time;
        IO proximoIO = processoParaExecutar.io[indiceProximoIO];

        time += instanteProximoIO;
        printf("Processo PID: %d executou por %du.t.\n",
               processoParaExecutar.pid, instanteProximoIO);

        switch (proximoIO.tipoIO)
        {
        case DISCO:
          if(contains(qIODisco, processoParaExecutar.pid) == 0)
            enqueue(qIODisco, processoParaExecutar);
          printf("Processo PID: %d pediu operacao de IO Disco em %du.t.\n",
                 processoParaExecutar.pid, time);
          break;
        case IMPRESSORA:
          if(contains(qIOImpressora, processoParaExecutar.pid) == 0)
            enqueue(qIOImpressora, processoParaExecutar);
          printf("Processo PID: %d pediu operacao de IO Impressora em %du.t.\n",
                 processoParaExecutar.pid, time);
          break;
        case FITA:
          if(contains(qIOFita, processoParaExecutar.pid) == 0)
            enqueue(qIOFita, processoParaExecutar);
          printf("Processo PID: %d pediu operacao de IO Fita em %du.t.\n",
                 processoParaExecutar.pid, time);
          break;
        }
      }
      else
      {
        // pedido de IO NAO sera na janela de tempo atual
        // processo sofrera preampcao
        processoParaExecutar.tempoDeServico -= QUANTUM;
        printf("Processo PID: %d executou por %du.t.\n",
               processoParaExecutar.pid, QUANTUM);

        if(contains(qAltaPrioridade, processoParaExecutar.pid) == 0)
          enqueue(qBaixaPrioridade, processoParaExecutar);

        time += QUANTUM;
      }
    }
    else if (processoParaExecutar.tempoDeServico <= QUANTUM)
    {
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
    }
    else
    {
      // processo ira sofrer preampcao
      // int tempoDeServicoExecutado = processoParaExecutar.tempoDeServico;
      processoParaExecutar.tempoDeServico -= QUANTUM;
      printf("Processo PID: %d executou por %du.t.\n", processoParaExecutar.pid,
             QUANTUM);

      if(contains(qAltaPrioridade, processoParaExecutar.pid) == 0)
        enqueue(qBaixaPrioridade, processoParaExecutar);

      time += QUANTUM;
    }

    puts("");
  }

  puts("=-=-=-=-=-=-=-=-=-=-=-=-=-FIM-=-=-=-=-=-=-=-=-=-=-=-=-=");
  printf("Tempo Final: %d\n", time);

  printf("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n");
  for (int i = 0; i < numProcessos; i++)
  {
    printf("P%d foi finalizado no tempo %d u.t.\n",
           pidProcessoFinalizados[i], tempoProcessoFinalizados[i]);
    printf("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n");
  }

  return 0;
}
