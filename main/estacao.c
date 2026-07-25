#include <stdio.h>

#include "estacao.h"
#include "config.h"
#include "fila.h"
#include "veiculo.h"
#include "carregador.h"

#include "freertos/task.h"

static void tarefaChegada(void *parametro)
{
    (void)parametro;

    int proximoId = 1;

    while (1)
    {
        Veiculo veiculo =
            criarVeiculo(proximoId);

        printf(
            "[CHEGADA] Veiculo %d | "
            "Bateria: %d%% | Prioridade: %s\n",
            veiculo.id,
            veiculo.bateria,
            textoPrioridade(veiculo.prioridade)
        );

        if (
            inserirVeiculoNaFila(&veiculo)
            != pdPASS
        )
        {
            printf(
                "[ERRO] Nao foi possivel inserir "
                "o veiculo %d na fila.\n",
                veiculo.id
            );
        }

        proximoId++;

        vTaskDelay(
            pdMS_TO_TICKS(INTERVALO_CHEGADA_MS)
        );
    }
}

static void tarefaEstacao(void *parametro)
{
    (void)parametro;

    Veiculo veiculo;

    while (1)
    {
        /*
         * Primeiro aguarda existir um veículo.
         */
        if (
            aguardarVeiculoDisponivel()
            != pdPASS
        )
        {
            printf(
                "[ERRO] Falha ao aguardar veiculo.\n"
            );

            continue;
        }

        /*
         * Depois aguarda algum carregador ficar livre.
         *
         * Enquanto todos estiverem ocupados, os veículos
         * continuam nas filas de prioridade.
         */
        if (
            aguardarVagaCarregador()
            != pdPASS
        )
        {
            printf(
                "[ERRO] Falha ao aguardar carregador.\n"
            );

            continue;
        }

        /*
         * Somente agora selecionamos o veículo.
         * Portanto, emergências que chegaram enquanto
         * os carregadores estavam ocupados podem passar
         * na frente dos veículos normais.
         */
        if (
            removerVeiculoPrioritario(&veiculo)
            != pdPASS
        )
        {
            printf(
                "[ERRO] Falha ao remover veiculo "
                "prioritario.\n"
            );

            continue;
        }

        printf(
            "[ESTACAO] Veiculo %d encaminhado | "
            "Prioridade: %s\n",
            veiculo.id,
            textoPrioridade(veiculo.prioridade)
        );

        if (
            enviarVeiculoParaCarregador(&veiculo)
            != pdPASS
        )
        {
            printf(
                "[ERRO] Falha ao enviar veiculo %d "
                "para carregamento.\n",
                veiculo.id
            );
        }
    }
}

BaseType_t inicializarEstacao(void)
{
    if (inicializarFilas() != pdPASS)
    {
        printf(
            "[ERRO] Falha ao inicializar filas.\n"
        );

        return pdFAIL;
    }

    if (inicializarCarregadores() != pdPASS)
    {
        printf(
            "[ERRO] Falha ao inicializar carregadores.\n"
        );

        return pdFAIL;
    }

    BaseType_t resultadoChegada = xTaskCreate(
        tarefaChegada,
        "tarefa_chegada",
        PILHA_TAREFA_CHEGADA,
        NULL,
        PRIORIDADE_TAREFA_CHEGADA,
        NULL
    );

    BaseType_t resultadoEstacao = xTaskCreate(
        tarefaEstacao,
        "tarefa_estacao",
        PILHA_TAREFA_ESTACAO,
        NULL,
        PRIORIDADE_TAREFA_ESTACAO,
        NULL
    );

    if (
        resultadoChegada != pdPASS ||
        resultadoEstacao != pdPASS
    )
    {
        printf(
            "[ERRO] Falha ao criar tarefas "
            "da estacao.\n"
        );

        return pdFAIL;
    }

    return pdPASS;
}