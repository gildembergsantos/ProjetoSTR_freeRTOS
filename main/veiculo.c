#include "veiculo.h"
#include "config.h"

#include "esp_random.h"



PrioridadeVeiculo definirPrioridade(int bateria)
{
    PrioridadeVeiculo prioridade = PRIORIDADE_NORMAL;

    if (bateria <= LIMITE_BATERIA_CRITICA)
    {
        prioridade = PRIORIDADE_BATERIA_CRITICA;
    }

    /*
     * A emergência possui prioridade maior e, por isso,
     * substitui a classificação de bateria crítica.
     */
    if ((esp_random() % 100) < CHANCE_EMERGENCIA)
    {
        prioridade = PRIORIDADE_EMERGENCIA;
    }

    return prioridade;
}

int calcularTempoCarregamentoMs(int bateria)
{
    /*
     * Proteção contra valores inválidos.
     */
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
            /*
             * Bateria entre 10% e 30%.
             */
            veiculo.bateria =
                10 + (esp_random() % 21);

            veiculo.prioridade =
                PRIORIDADE_BATERIA_CRITICA;

            break;

        case TIPO_VEICULO_EMERGENCIA:
            /*
             * Emergência pode possuir qualquer bateria
             * dentro da faixa da simulação.
             */
            veiculo.bateria =
                10 + (esp_random() % 61);

            veiculo.prioridade =
                PRIORIDADE_EMERGENCIA;

            break;

        case TIPO_VEICULO_NORMAL:
        default:
            /*
             * Bateria entre 31% e 70%.
             */
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