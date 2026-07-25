#include <stdio.h>

#include "carregador.h"
#include "config.h"
#include "veiculo.h"
#include "hardware.h"

#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

/*
 * Fila compartilhada pelas três tarefas de carregadores.
 *
 * A estação coloca veículos nessa fila e qualquer
 * carregador livre pode retirar o próximo.
 */
static QueueHandle_t filaAtendimento = NULL;

/*
 * Representa a quantidade de carregadores livres.
 *
 * Começa com NUM_CARREGADORES e diminui sempre
 * que um atendimento é iniciado.
 */
static SemaphoreHandle_t vagasCarregadores = NULL;

/*
 * IDs passados como parâmetros para as tarefas.
 * Devem permanecer válidos durante toda a execução.
 */
static int idsCarregadores[NUM_CARREGADORES];

static void tarefaCarregador(void *parametro)
{
    int idCarregador = *((int *)parametro);

    Veiculo veiculo;

    while (1)
    {
        /*
         * A tarefa permanece bloqueada até receber
         * um veículo encaminhado pela estação.
         */
        if (xQueueReceive(
                filaAtendimento,
                &veiculo,
                portMAX_DELAY
            ) == pdPASS)
        {
            ligarLedCarregador(idCarregador);

            printf(
                "[CARREGADOR %d] Veiculo %d iniciado | "
                "Bateria: %d%% | Tempo: %.1f s | Prioridade: %s\n",
                idCarregador,
                veiculo.id,
                veiculo.bateria,
                veiculo.tempoCarregamentoMs / 1000.0,
                textoPrioridade(veiculo.prioridade)
            );

            vTaskDelay(
                pdMS_TO_TICKS(
                    veiculo.tempoCarregamentoMs
                )
            );

            printf(
                "[CARREGADOR %d] Veiculo %d concluido "
                "apos %.1f s.\n",
                idCarregador,
                veiculo.id,
                veiculo.tempoCarregamentoMs / 1000.0
            );

            desligarLedCarregador(idCarregador);

            /*
             * Devolve uma unidade ao semáforo contador,
             * sinalizando que esse carregador está livre.
             */
            xSemaphoreGive(vagasCarregadores);
        }
    }
}

BaseType_t inicializarCarregadores(void)
{
    filaAtendimento = xQueueCreate(
        NUM_CARREGADORES,
        sizeof(Veiculo)
    );

    vagasCarregadores = xSemaphoreCreateCounting(
        NUM_CARREGADORES,
        NUM_CARREGADORES
    );

    if (
        filaAtendimento == NULL ||
        vagasCarregadores == NULL
    )
    {
        return pdFAIL;
    }

    for (int i = 0; i < NUM_CARREGADORES; i++)
    {
        idsCarregadores[i] = i + 1;

        BaseType_t resultado = xTaskCreate(
            tarefaCarregador,
            "tarefa_carregador",
            PILHA_TAREFA_CARREGADOR,
            &idsCarregadores[i],
            PRIORIDADE_TAREFA_CARREGADOR,
            NULL
        );

        if (resultado != pdPASS)
        {
            return pdFAIL;
        }
    }

    return pdPASS;
}

BaseType_t aguardarVagaCarregador(void)
{
    if (vagasCarregadores == NULL)
    {
        return pdFAIL;
    }

    return xSemaphoreTake(
        vagasCarregadores,
        portMAX_DELAY
    );
}

BaseType_t enviarVeiculoParaCarregador(
    const Veiculo *veiculo
)
{
    if (
        veiculo == NULL ||
        filaAtendimento == NULL
    )
    {
        return pdFAIL;
    }

    return xQueueSend(
        filaAtendimento,
        veiculo,
        portMAX_DELAY
    );
}