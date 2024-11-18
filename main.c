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

void alocaNovosProcessos()
{
  while (!isEmpty(qNovosProcessos) &&
         peek(qNovosProcessos)->tempoChegada <= time)
  {
    Process novoProcesso;
    dequeue(qNovosProcessos, &novoProcesso);
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
    IO io;
    // assumimos que um processo pode ter ate uma operacao de IO de cada tipo
    for (int i = 0; i < p->qntIO; i++)
    {
      if (p->io[i].tipoIO == FITA)
      {
        io = p->io[i];
        break;
      }
    }
    if (io.tempoInicio + io.tempoIO <= time)
    {
      Process processoRetornado;
      dequeue(qIOFita, &processoRetornado);
      enqueue(qBaixaPrioridade, processoRetornado);
      printf("Processo PID: %d saiu da fila de Fita em %du.t\n",
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
    IO io;
    // assumimos que um processo pode ter ate uma operacao de IO de cada tipo
    for (int i = 0; i < p->qntIO; i++)
    {
      if (p->io[i].tipoIO == IMPRESSORA)
      {
        io = p->io[i];
        break;
      }
    }
    if (io.tempoInicio + io.tempoIO <= time)
    {
      Process processoRetornado;
      dequeue(qIOImpressora, &processoRetornado);
      enqueue(qBaixaPrioridade, processoRetornado);
      printf("Processo PID: %d saiu da fila de Impressora em %du.t\n",
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
    IO io;
    // assumimos que um processo pode ter ate uma operacao de IO de cada tipo
    for (int i = 0; i < p->qntIO; i++)
    {
      if (p->io[i].tipoIO == DISCO)
      {
        io = p->io[i];
        break;
      }
    }
    if (io.tempoInicio + io.tempoIO <= time)
    {
      Process processoRetornado;
      dequeue(qIODisco, &processoRetornado);
      enqueue(qBaixaPrioridade, processoRetornado);
      printf("Processo PID: %d saiu da fila de Disco em %du.t\n",
             processoRetornado.pid, time);
    }
    else
    {
      break;
    }
  }
}

int main()
{

  qNovosProcessos = alocaQueue();
  qAltaPrioridade = alocaQueue();
  qBaixaPrioridade = alocaQueue();

  qIODisco = alocaQueue();
  qIOFita = alocaQueue();
  qIOImpressora = alocaQueue();

  Process p1, p2, p3 ,p4,p5;
  p1.pid = 1;
  p1.estado = PRONTO;
  p1.tempoChegada = 0;
  p1.tempoDeServico = 13;
  p1.qntIO = 0;

  p1.io = (IO *)malloc(sizeof(IO) * 1);
  p1.io[0].tempoChegada = 4;
  p1.io[0].tipoIO = IMPRESSORA;
  p1.io[0].tempoIO = 7;


  p2.pid = 2;
  p2.estado = PRONTO;
  p2.tempoChegada = 4;
  p2.tempoDeServico = 11;
  p2.qntIO = 2;

  p2.io = (IO *)malloc(sizeof(IO) * 2);
  p2.io[0].tempoChegada = 2;
  p2.io[0].tipoIO = FITA;
  p2.io[0].tempoIO = 4;

  p2.io[1].tempoChegada = 6;
  p2.io[1].tipoIO = IMPRESSORA;
  p2.io[1].tempoIO = 7;
  
  p3.pid = 3;
  p3.estado = PRONTO;
  p3.tempoChegada = 5;
  p3.tempoDeServico = 7;
  p3.qntIO = 0;

  p4.pid = 4;
  p4.estado = PRONTO;
  p4.tempoChegada=7;
  p4.tempoDeServico=8;
  p4.qntIO = 0;

  p5.pid = 5;
  p5.estado = PRONTO;
  p5.tempoChegada = 10;
  p5.tempoDeServico = 16;
  p5.qntIO = 0;

  p5.io = (IO *)malloc(sizeof(IO) * 2);
  p5.io[0].tempoChegada = 2;
  p5.io[0].tipoIO = IMPRESSORA;
  p5.io[0].tempoIO = 7;

  p5.io[1].tempoChegada = 6;
  p5.io[1].tipoIO = FITA;
  p5.io[1].tempoIO = 4;

  enqueue(qNovosProcessos, p1);
  numProcessos++;
  enqueue(qNovosProcessos, p2);
  numProcessos++;
  enqueue(qNovosProcessos, p3);
  numProcessos++;
  enqueue(qNovosProcessos, p4);
  numProcessos++;
  enqueue(qNovosProcessos, p5);
  numProcessos++;
  

  int numProcessosRestantes;

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

    numProcessosRestantes = qAltaPrioridade->size + qBaixaPrioridade->size;

    if (numProcessosRestantes == 0)
    {
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
        int tempoExecutado = (instanteProximoIO - time);
        // pedido de IO ocorrera na janela de tempo atual
        processoParaExecutar.tempoDeServico -= tempoExecutado;

        // armazenar o processo na respectiva fila de IO;
        processoParaExecutar.io[indiceProximoIO].tempoInicio = instanteProximoIO;
        IO proximoIO = processoParaExecutar.io[indiceProximoIO];

        time += tempoExecutado;
        printf("Processo PID: %d executou por %du.t\n",
               processoParaExecutar.pid, instanteProximoIO);

        switch (proximoIO.tipoIO)
        {
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
      }
      else
      {
        // pedido de IO NAO sera na janela de tempo atual
        // processo sofrera preampcao
        processoParaExecutar.tempoDeServico -= QUANTUM;
        printf("Processo PID: %d executou por %du.t.\n",
               processoParaExecutar.pid, QUANTUM);

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
      printf("Processo PID: %d executou por %du.t\n", processoParaExecutar.pid,
             QUANTUM);

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
