#include "process.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_PROCESS 10
#define TIME_SLICE 4
#define MAX_SERVICE_TIME 20
#define MAX_IO_TIME 5

//Tipos de IO e suas respectivas unidades de tempo
#define TEMPO_IMPRESSORA 20
#define TEMPO_FITA 12
#define TEMPO_DISCO 4

Process processos[MAX_PROCESS];
int numeroProcessos = 0;
int numero_io = 0;

Process* leProcessosArquivo(const char *nomeArquivo){
  FILE *arquivo = fopen(nomeArquivo, "r");
  if(arquivo == NULL){
    printf("Erro ao abrir arquivo\n");
    exit(1);
  }

  char line[256];
  while(fgets(line, sizeof(line), arquivo)){
    if(numeroProcessos>=MAX_PROCESS){
      printf("Numero maximo de processos atingido\n");
      break;
    }

    int pid, tempo_chegada, tempo_servico;
    int tipos_io[MAX_IO_TIME];
    int tempos_io[MAX_IO_TIME];
    int tempo_chegada_io[MAX_IO_TIME];

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
      tempo_chegada_io[i] = atoi(token);

      if(tipos_io[i] == 0){
        tempos_io[i] = TEMPO_DISCO;
      }else if(tipos_io[i] == 1){
        tempos_io[i] = TEMPO_FITA;
      }else if(tipos_io[i] == 2){
        tempos_io[i] = TEMPO_IMPRESSORA;
      }
    }

    
    //Aqui, associamos aos valores lidos do arquivo a cada processo
    processos[numeroProcessos].pid = pid;
    processos[numeroProcessos].tempoDeServico = tempo_servico;
    processos[numeroProcessos].tempoChegada = tempo_chegada;
    processos[numeroProcessos].estado = PRONTO;
    processos[numeroProcessos].qntIO = numero_io;

    processos[numeroProcessos].io = (IO *) malloc(sizeof(IO) * numero_io);
    for(int i=0; i<numero_io; i++){
      processos[numeroProcessos].io[i].tempoIO = tempos_io[i];
      processos[numeroProcessos].io[i].tipoIO = tipos_io[i];
      processos[numeroProcessos].io[i].tempoChegada = tempo_chegada_io[i];
    }


    //Fim da associação

    numeroProcessos++;
  }

  fclose(arquivo);
  return processos;
}

void printarProcessos(){
  for(int i=0; i<numeroProcessos; i++){
    printf("Processo %d\n", processos[i].pid);
    printf("Tempo de chegada: %d\n", processos[i].tempoChegada);
    printf("Tempo de servico: %d\n", processos[i].tempoDeServico);
    printf("Numero de IOs: %d\n", processos[i].qntIO);
    for(int j=0; j<=numero_io; j++){
      if(processos[i].io[j].tempoIO == 0){
        break;
      }
      printf("IO %d\n", j+1);
      if(processos[i].io[j].tipoIO == 0){
        printf("DISCO\n");
      }else if(processos[i].io[j].tipoIO == 1){
        printf("FITA\n");
      }else{
        printf("IMPRESSORA\n");
      }
      printf("Tempo de IO: %d\n", processos[i].io[j].tempoIO);
      printf("Tempo de chegada: %d\n", processos[i].io[j].tempoChegada);
    }
    printf("\n");
  }

}

Process* leitura(char *nomeArquivo){
  Process* processos = malloc(sizeof(Process) * MAX_PROCESS);
  processos = leProcessosArquivo(nomeArquivo);
  printarProcessos();

  return processos;
}
