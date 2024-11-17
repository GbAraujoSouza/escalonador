#include "constants.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Processo processos[MAX_PROCESS];
int numProcessos = 0;

void leProcessosArquivo(const char *nomeArquivo){
  FILE *arquivo = fopen(nomeArquivo, "r");
  if(arquivo == NULL){
    printf("Erro ao abrir arquivo\n");
    exit(1);
  }

  char line[256];
  while(fgets(line, sizeof(line), arquivo)){
    if(numProcessos>=MAX_PROCESS){
      printf("Numero maximo de processos atingido\n");
      break;
    }

    int pid, tempo_chegada, tempo_servico, numero_io;
    int tipos_io[MAX_IO_TIME];
    int tempos_io[MAX_IO_TIME];

    char *token = strtok(line, ";");
    pid = atoi(token);

    token = strtok(NULL, ";");
    tempo_chegada = atoi(token);

    token = strtok(NULL, ";");
    tempo_servico = atoi(token);

    token = strtok(NULL, ";");
    numero_io = atoi(token);

    for(int i=0; i<numero_io; i++){
      token = strtok(NULL, ";");
      tipos_io[i] = atoi(token);

      token = strtok(NULL, ";");
      tempos_io[i] = atoi(token);
    }

    
    //Aqui, associamos aos valores lidos do arquivo a cada processo
    processos[numProcessos].pid = pid;
    processos[numProcessos].tempoDeServico = tempo_servico;
    processos[numProcessos].tempoChegada = tempo_chegada;
    processos[numProcessos].estado = PRONTO;
    processos[numProcessos].prioridade = 0;

    for(int i=0; i<numero_io; i++){
      processos[numProcessos].tempoIO += tempos_io[i];
    }
    
    for(int i=0; i<numero_io; i++){
      if(tipos_io[i] == 0){
        processos[numProcessos].tipoIO = DISCO;
      }else if(tipos_io[i] == 1){
        processos[numProcessos].tipoIO = IMPRESSORA;
      }else if(tipos_io[i] == 2){
        processos[numProcessos].tipoIO = FITA;
      }
    }
    //Fim da associação

    numProcessos++;
  }

  fclose(arquivo);
}

void printarProcessos(){
  for(int i=0; i<numProcessos; i++){
    printf("PID: %d\n", processos[i].pid);
    printf("Tempo de Servico: %d\n", processos[i].tempoDeServico);
    printf("Tempo de Chegada: %d\n", processos[i].tempoChegada);
    printf("Tempo de IO: %d\n", processos[i].tempoIO);
    printf("Tipo de IO: %d\n", processos[i].tipoIO);
    printf("Estado: %d\n", processos[i].estado);
    printf("Prioridade: %d\n", processos[i].prioridade);
    printf("\n");
  }
}
