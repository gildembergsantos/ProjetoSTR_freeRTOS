/**
 * @file estacao.c
 * @brief Implementação do gerenciamento da estação.
 */

#include <stdio.h>

#include "estacao.h"
#include "config.h"
#include "fila.h"
#include "veiculo.h"
#include "carregador.h"
#include "hardware.h"

#include "freertos/task.h"

/**
 * @brief Converte o evento de botão para o tipo de veículo.
 *
 * @param evento Evento recebido da fila de botões.
 * @return Tipo de veículo correspondente.
 */
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

/**
 * @brief Processa os eventos dos botões e cria novos veículos.
 *
 * @param parametro Parâmetro não utilizado.
 */
static void tarefaChegada(void *parametro)
{
    (void)parametro;

    int proximoId = 1;

    /* Armazena o instante do último evento de cada botão. */
    TickType_t ultimoEvento[3] = {
        0,
        0,
        0
    };

    EventoBotao evento;

    while (1)
    {
        /* Aguarda um evento gerado por algum botão. */
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

        /* Ignora eventos repetidos dentro do tempo de debounce. */
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

        /* Insere o veículo na fila correspondente à sua prioridade. */
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

/**
 * @brief Seleciona veículos e os encaminha aos carregadores.
 *
 * @param parametro Parâmetro não utilizado.
 */
static void tarefaEstacao(void *parametro)
{
    (void)parametro;

    Veiculo veiculo;

    while (1)
    {
        /* Aguarda até existir pelo menos um veículo nas filas. */
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

        /* Aguarda até existir um carregador disponível. */
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

        /* Remove o veículo de maior prioridade disponível. */
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

        /* Envia o veículo para a fila compartilhada dos carregadores. */
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

/**
 * @brief Inicializa as filas, os carregadores e as tarefas da estação.
 *
 * @return pdPASS em caso de sucesso.
 * @return pdFAIL em caso de falha.
 */
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