#ifndef ESTACAO_H
#define ESTACAO_H

/**
 * @file estacao.h
 * @brief Interface do módulo de gerenciamento da estação.
 */

#include "freertos/FreeRTOS.h"

/**
 * @brief Inicializa os recursos da estação.
 *
 * Cria as tarefas e estruturas responsáveis pelo gerenciamento
 * das filas de veículos e pelo despacho aos carregadores.
 *
 * @return pdPASS em caso de sucesso.
 * @return pdFAIL caso ocorra alguma falha na inicialização.
 */
BaseType_t inicializarEstacao(void);

#endif /* ESTACAO_H */