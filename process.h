#ifndef TYPES_H
#define TYPES_H

typedef enum{
  DISCO,
  FITA,
  IMPRESSORA,
} tiposIO;

typedef struct{
  int tempoIO;
  tiposIO tipoIO;
  int tempoChegada;
  int tempoInicio;
} IO;

typedef struct{
  int pid;
  int tempoDeServico;
  int tempoChegada;
  IO *io;
  int qntIO;
} Process;

#endif
