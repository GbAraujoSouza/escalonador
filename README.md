<div class="titlepage" markdown="1">

**INSTITUTO DE COMPUTAÇÃO**  
**ARQUITETURA DE COMPUTADORES E SISTEMAS OPERACIONAIS – 2024-2**  
**Trabalho 1 - SIMULAÇÃO DE ESCALONAMENTO DE PROCESSOS**  
**Integrantes do Grupo:**  
Francisco Theodoro Arantes Florencio - 123099825  
Rafael Oliveira Chaffin - 121045260  
Gabriel de Araujo de Souza - 119053914  
**Data de Entrega: 19/11/2024**  

</div>

# Introdução [introdução]

O presente trabalho, desenvolvido no contexto da disciplina
**Arquitetura de Computadores e Sistemas Operacionais (ACSO)**, tem como
foco a simulação de algoritmos de escalonamento de processos, com ênfase
na estratégia *Round Robin* com Feedback. Essa abordagem é fundamental
para compreender o funcionamento de sistemas operacionais modernos, que
precisam gerenciar eficientemente os recursos computacionais para
atender múltiplos processos simultaneamente.

Por meio deste projeto, os alunos serão desafiados a projetar,
implementar e testar um simulador em linguagem C, explorando conceitos
essenciais de escalonamento, filas de prioridade e gerência de
processos. Além disso, o trabalho busca desenvolver habilidades práticas
relacionadas à organização de equipes e à produção de documentação
técnica, aspectos indispensáveis na formação profissional.

# Objetivos [objetivos]

Os objetivos deste trabalho são:

1.  **Compreender e aplicar conceitos fundamentais de sistemas
    operacionais**: Estudar e implementar o algoritmo de escalonamento
    *Round Robin* com Feedback, analisando seu impacto na eficiência e
    organização do processamento de tarefas.

2.  **Desenvolver habilidades práticas em programação de sistemas**:
    Projetar e implementar um simulador funcional utilizando a linguagem
    C, com base nas premissas estabelecidas.

3.  **Fomentar o trabalho em equipe**: Dividir responsabilidades de
    forma equilibrada entre os integrantes do grupo, promovendo
    colaboração e desenvolvimento coletivo.

4.  **Elaborar documentação técnica**: Redigir um relatório que descreva
    claramente os objetivos, premissas, resultados e análises
    realizadas, fortalecendo a capacidade de comunicação técnica.

5.  **Aprimorar o aprendizado prático**: Realizar simulações que
    evidenciem o comportamento do escalonador, permitindo uma
    compreensão mais profunda de sua lógica e desafios operacionais.

# Premissas

Liste as premissas estabelecidas pelo grupo para o desenvolvimento do
simulador:

-   **Limite máximo de processos criados**: O simulador suporta até **20
    processos**, conforme definido pelo grupo.

-   **Limite de IO’s**: O simulador suporta até **1 ou 0 IO’s de cada
    tipo para cada processo**, conforme definido pelo grupo.

**Tempos de serviço e de I/O**: Os tempos de CPU e número máximo de I/O
são configurados por meio de constantes definidas nos arquivos. Para
cada processo dentro de limites predefinidos:

-   Tempo máximo de CPU: **20 unidades de tempo**.

-   Número máximo de I/O: **5 unidades de tempo**.

**Gerenciamento de processos**:

-   Filas de prioridade:

    -   Fila de alta prioridade: para processos novos e processos que
        retornam de I/O prioritário.

    -   Fila de baixa prioridade: para processos que sofreram preempção
        ou retornam de I/O menos prioritário.

-   Regras de retorno à fila:

    -   Disco: Retorna à fila de baixa prioridade.

    -   Fita magnética: Retorna à fila de alta prioridade.

    -   Impressora: Retorna à fila de alta prioridade.

# Metodologia

Nesta seção, detalharemos as estruturas utilizadas no código, o setup
inicial, a lógica do algoritmo *Round Robin* com *feedback* e a saída
gerada pelo programa.

## Estruturas (`structs`)

Para o funcionamento do simulador, utilizamos as seguintes estruturas
principais:

`IO`: Estrutura que detalha as operações de entrada/saída (*I/O*)
realizadas por um processo. Inclui:

-   `tempoIO`: Duração da operação de entrada/saída.

-   `tipoIO`: Tipo de dispositivo associado à operação, que pode ser:

    -   `DISCO`: Operação em um dispositivo de disco.

    -   `IMPRESSORA`: Operação em uma impressora.

    -   `FITA`: Operação em uma unidade de fita.

-   `tempoChegada`: Momento em que a operação de I/O é requisitada.

-   `tempoInicio`: Momento em que a operação de I/O é iniciada.

<!-- -->

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

