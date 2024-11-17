#include "constants.h"
#include "process.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Process processos[MAX_PROCESS];
int numProcessos = 0;
int numero_io = 0;

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
      tempos_io[i] = atoi(token);

      token = strtok(NULL, ";");
      tempo_chegada_io[i] = atoi(token);
    }

    
    //Aqui, associamos aos valores lidos do arquivo a cada processo
    processos[numProcessos].pid = pid;
    processos[numProcessos].tempoDeServico = tempo_servico;
    processos[numProcessos].tempoChegada = tempo_chegada;
    processos[numProcessos].estado = PRONTO;

    processos[numProcessos].io = (IO *) malloc(sizeof(IO) * numero_io);
    for(int i=0; i<numero_io; i++){
      processos[numProcessos].io[i].tempoIO = tempos_io[i];
      processos[numProcessos].io[i].tipoIO = tipos_io[i];
      processos[numProcessos].io[i].tempoChegada = tempo_chegada_io[i];
    }


    //Fim da associação

    numProcessos++;
  }

  fclose(arquivo);
}

void printarProcessos(){
  for(int i=0; i<numProcessos; i++){
    printf("Processo %d\n", processos[i].pid);
    printf("Tempo de chegada: %d\n", processos[i].tempoChegada);
    printf("Tempo de servico: %d\n", processos[i].tempoDeServico);
    printf("Numero de IOs: %d\n", MAX_IO_TIME);
    for(int j=0; j<=numero_io; j++){
      if(processos[i].io[j].tempoIO == 0){
        break;
      }
      printf("IO %d\n", j);
      printf("Tipo: %d\n", processos[i].io[j].tipoIO);
      printf("Tempo de IO: %d\n", processos[i].io[j].tempoIO);
      printf("Tempo de chegada: %d\n", processos[i].io[j].tempoChegada);
    }
    printf("\n");
  }

}

int main() {
  leProcessosArquivo("teste1.txt");
  printarProcessos();

  return 0;
}
