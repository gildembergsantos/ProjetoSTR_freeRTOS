#include "fila.h"
#include "config.h"

#include "freertos/queue.h"
#include "freertos/semphr.h"

static QueueHandle_t filaEmergencia = NULL;
static QueueHandle_t filaBateriaCritica = NULL;
static QueueHandle_t filaNormal = NULL;

/*
 * Conta o total de veículos existentes nas três filas.
 */
static SemaphoreHandle_t veiculosDisponiveis = NULL;

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

    /*
     * O contador só é incrementado após
     * a inserção efetiva do veículo.
     */
    xSemaphoreGive(veiculosDisponiveis);

    return pdPASS;
}

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