`Node` e `Queue`: Utilizadas para implementar filas genéricas no
simulador.

-   A estrutura `Node` conecta os processos em uma lista encadeada.

-   A estrutura `Queue` gerencia os nós, mantendo referências ao início
    e ao final da fila.

<!-- -->

    typedef struct Node {
      Process process;
      struct Node *next;
    } Node;

    typedef struct {
      Node* head;
      Node* tail;
      int size;
    } Queue;

## Setup Inicial

O programa realiza o seguinte setup inicial:

1.  Inicialização das filas:

    -   `qNovosProcessos`: Armazena os processos que ainda não chegaram
        ao sistema.

    -   `qAltaPrioridade`: Contém processos com maior prioridade de
        execução.

    -   `qBaixaPrioridade`: Contém processos que sofreram preempção ou
        não concluíram no *quantum*.

    -   Filas de I/O: (`qIODisco`, `qIOFita`, `qIOImpressora`) mantêm
        processos aguardando operações de I/O específicas.

2.  Leitura do arquivo:

    -   Um arquivo de texto é lido para criar e preencher a fila
        `qNovosProcessos`.

    -   Cada linha do arquivo representa um processo com atributos como
        PID, tempo de chegada, tempo de serviço e lista de I/O.

3.  Exibição inicial:

    -   O programa imprime os processos lidos e suas respectivas
        características.

<figure id="fig:enter-label">
<img src="leitura-processos.png" style="width:50.0%" />
<figcaption>Entrada de processos feita por aquivo, a qual o programa
lista os processos lidos</figcaption>
</figure>

### Função `leProcessosArquivo`

A função `leProcessosArquivo` é responsável por ler os processos a
partir de um arquivo texto. Cada linha do arquivo contém informações
sobre um processo no formato:

    PID;TEMPO_CHEGADA;TEMPO_SERVIÇO;NUM_IO;TIPO_IO;CHEGADA_IO;...

Detalhamento da implementação:

-   Abre o arquivo e valida se ele foi carregado corretamente.

-   Lê cada linha e converte os valores para os campos do processo:

    -   `pid`: Identificador único do processo.

    -   `tempo_chegada`: Momento em que o processo chega ao sistema.

    -   `tempo_serviço`: Tempo necessário para completar o processo.

    -   `numero_io`: Número de operações de I/O associadas.

    -   `tempos_io, tipos_io (0 representa DISCO; 1 representa FITA MAGNÉTICA; 2 representa IMPRESSORA), chegada_io`:
        Detalhes das operações de I/O.

-   Associa os valores lidos a uma estrutura do tipo `Process` no vetor
    global `processos`.

<!-- -->

    Process* leProcessosArquivo(const char *nomeArquivo, int* numeroProcessos){
      // passamos a referencia de numeroProcessos para que possamos incrementar 
      // esta variavel que esta fora do escopo
      
      FILE *arquivo = fopen(nomeArquivo, "r");
      if(arquivo == NULL){
        printf("Erro ao abrir arquivo\n");
        exit(1);
      }

      char line[256];
      while(fgets(line, sizeof(line), arquivo)){
        if(*numeroProcessos>=MAX_PROCESS){
          printf("Numero maximo de processos atingido\n");
          break;
        }

        int pid, tempo_chegada, tempo_servico;
        int tipos_io[MAX_IO];
        int tempos_io[MAX_IO];
        int tempo_chegada_io[MAX_IO];

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
        processos[*numeroProcessos].pid = pid;
        processos[*numeroProcessos].tempoDeServico = tempo_servico;
        processos[*numeroProcessos].tempoChegada = tempo_chegada;
        processos[*numeroProcessos].qntIO = numero_io;

        processos[*numeroProcessos].io = (IO *) malloc(sizeof(IO) * numero_io);
        for(int i=0; i<numero_io; i++){
          processos[*numeroProcessos].io[i].tempoIO = tempos_io[i];
          processos[*numeroProcessos].io[i].tipoIO = tipos_io[i];
          processos[*numeroProcessos].io[i].tempoChegada = tempo_chegada_io[i];
        }


        //Fim da associacao

        *(numeroProcessos) = *numeroProcessos + 1;
      }

      fclose(arquivo);
      return processos;
    }

### Função `printarProcessos`

