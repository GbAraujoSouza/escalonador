#ifndef READ_H
#define READ_H

#include "process.h"

#define MAX_PROCESS 10
#define TIME_SLICE 4
#define MAX_SERVICE_TIME 20
#define MAX_IO_TIME 5

//Tipos de IO e suas respectivas unidades de tempo
#define TEMPO_IMPRESSORA 20
#define TEMPO_FITA 12
#define TEMPO_DISCO 4

Process* leProcessosArquivo(const char *nomeArquivo, int* numeroProcessos);
void printarProcessos(int numeroProcessos);
Process* leitura(char *nomeArquivo, int* numeroProcessos);


#endif
