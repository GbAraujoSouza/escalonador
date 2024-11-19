#ifndef READ_H
#define READ_H

#include "process.h"

#define MAX_PROCESS 20
#define MAX_SERVICE_TIME 20
#define MAX_IO 5

//Tipos de IO e suas respectivas unidades de tempo
#define TEMPO_IMPRESSORA 20
#define TEMPO_FITA 10
#define TEMPO_DISCO 5

Process* leProcessosArquivo(const char *nomeArquivo, int* numeroProcessos);
void printarProcessos(int numeroProcessos);
Process* leitura(char *nomeArquivo, int* numeroProcessos);


#endif
