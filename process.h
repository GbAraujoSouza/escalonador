#ifndef TYPES_H
#define TYPES_H

typedef enum{
  PRONTO,
  OCUPADO,
  ESPERANDO,
  TERMINADO
} estadosdoProcesso;

typedef enum{
  DISCO,
  IMPRESSORA,
  FITA
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
  estadosdoProcesso estado;
} Process;

#endif
