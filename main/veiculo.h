#ifndef VEICULO_H
#define VEICULO_H

typedef enum
{
    PRIORIDADE_NORMAL = 0,
    PRIORIDADE_BATERIA_CRITICA = 1,
    PRIORIDADE_EMERGENCIA = 2
} PrioridadeVeiculo;

typedef enum
{
    TIPO_VEICULO_NORMAL = 0,
    TIPO_VEICULO_CRITICO,
    TIPO_VEICULO_EMERGENCIA
} TipoVeiculo;

typedef struct
{
    int id;
    int bateria;
    int tempoCarregamentoMs;
    PrioridadeVeiculo prioridade;
} Veiculo;


Veiculo criarVeiculoPorTipo(
    int id,
    TipoVeiculo tipo
);

PrioridadeVeiculo definirPrioridade(int bateria);

int calcularTempoCarregamentoMs(int bateria);

const char *textoPrioridade(
    PrioridadeVeiculo prioridade
);

#endif