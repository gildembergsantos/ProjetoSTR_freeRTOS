#ifndef CONFIG_H
#define CONFIG_H

/* Filas de prioridade */
#define TAMANHO_FILA 10

/* Classificação dos veículos */
#define LIMITE_BATERIA_CRITICA 30
#define CHANCE_EMERGENCIA 20

/* Estação */
#define NUM_CARREGADORES 3

/*
 * Um veículo chega a cada segundo.
 * Como o carregamento dura cinco segundos,
 * haverá acúmulo de veículos nas filas.
 */
#define INTERVALO_CHEGADA_MS 1000
#define TEMPO_CARREGAMENTO_MS 5000

/* Prioridades das tarefas FreeRTOS */
#define PRIORIDADE_TAREFA_CHEGADA      2
#define PRIORIDADE_TAREFA_ESTACAO      3
#define PRIORIDADE_TAREFA_CARREGADOR   2

/* Pilhas das tarefas */
#define PILHA_TAREFA_CHEGADA      3072
#define PILHA_TAREFA_ESTACAO      3072
#define PILHA_TAREFA_CARREGADOR   3072

#endif