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
 * Tempo simulado necessário para carregar 1% da bateria.
 *
 * Exemplo:
 * bateria inicial = 40%
 * carga restante = 60%
 * tempo = 60 × 100 ms = 6000 ms
 */
#define TEMPO_POR_PERCENTUAL_MS 100

/* Prioridades das tarefas FreeRTOS */
#define PRIORIDADE_TAREFA_CHEGADA      2
#define PRIORIDADE_TAREFA_ESTACAO      3
#define PRIORIDADE_TAREFA_CARREGADOR   2

/* Pilhas das tarefas */
#define PILHA_TAREFA_CHEGADA      3072
#define PILHA_TAREFA_ESTACAO      3072
#define PILHA_TAREFA_CARREGADOR   3072

/* LEDs dos carregadores */
#define GPIO_LED_CARREGADOR_1 25
#define GPIO_LED_CARREGADOR_2 26
#define GPIO_LED_CARREGADOR_3 27

/* Botões */
#define GPIO_BOTAO_NORMAL       18
#define GPIO_BOTAO_CRITICO      19
#define GPIO_BOTAO_EMERGENCIA   21

#define TAMANHO_FILA_BOTOES 10
#define TEMPO_DEBOUNCE_MS 200

#endif