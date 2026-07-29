#include <stdio.h>

#include "estacao.h"
#include "config.h"
#include "fila.h"
#include "veiculo.h"
#include "carregador.h"
#include "hardware.h"

#include "freertos/task.h"

static TipoVeiculo converterEventoParaTipo(
    EventoBotao evento
)
{
    switch (evento)
    {
        case EVENTO_BOTAO_CRITICO:
            return TIPO_VEICULO_CRITICO;

        case EVENTO_BOTAO_EMERGENCIA:
            return TIPO_VEICULO_EMERGENCIA;

        case EVENTO_BOTAO_NORMAL:
        default:
            return TIPO_VEICULO_NORMAL;
    }
}

static void tarefaChegada(void *parametro)
{
    (void)parametro;

    int proximoId = 1;

    /*
     * Um instante anterior para cada botão.
     * Utilizado no tratamento de debounce.
     */
    TickType_t ultimoEvento[3] = {
        0,
        0,
        0
    };

    EventoBotao evento;

    while (1)
    {
        /*
         * A tarefa permanece bloqueada até algum botão
         * gerar um evento.
         */
        if (
            aguardarEventoBotao(
                &evento,
                portMAX_DELAY
            ) != pdPASS
        )
        {
            continue;
        }

        TickType_t instanteAtual =
            xTaskGetTickCount();

        /*
         * Ignora eventos repetidos do mesmo botão
         * dentro da janela de debounce.
         */
        if (
            ultimoEvento[evento] != 0 &&
            (instanteAtual - ultimoEvento[evento]) <
                pdMS_TO_TICKS(TEMPO_DEBOUNCE_MS)
        )
        {
            continue;
        }

        ultimoEvento[evento] = instanteAtual;

        TipoVeiculo tipo =
            converterEventoParaTipo(evento);

        Veiculo veiculo =
            criarVeiculoPorTipo(
                proximoId,
                tipo
            );

        printf(
            "[BOTAO] Veiculo %d adicionado | "
            "Bateria: %d%% | Tempo: %.1f s | "
            "Prioridade: %s\n",
            veiculo.id,
            veiculo.bateria,
            veiculo.tempoCarregamentoMs / 1000.0,
            textoPrioridade(
                veiculo.prioridade
            )
        );

        if (
            inserirVeiculoNaFila(&veiculo)
            != pdPASS
        )
        {
            printf(
                "[ERRO] Fila cheia. Veiculo %d "
                "nao foi inserido.\n",
                veiculo.id
            );
        }
        else
        {
            proximoId++;
        }
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