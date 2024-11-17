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
  int pid;
  int tempoDeServico;
  int tempoChegada;
  int tempoIO;
  tiposIO tipoIO;
  estadosdoProcesso estado;
  int prioridade;
} Processo;

#endif
