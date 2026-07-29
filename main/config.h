#ifndef CONFIG_H
#define CONFIG_H

/**
 * @file config.h
 * @brief Configurações gerais da aplicação.
 */

/* Tamanho das filas de prioridade */
#define TAMANHO_FILA 10

/* Veículos com bateria menor ou igual a este valor são considerados críticos */
#define LIMITE_BATERIA_CRITICA 30


/* Quantidade de carregadores da estação */
#define NUM_CARREGADORES 3

/*
 * Tempo necessário para carregar 1% da bateria.
 *
 * Tempo de carga = (100 - bateria) × TEMPO_POR_PERCENTUAL_MS
 */
#define TEMPO_POR_PERCENTUAL_MS 100

/* Prioridades das tarefas */
#define PRIORIDADE_TAREFA_CHEGADA      2
#define PRIORIDADE_TAREFA_ESTACAO      3
#define PRIORIDADE_TAREFA_CARREGADOR   2

/* Tamanho das pilhas das tarefas (bytes) */
#define PILHA_TAREFA_CHEGADA      3072
#define PILHA_TAREFA_ESTACAO      3072
#define PILHA_TAREFA_CARREGADOR   3072

/* GPIOs dos LEDs */
#define GPIO_LED_CARREGADOR_1 25
#define GPIO_LED_CARREGADOR_2 26
#define GPIO_LED_CARREGADOR_3 27

/* GPIOs dos botões */
#define GPIO_BOTAO_NORMAL      18
#define GPIO_BOTAO_CRITICO     19
#define GPIO_BOTAO_EMERGENCIA  21

/* Configuração da fila de eventos dos botões */
#define TAMANHO_FILA_BOTOES 10

/* Tempo de debounce dos botões (ms) */
#define TEMPO_DEBOUNCE_MS 200

#endif /* CONFIG_H */