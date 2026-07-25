#ifndef VEICULO_H
#define VEICULO_H

typedef enum
{
    PRIORIDADE_NORMAL = 0,
    PRIORIDADE_BATERIA_CRITICA = 1,
    PRIORIDADE_EMERGENCIA = 2
} PrioridadeVeiculo;

typedef struct
{
    int id;
    int bateria;
    PrioridadeVeiculo prioridade;
} Veiculo;

Veiculo criarVeiculo(int id);

PrioridadeVeiculo definirPrioridade(int bateria);

const char *textoPrioridade(PrioridadeVeiculo prioridade);

#endif