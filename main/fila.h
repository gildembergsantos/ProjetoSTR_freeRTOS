#ifndef FILA_H
#define FILA_H

/**
 * @file fila.h
 * @brief Interface do gerenciamento das filas de prioridade.
 */

#include "freertos/FreeRTOS.h"

#include "veiculo.h"

/**
 * @brief Inicializa as filas e os recursos de sincronização.
 *
 * @return pdPASS em caso de sucesso.
 * @return pdFAIL em caso de falha.
 */
BaseType_t inicializarFilas(void);

/**
 * @brief Insere um veículo na fila correspondente à sua prioridade.
 *
 * @param veiculo Ponteiro para o veículo.
 * @return pdPASS em caso de sucesso.
 * @return pdFAIL em caso de falha.
 */
BaseType_t inserirVeiculoNaFila(
    const Veiculo *veiculo
);

/**
 * @brief Aguarda até existir um veículo disponível nas filas.
 *
 * @return pdTRUE quando houver um veículo disponível.
 */
BaseType_t aguardarVeiculoDisponivel(void);

/**
 * @brief Remove o veículo de maior prioridade disponível.
 *
 * A ordem de atendimento é:
 * emergência → crítico → normal.
 *
 * @param veiculo Ponteiro para armazenar o veículo removido.
 * @return pdPASS em caso de sucesso.
 * @return pdFAIL caso nenhuma fila possua veículos.
 */
BaseType_t removerVeiculoPrioritario(
    Veiculo *veiculo
);

#endif /* FILA_H */