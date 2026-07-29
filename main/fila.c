/**
 * @file fila.c
 * @brief Implementação das filas de prioridade da estação.
 */

#include "fila.h"
#include "config.h"

#include "freertos/queue.h"
#include "freertos/semphr.h"

/* Filas para cada nível de prioridade. */
static QueueHandle_t filaEmergencia = NULL;
static QueueHandle_t filaBateriaCritica = NULL;
static QueueHandle_t filaNormal = NULL;

/* Contabiliza a quantidade total de veículos nas filas. */
static SemaphoreHandle_t veiculosDisponiveis = NULL;

/**
 * @brief Retorna a fila correspondente à prioridade informada.
 *
 * @param prioridade Prioridade do veículo.
 * @return Fila correspondente.
 */
static QueueHandle_t selecionarFila(
    PrioridadeVeiculo prioridade
)
{
    switch (prioridade)
    {
        case PRIORIDADE_EMERGENCIA:
            return filaEmergencia;

        case PRIORIDADE_BATERIA_CRITICA:
            return filaBateriaCritica;

        case PRIORIDADE_NORMAL:
        default:
            return filaNormal;
    }
}

/**
 * @brief Inicializa as filas de prioridade e o semáforo de veículos.
 *
 * @return pdPASS em caso de sucesso.
 * @return pdFAIL em caso de falha.
 */
BaseType_t inicializarFilas(void)
{
    filaEmergencia = xQueueCreate(
        TAMANHO_FILA,
        sizeof(Veiculo)
    );

    filaBateriaCritica = xQueueCreate(
        TAMANHO_FILA,
        sizeof(Veiculo)
    );

    filaNormal = xQueueCreate(
        TAMANHO_FILA,
        sizeof(Veiculo)
    );

    veiculosDisponiveis = xSemaphoreCreateCounting(
        TAMANHO_FILA * 3,
        0
    );

    if (
        filaEmergencia == NULL ||
        filaBateriaCritica == NULL ||
        filaNormal == NULL ||
        veiculosDisponiveis == NULL
    )
    {
        return pdFAIL;
    }

    return pdPASS;
}

/**
 * @brief Insere um veículo na fila correspondente à sua prioridade.
 *
 * @param veiculo Ponteiro para o veículo.
 * @return pdPASS em caso de sucesso.
 * @return pdFAIL em caso de falha.
 */
BaseType_t inserirVeiculoNaFila(
    const Veiculo *veiculo
)
{
    if (veiculo == NULL)
    {
        return pdFAIL;
    }

    QueueHandle_t filaSelecionada =
        selecionarFila(veiculo->prioridade);

    if (filaSelecionada == NULL)
    {
        return pdFAIL;
    }

    if (xQueueSend(
            filaSelecionada,
            veiculo,
            pdMS_TO_TICKS(1000)
        ) != pdPASS)
    {
        return pdFAIL;
    }

    /* Indica que há mais um veículo disponível para atendimento. */
    xSemaphoreGive(veiculosDisponiveis);

    return pdPASS;
}

/**
 * @brief Aguarda até existir um veículo disponível.
 *
 * @return pdTRUE quando houver um veículo disponível.
 * @return pdFAIL se o semáforo não estiver inicializado.
 */
BaseType_t aguardarVeiculoDisponivel(void)
{
    if (veiculosDisponiveis == NULL)
    {
        return pdFAIL;
    }

    return xSemaphoreTake(
        veiculosDisponiveis,
        portMAX_DELAY
    );
}

/**
 * @brief Remove o veículo de maior prioridade disponível.
 *
 * A ordem de atendimento é:
 * emergência → crítico → normal.
 *
 * @param veiculo Ponteiro para armazenar o veículo removido.
 * @return pdPASS em caso de sucesso.
 * @return pdFAIL caso todas as filas estejam vazias.
 */
BaseType_t removerVeiculoPrioritario(
    Veiculo *veiculo
)
{
    if (veiculo == NULL)
    {
        return pdFAIL;
    }

    if (xQueueReceive(
            filaEmergencia,
            veiculo,
            0
        ) == pdPASS)
    {
        return pdPASS;
    }

    if (xQueueReceive(
            filaBateriaCritica,
            veiculo,
            0
        ) == pdPASS)
    {
        return pdPASS;
    }

    if (xQueueReceive(
            filaNormal,
            veiculo,
            0
        ) == pdPASS)
    {
        return pdPASS;
    }

    return pdFAIL;
}