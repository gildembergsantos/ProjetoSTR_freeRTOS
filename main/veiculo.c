/**
 * @file veiculo.c
 * @brief Implementação das operações relacionadas aos veículos.
 */

#include "veiculo.h"
#include "config.h"

#include "esp_random.h"

/**
 * @brief Define a prioridade de acordo com a bateria e a chance de emergência.
 *
 * @param bateria Nível de bateria do veículo.
 * @return Prioridade atribuída ao veículo.
 */
PrioridadeVeiculo definirPrioridade(int bateria)
{
    PrioridadeVeiculo prioridade = PRIORIDADE_NORMAL;

    if (bateria <= LIMITE_BATERIA_CRITICA)
    {
        prioridade = PRIORIDADE_BATERIA_CRITICA;
    }

    /* A emergência possui prioridade superior às demais classificações. */
    if ((esp_random() % 100) < CHANCE_EMERGENCIA)
    {
        prioridade = PRIORIDADE_EMERGENCIA;
    }

    return prioridade;
}

/**
 * @brief Calcula o tempo necessário para completar a carga.
 *
 * @param bateria Nível atual de bateria.
 * @return Tempo de carregamento em milissegundos.
 */
int calcularTempoCarregamentoMs(int bateria)
{
    /* Limita a bateria ao intervalo válido. */
    if (bateria < 0)
    {
        bateria = 0;
    }
    else if (bateria > 100)
    {
        bateria = 100;
    }

    int percentualRestante = 100 - bateria;

    return percentualRestante *
           TEMPO_POR_PERCENTUAL_MS;
}

/**
 * @brief Cria um veículo com atributos definidos pelo tipo.
 *
 * @param id Identificador do veículo.
 * @param tipo Tipo de veículo.
 * @return Veículo criado.
 */
Veiculo criarVeiculoPorTipo(
    int id,
    TipoVeiculo tipo
)
{
    Veiculo veiculo;

    veiculo.id = id;

    switch (tipo)
    {
        case TIPO_VEICULO_CRITICO:
            /* Gera bateria entre 10% e 30%. */
            veiculo.bateria =
                10 + (esp_random() % 21);

            veiculo.prioridade =
                PRIORIDADE_BATERIA_CRITICA;

            break;

        case TIPO_VEICULO_EMERGENCIA:
            /* Gera bateria entre 10% e 70%. */
            veiculo.bateria =
                10 + (esp_random() % 61);

            veiculo.prioridade =
                PRIORIDADE_EMERGENCIA;

            break;

        case TIPO_VEICULO_NORMAL:
        default:
            /* Gera bateria entre 31% e 70%. */
            veiculo.bateria =
                31 + (esp_random() % 40);

            veiculo.prioridade =
                PRIORIDADE_NORMAL;

            break;
    }

    veiculo.tempoCarregamentoMs =
        calcularTempoCarregamentoMs(
            veiculo.bateria
        );

    return veiculo;
}

/**
 * @brief Retorna o texto correspondente à prioridade.
 *
 * @param prioridade Prioridade do veículo.
 * @return Texto descritivo da prioridade.
 */
const char *textoPrioridade(PrioridadeVeiculo prioridade)
{
    switch (prioridade)
    {
        case PRIORIDADE_EMERGENCIA:
            return "EMERGENCIA";

        case PRIORIDADE_BATERIA_CRITICA:
            return "BATERIA CRITICA";

        case PRIORIDADE_NORMAL:
        default:
            return "NORMAL";
    }
}