A função `printarProcessos` exibe no console os atributos de todos os
processos armazenados. Cada processo é descrito com suas características
principais, incluindo operações de I/O.

    void printarProcessos(int numeroProcessos){
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

# Estrutura do *Round Robin*

A seguir, explicamos o funcionamento do programa principal em `main.c`,
que implementa o algoritmo de escalonamento Round Robin com
gerenciamento de filas de entrada/saída (IO). Cada função tem um papel
específico na simulação, descrito abaixo:

## Inicialização de Filas e Estruturas

A função `alocaInicial` cria todas as filas necessárias para o
gerenciamento dos processos:

    void alocaInicial(){
      qNovosProcessos = alocaQueue();
      qAltaPrioridade = alocaQueue();
      qBaixaPrioridade = alocaQueue();
      qIODisco = alocaQueue();
      qIOFita = alocaQueue();
      qIOImpressora = alocaQueue();
    }

Esta função inicializa:

-   **qNovosProcessos**: fila para novos processos aguardando execução.

-   **qAltaPrioridade** e **qBaixaPrioridade**: filas para processos com
    diferentes prioridades.

-   **qIODisco**, **qIOFita**, **qIOImpressora**: filas para processos
    aguardando operações de IO.

## Leitura de Processos

A função `leProcessos` lê os dados dos processos a partir de um arquivo
especificado pelo usuário:

    void leProcessos() {
      printf("Digite o nome do arquivo a ser lido: ");
      char nomeArquivo[100];
      scanf("%s", nomeArquivo);
      Process *processos = leitura(nomeArquivo, &numProcessos);
      for (int i = 0; i < numProcessos; i++) {
        enqueue(qNovosProcessos, processos[i]);
      }
      sleep(1);
    }

Esta função:

-   Solicita o nome do arquivo de entrada.

-   Insere os processos lidos na fila **qNovosProcessos**.

## Alocação de Novos Processos

A função `alocaNovosProcessos` verifica se há novos processos para serem
inseridos nas filas de execução:

    void alocaNovosProcessos() {
      while (!isEmpty(qNovosProcessos) &&
             peek(qNovosProcessos)->tempoChegada <= time) {
        Process novoProcesso;
        dequeue(qNovosProcessos, &novoProcesso);
        enqueue(qAltaPrioridade, novoProcesso);
        printf("Novo processo com pid: %d\n", novoProcesso.pid);
      }
    }

Esta função:

-   Verifica o tempo de chegada dos processos.

-   Move os processos da fila **qNovosProcessos** para a fila
    **qAltaPrioridade**.

## Liberação de Processos de IO

As variaveis tempoFitaConcluido, tempoImpressoraConcluido e
tempoDiscoComcluido servem para armazenar o tempo em que a ultima
operacao de IO foi concluida nas respectivas filas. Ela serve para, caso
um processo chege na fila de IO e precise aguardar, a avaliacao do tempo
seja correta.

A função `liberaProcessosDoIO` verifica os processos concluídos em filas
de IO (Disco, Impressora e Fita):

    void liberaProcessosDoIO() {
      while (!isEmpty(qIOFita)) {
        Process *p = peek(qIOFita);
        IO io;

        for (int i = 0; i < p->qntIO; i++) {
          if (p->io[i].tipoIO == FITA) {
            io = p->io[i];
            break;
          }
        }
        tempoFitaConcluido = (tempoFitaConcluido > io.tempoInicio)
                                 ? tempoFitaConcluido
                                 : io.tempoInicio;

        if (tempoFitaConcluido + io.tempoIO <= time) {
          Process processoRetornado;
          dequeue(qIOFita, &processoRetornado);
          enqueue(qAltaPrioridade, processoRetornado);
          printf("Processo PID: %d saiu da fila de Fita em %du.t.\n",
                 processoRetornado.pid, tempoFitaConcluido + io.tempoIO);
        } else {
          break;
        }
      }

      while (!isEmpty(qIOImpressora)) {
        Process *p = peek(qIOImpressora);
        IO io;

        for (int i = 0; i < p->qntIO; i++) {
          if (p->io[i].tipoIO == IMPRESSORA) {
            io = p->io[i];
            break;
          }
        }
        tempoImpressoraConcluido = (tempoImpressoraConcluido > io.tempoInicio)
                                       ? tempoImpressoraConcluido
                                       : io.tempoInicio;

        if (tempoImpressoraConcluido + io.tempoIO <= time) {
          Process processoRetornado;
          dequeue(qIOImpressora, &processoRetornado);
          enqueue(qAltaPrioridade, processoRetornado);
          printf("Processo PID: %d saiu da fila de Impressao em %du.t.\n",
                 processoRetornado.pid, tempoImpressoraConcluido + io.tempoIO);
        } else {
          break;
        }
      }

      while (!isEmpty(qIODisco)) {
        Process *p = peek(qIODisco);
        IO io;

        for (int i = 0; i < p->qntIO; i++) {
          if (p->io[i].tipoIO == DISCO) {
            io = p->io[i];
            break;
          }
        }
        tempoDiscoConcluido = (tempoDiscoConcluido > io.tempoInicio)
                                  ? tempoDiscoConcluido
                                  : io.tempoInicio;

        if (tempoDiscoConcluido + io.tempoIO <= time) {
          Process processoRetornado;
          dequeue(qIODisco, &processoRetornado);
          enqueue(qBaixaPrioridade, processoRetornado);
          printf("Processo PID: %d saiu da fila de Disco em %du.t.\n",
                 processoRetornado.pid, tempoDiscoConcluido + io.tempoIO);
        } else {
          break;
        }
      }
    }

Esta função:

-   Percorre cada fila de IO (**qIOFita**, **qIODisco**,
    **qIOImpressora**).

-   Libera os processos que concluíram suas operações de IO e os reenvia
    para a fila **qAltaPrioridade**.

## (*Main*)

### Explicação Detalhada da Função `main()`

A função `main()` é o coração do programa que implementa o algoritmo de
escalonamento *Round Robin* com suporte a filas de prioridade e
operações de E/S (*I/O*). A seguir, cada bloco do código será detalhado:

1.  **Inicialização das Estruturas e Variáveis:**

            alocaInicial();
            leProcessos();

            int numProcessosRestantes = 0;

            int pidProcessoFinalizados[numProcessos];
            int tempoProcessoFinalizados[numProcessos];

    \- A função `alocaInicial()` inicializa as filas do sistema: -
    `qAltaPrioridade` e `qBaixaPrioridade`: filas para processos de alta
    e baixa prioridade. - `qIODisco`, `qIOFita` e `qIOImpressora`: filas
    para operações de E/S (disco, fita e impressora). - A função
    `leProcessos()` lê os processos e suas informações (tempo de
    chegada, tempo de serviço, E/S, etc.). - Variáveis auxiliares: -
    `numProcessosRestantes`: rastreia quantos processos ainda precisam
    ser executados. - `pidProcessoFinalizados` e
    `tempoProcessoFinalizados`: armazenam o PID e o tempo de término de
    cada processo.

2.  **Início da Simulação:**

            puts("=-=-=-=-=-=-=-=-=-=-=-=-=INICIO=-=-=-=-=-=-=-=-=-=-=-=-=");
            while (processosTerminados < numProcessos) {

    \- Exibe uma mensagem inicial para indicar o começo da simulação. -
    Entra no *loop* principal, que continuará até que todos os processos
    (`numProcessos`) sejam concluídos. A variável `processosTerminados`
    é usada como critério de parada.

3.  **Tratamento de Novos Processos:**

            alocaNovosProcessos();

    \- Esta função verifica se algum processo tem seu `tempoDeChegada`
    igual ao tempo atual (`time`). Caso positivo, o processo é movido da
    lista de novos processos para a fila de alta prioridade
    (`qAltaPrioridade`).

4.  **Liberação de Processos de E/S:**

            liberaProcessosDoIO();

    \- Esta função verifica as filas de E/S (disco, fita e impressora)
    para determinar se algum processo terminou sua operação de E/S.
    Processos liberados são movidos para a fila de alta prioridade ou
    baixo
    prioridade($\texttt{qAltaPrioridade} \lor \texttt{qBaixaPrioridade}$).

5.  **Exibição do Estado Atual:**

            printf("Time: %d\n", time);
            // tratar novos processos
            alocaNovosProcessos();

            // tratar processos vindos da fila de IO
            liberaProcessosDoIO();

            printf("PIDs na fila de alta: ");
            printQueue(qAltaPrioridade);

            printf("PIDs na fila de baixa: ");
            printQueue(qBaixaPrioridade);

        ...


            if (!isEmpty(qIODisco)) {
              printf("PIDs na fila de IO do Disco: ");
              printQueue(qIODisco);
            }

            if (!isEmpty(qIOFita)) {
              printf("PIDs na fila de IO da Fita: ");
              printQueue(qIOFita);
            }

            if (!isEmpty(qIOImpressora)) {
              printf("PIDs na fila de IO da Impressora: ");
              printQueue(qIOImpressora);
            }

    \- Mostra o estado das filas de processos e E/S no tempo atual.
    Filas vazias não são exibidas.

6.  **Verificação de Processos Restantes:**

            numProcessosRestantes = qAltaPrioridade->size + 
                                    qBaixaPrioridade->size;

            if (numProcessosRestantes == 0) {
                puts("Sem processos a executar\n");
                time++;
                continue;
            }

    \- Calcula o total de processos ainda pendentes, somando os tamanhos
    das filas de prioridade. - Se não houver processos pendentes, o
    tempo avança diretamente e o *loop* continua.

7.  **Seleção de Processo para Execução:**

            if (qAltaPrioridade->size > 0) {
                dequeue(qAltaPrioridade, &processoParaExecutar);
            } else {
                dequeue(qBaixaPrioridade, &processoParaExecutar);
            }

    \- O processo com a maior prioridade (primeiro na fila de alta
    prioridade) é selecionado para execução. Caso a fila de alta
    prioridade esteja vazia, um processo da fila de baixa prioridade é
    escolhido.

8.  **Gerenciamento de Operações de E/S:**

            for (int i = 0; i < processoParaExecutar.qntIO; i++) {
                if (processoParaExecutar.io[i].tempoChegada != -1) {
                    instanteProximoIO = processoParaExecutar.io[i].tempoChegada;
                    processoParaExecutar.io[i].tempoChegada = -1;
                    indiceProximoIO = i;
                    break;
                }
            }

    Verifica se o processo precisa realizar alguma operação de E/S. Caso
    positivo: - Obtém o instante da próxima operação e o índice
    correspondente na lista de operações.  
    - Marca a operação como pendente, definindo `tempoChegada = -1`.  
    - Caso a operação de E/S ocorra durante o *quantum*, o processo é
    movido para a fila de E/S correspondente:

            switch (proximoIO.tipoIO) {
                case DISCO: enqueue(qIODisco, processoParaExecutar); break;
                case IMPRESSORA: enqueue(qIOImpressora, processoParaExecutar); 
                break;
                case FITA: enqueue(qIOFita, processoParaExecutar); break;
            }

9.  **Execução ou Preempção do Processo:**

            if (processoParaExecutar.tempoDeServico <= QUANTUM) {
                time += processoParaExecutar.tempoDeServico;
                processosTerminados++;
            } else {
                processoParaExecutar.tempoDeServico -= QUANTUM;
                enqueue(qBaixaPrioridade, processoParaExecutar);
                time += QUANTUM;
            }

    \- Se o processo puder ser concluído dentro do *quantum*, ele é
    finalizado. Caso contrário, sofre preempção: - O tempo restante é
    atualizado. - O processo é movido para a fila de baixa prioridade.

## Saída (*Output*)

A saída do programa é gerada ao longo da execução, exibindo o estado das
filas e a execução dos processos. Ao final, é apresentado um resumo:

-   \*\*Tempo de execução de cada processo.\*\*

-   \*\*Fila de processos concluídos.\*\*

<!-- -->

    puts("=-=-=-=-=-=-=-=-=-=-=-=-=-FIM-=-=-=-=-=-=-=-=-=-=-=-=-=");
    printf("Tempo Final: %d\n", time);

    for (int i = 0; i < numProcessos; i++) {
        printf("P%d foi finalizado no tempo %d u.t.\n", pidProcessoFinalizados[i], tempoProcessoFinalizados[i]);
    }

# Resultados

Nesta seção, apresentamos os resultados obtidos ao executar o
escalonador com entradas específicas de processos e operações de E/S.
Cada exemplo demonstra a funcionalidade do algoritmo *Round Robin* e o
gerenciamento das filas de alta e baixa prioridade, bem como das filas
de E/S.  
Para compilar o código, basta usar o comando:

    gcc main.c read.c queue.c -o main
    ./main

## Exemplo 1

**Entrada:** Arquivo `teste1relatorio.txt` com o seguinte conteúdo:

    1;0;12;0;
    2;4;11;2;1;4;2;6;
    3;5;7;0;
    4;7;8;0;
    5;10;16;2;2;2;1;6;

Cada linha corresponde às informações de um processo no formato:  
`PID;TempoDeChegada;TempoDeServico;NumeroDeIOs;(IOs)`. Para os processos
com operações de E/S, os valores adicionais representam o tipo
(`1=FITA`, `2=IMPRESSORA`, `0=DISCO`) e o instante de chegada de cada
E/S.

**Saída:** Execução do programa com o arquivo de entrada fornecido:

      ./main
    Digite o nome do arquivo a ser lido: teste1relatorio.txt
    Processo 1
    Tempo de chegada: 0
    Tempo de servico: 12
    Numero de IOs: 0

    Processo 2
    Tempo de chegada: 4
    Tempo de servico: 11
    Numero de IOs: 2
    IO 1
    FITA
    Tempo de IO: 10
    Tempo de chegada: 4
    IO 2
    IMPRESSORA
    Tempo de IO: 20
    Tempo de chegada: 6

    Processo 3
    Tempo de chegada: 5
    Tempo de servico: 7
    Numero de IOs: 0

    Processo 4
    Tempo de chegada: 7
    Tempo de servico: 8
    Numero de IOs: 0

    Processo 5
    Tempo de chegada: 10
    Tempo de servico: 16
    Numero de IOs: 2
    IO 1
    IMPRESSORA
    Tempo de IO: 20
    Tempo de chegada: 2
    IO 2
    FITA
    Tempo de IO: 10
    Tempo de chegada: 6

    =-=-=-=-=-=-=-=-=-=-=-=-=INICIO=-=-=-=-=-=-=-=-=-=-=-=-=
    Time: 0
    Novo processo com pid: 1
    PIDs na fila de alta: 1 
    PIDs na fila de baixa: 
    Peguei o pid 1 da fila de alta
    Processo PID: 1 executou por 5u.t.

    Time: 5
    Novo processo com pid: 2
    Novo processo com pid: 3
    PIDs na fila de alta: 2 3 
    PIDs na fila de baixa: 1 
    Peguei o pid 2 da fila de alta
    Processo PID: 2 executou por 4u.t.
    Processo PID: 2 pediu operacao de IO Fita em 9u.t.
    PIDs na fila de IO da Fita: 2 

    Time: 9
    Novo processo com pid: 4
    PIDs na fila de alta: 3 4 
    PIDs na fila de baixa: 1 
    Peguei o pid 3 da fila de alta
    Processo PID: 3 executou por 5u.t.
    PIDs na fila de IO da Fita: 2 

    Time: 14
    Novo processo com pid: 5
    PIDs na fila de alta: 4 5 
    PIDs na fila de baixa: 1 3 
    Peguei o pid 4 da fila de alta
    Processo PID: 4 executou por 5u.t.
    PIDs na fila de IO da Fita: 2 

    Time: 19
    Processo PID: 2 saiu da fila de Fita em 19u.t.
    PIDs na fila de alta: 5 2 
    PIDs na fila de baixa: 1 3 4 
    Peguei o pid 5 da fila de alta
    Processo PID: 5 executou por 2u.t.
    Processo PID: 5 pediu operacao de IO Impressora em 21u.t.
    PIDs na fila de IO da Impressora: 5 

    Time: 21
    PIDs na fila de alta: 2 
    PIDs na fila de baixa: 1 3 4 
    Peguei o pid 2 da fila de alta
    Processo PID: 2 executou por 6u.t.
    Processo PID: 2 pediu operacao de IO Impressora em 27u.t.
    PIDs na fila de IO da Impressora: 5 2 

    Time: 27
    PIDs na fila de alta: 
    PIDs na fila de baixa: 1 3 4 
    Peguei o pid 1 da fila de baixa
    Processo PID: 1 executou por 5u.t.
    PIDs na fila de IO da Impressora: 5 2 

    Time: 32
    PIDs na fila de alta: 
    PIDs na fila de baixa: 3 4 1 
    Peguei o pid 3 da fila de baixa
    Processo PID: 3 executou por 2u.t.
    Processo PID: 3 Terminou em 34u.t.
    PIDs na fila de IO da Impressora: 5 2 

    Time: 34
    PIDs na fila de alta: 
    PIDs na fila de baixa: 4 1 
    Peguei o pid 4 da fila de baixa
    Processo PID: 4 executou por 3u.t.
    Processo PID: 4 Terminou em 37u.t.
    PIDs na fila de IO da Impressora: 5 2 

    Time: 37
    PIDs na fila de alta: 
    PIDs na fila de baixa: 1 
    Peguei o pid 1 da fila de baixa
    Processo PID: 1 executou por 2u.t.
    Processo PID: 1 Terminou em 39u.t.
    PIDs na fila de IO da Impressora: 5 2 

    Time: 39
    PIDs na fila de alta: 
    PIDs na fila de baixa: 
    Sem processos a executar

    Time: 40
    PIDs na fila de alta: 
    PIDs na fila de baixa: 
    Sem processos a executar

    Time: 41
    Processo PID: 5 saiu da fila de Impressao em 41u.t.
    PIDs na fila de alta: 5 
    PIDs na fila de baixa: 
    Peguei o pid 5 da fila de alta
    Processo PID: 5 executou por 6u.t.
    Processo PID: 5 pediu operacao de IO Fita em 47u.t.
    PIDs na fila de IO da Fita: 5 
    PIDs na fila de IO da Impressora: 2 

    Time: 47
    Processo PID: 2 saiu da fila de Impressao em 47u.t.
    PIDs na fila de alta: 2 
    PIDs na fila de baixa: 
    Peguei o pid 2 da fila de alta
    Processo PID: 2 executou por 1u.t.
    Processo PID: 2 Terminou em 48u.t.
    PIDs na fila de IO da Fita: 5 

    Time: 48
    PIDs na fila de alta: 
    PIDs na fila de baixa: 
    Sem processos a executar

    Time: 49
    PIDs na fila de alta: 
    PIDs na fila de baixa: 
    Sem processos a executar

    Time: 50
    PIDs na fila de alta: 
    PIDs na fila de baixa: 
    Sem processos a executar

    Time: 51
    PIDs na fila de alta: 
    PIDs na fila de baixa: 
    Sem processos a executar

    Time: 52
    PIDs na fila de alta: 
    PIDs na fila de baixa: 
    Sem processos a executar

    Time: 53
    PIDs na fila de alta: 
    PIDs na fila de baixa: 
    Sem processos a executar

    Time: 54
    PIDs na fila de alta: 
    PIDs na fila de baixa: 
    Sem processos a executar

    Time: 55
    PIDs na fila de alta: 
    PIDs na fila de baixa: 
    Sem processos a executar

    Time: 56
    PIDs na fila de alta: 
    PIDs na fila de baixa: 
    Sem processos a executar

    Time: 57
    Processo PID: 5 saiu da fila de Fita em 57u.t.
    PIDs na fila de alta: 5 
    PIDs na fila de baixa: 
    Peguei o pid 5 da fila de alta
    Processo PID: 5 executou por 5u.t.

    Time: 62
    PIDs na fila de alta: 
    PIDs na fila de baixa: 5 
    Peguei o pid 5 da fila de baixa
    Processo PID: 5 executou por 3u.t.
    Processo PID: 5 Terminou em 65u.t.

    =-=-=-=-=-=-=-=-=-=-=-=-=-FIM-=-=-=-=-=-=-=-=-=-=-=-=-=
    Tempo Final: 65
    =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    P3 foi finalizado no tempo 34 u.t.
    =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    P4 foi finalizado no tempo 37 u.t.
    =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    P1 foi finalizado no tempo 39 u.t.
    =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    P2 foi finalizado no tempo 48 u.t.
    =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    P5 foi finalizado no tempo 65 u.t.
    =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

## Exemplo 2

**Entrada:** Arquivo `teste2relatorio.txt` com o seguinte conteúdo:

    1;0;31;2;2;1;0;30;
    2;2;7;0;

Cada linha corresponde às informações de um processo no formato:
`PID;TempoDeChegada;TempoDeServico;NumeroDeIOs;(IOs)`. Para os processos
com operações de E/S, os valores adicionais representam o tipo
(`1=FITA`, `2=IMPRESSORA`, `0=DISCO`) e o instante de chegada de cada
E/S.

**Saída:** Execução do programa com o arquivo de entrada fornecido:

       ./main 
    Digite o nome do arquivo a ser lido: teste2relatorio.txt
    Processo 1
    Tempo de chegada: 0
    Tempo de servico: 31
    Numero de IOs: 2
    IO 1
    IMPRESSORA
    Tempo de IO: 20
    Tempo de chegada: 1

    Processo 2
    Tempo de chegada: 2
    Tempo de servico: 7
    Numero de IOs: 0

    =-=-=-=-=-=-=-=-=-=-=-=-=INICIO=-=-=-=-=-=-=-=-=-=-=-=-=
    Time: 0
    Novo processo com pid: 1
    PIDs na fila de alta: 1 
    PIDs na fila de baixa: 
    Peguei o pid 1 da fila de alta
    Processo PID: 1 executou por 1u.t.
    Processo PID: 1 pediu operacao de IO Impressora em 1u.t.
    PIDs na fila de IO da Impressora: 1 

    Time: 1
    PIDs na fila de alta: 
    PIDs na fila de baixa: 
    Sem processos a executar

    Time: 2
    Novo processo com pid: 2
    PIDs na fila de alta: 2 
    PIDs na fila de baixa: 
    Peguei o pid 2 da fila de alta
    Processo PID: 2 executou por 5u.t.
    PIDs na fila de IO da Impressora: 1 

    Time: 7
    PIDs na fila de alta: 
    PIDs na fila de baixa: 2 
    Peguei o pid 2 da fila de baixa
    Processo PID: 2 executou por 2u.t.
    Processo PID: 2 Terminou em 9u.t.
    PIDs na fila de IO da Impressora: 1 

    Time: 9
    PIDs na fila de alta: 
    PIDs na fila de baixa: 
    Sem processos a executar

    Time: 10
    PIDs na fila de alta: 
    PIDs na fila de baixa: 
    Sem processos a executar

    Time: 11
    PIDs na fila de alta: 
    PIDs na fila de baixa: 
    Sem processos a executar

    Time: 12
    PIDs na fila de alta: 
    PIDs na fila de baixa: 
    Sem processos a executar

    Time: 13
    PIDs na fila de alta: 
    PIDs na fila de baixa: 
    Sem processos a executar

    Time: 14
    PIDs na fila de alta: 
    PIDs na fila de baixa: 
    Sem processos a executar

    Time: 15
    PIDs na fila de alta: 
    PIDs na fila de baixa: 
    Sem processos a executar

    Time: 16
    PIDs na fila de alta: 
    PIDs na fila de baixa: 
    Sem processos a executar

    Time: 17
    PIDs na fila de alta: 
    PIDs na fila de baixa: 
    Sem processos a executar

    Time: 18
    PIDs na fila de alta: 
    PIDs na fila de baixa: 
    Sem processos a executar

    Time: 19
    PIDs na fila de alta: 
    PIDs na fila de baixa: 
    Sem processos a executar

    Time: 20
    PIDs na fila de alta: 
    PIDs na fila de baixa: 
    Sem processos a executar

    Time: 21
    Processo PID: 1 saiu da fila de Impressao em 21u.t.
    PIDs na fila de alta: 1 
    PIDs na fila de baixa: 
    Peguei o pid 1 da fila de alta
    Processo PID: 1 executou por 5u.t.

    Time: 26
    PIDs na fila de alta: 
    PIDs na fila de baixa: 1 
    Peguei o pid 1 da fila de baixa
    Processo PID: 1 executou por 5u.t.

    Time: 31
    PIDs na fila de alta: 
    PIDs na fila de baixa: 1 
    Peguei o pid 1 da fila de baixa
    Processo PID: 1 executou por 5u.t.

    Time: 36
    PIDs na fila de alta: 
    PIDs na fila de baixa: 1 
    Peguei o pid 1 da fila de baixa
    Processo PID: 1 executou por 5u.t.

    Time: 41
    PIDs na fila de alta: 
    PIDs na fila de baixa: 1 
    Peguei o pid 1 da fila de baixa
    Processo PID: 1 executou por 5u.t.

    Time: 46
    PIDs na fila de alta: 
    PIDs na fila de baixa: 1 
    Peguei o pid 1 da fila de baixa
    Processo PID: 1 executou por 5u.t.
    Processo PID: 1 Terminou em 51u.t.

    =-=-=-=-=-=-=-=-=-=-=-=-=-FIM-=-=-=-=-=-=-=-=-=-=-=-=-=
    Tempo Final: 51
    =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    P2 foi finalizado no tempo 9 u.t.
    =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
    P1 foi finalizado no tempo 51 u.t.
    =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

O grande tempo para o qual os processos demoraram a ser finalizados se
deram por conta das chamadas de IO de P1, como pode ser visto na sua
entrada, o mesmo faz duas com tempos que desfavorecem a velocidade do
escalonador.

# Conclusão

O desenvolvimento deste trabalho permitiu aprofundar o conhecimento
sobre os conceitos fundamentais de escalonamento de processos, uma das
principais responsabilidades do sistema operacional. A implementação do
simulador utilizando o algoritmo Round Robin com Feedback, em linguagem
C, proporcionou uma compreensão prática e detalhada do funcionamento
desse método, destacando a importância da organização e priorização na
execução de múltiplos processos.

Através da metodologia adotada, que incluiu a definição clara de
premissas, a construção de um modelo detalhado de gerenciamento de
processos e a aplicação de filas de prioridades, foi possível observar o
comportamento de diferentes tipos de processos e dispositivos de entrada
e saída. Além disso, os resultados obtidos demonstraram a eficiência do
algoritmo em gerenciar o uso do processador de forma equitativa, ao
mesmo tempo que respeita os tempos de resposta e os requisitos de I/O.

Durante a execução do trabalho, enfrentamos desafios como a correta
implementação das regras de retorno de processos às filas e o
gerenciamento de tempos em um ambiente simulado. Contudo, esses
obstáculos foram superados por meio do trabalho em equipe e de uma
abordagem iterativa de desenvolvimento.

Por fim, este trabalho reforçou a relevância do estudo da arquitetura de
computadores e sistemas operacionais, não apenas no aspecto teórico, mas
também em sua aplicação prática. A experiência adquirida contribuirá
significativamente para futuras implementações e para a formação
acadêmica e profissional dos integrantes da equipe.

# Referências [referências]

1.  STALLINGS, William. Operating systems: internals and design
    principles. 7ª ed. Upper Saddle River: Pearson Prentice Hall, 2010.

2.  ROUND ROBIN, uma técnica preemptiva de escalonamento. Acessado em 22
    de maio de 2022. Disponível em:  
    <https://deinfo.uepg.br/~alunoso/2016/ROUNDROBIN/#:~:text=%C3%89%20o%20tipo%20de%20escalonamento,um%20n%C3%BAmero%20inteiro%20de%20quanta>.

# Links Importantes [links-importantes]

-   [Código em C](https://github.com/GbAraujoSouza/escalonador)
