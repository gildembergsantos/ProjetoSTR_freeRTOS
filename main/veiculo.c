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

Veiculo criarVeiculo(int id)
{
    Veiculo veiculo;

    veiculo.id = id;

    /*
     * Gera valores entre 10 e 70:
     * esp_random() % 61 produz valores entre 0 e 60.
     */
    veiculo.bateria = 10 + (esp_random() % 61);

    veiculo.prioridade =
        definirPrioridade(veiculo.bateria);

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