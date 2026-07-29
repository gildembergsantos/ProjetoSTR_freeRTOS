#ifndef VEICULO_H
#define VEICULO_H

/**
 * @file veiculo.h
 * @brief Definições e operações relacionadas aos veículos.
 */

/* Níveis de prioridade utilizados no atendimento. */
typedef enum
{
    PRIORIDADE_NORMAL = 0,
    PRIORIDADE_BATERIA_CRITICA = 1,
    PRIORIDADE_EMERGENCIA = 2
} PrioridadeVeiculo;

/* Tipos de veículos gerados pelos botões. */
typedef enum
{
    TIPO_VEICULO_NORMAL = 0,
    TIPO_VEICULO_CRITICO,
    TIPO_VEICULO_EMERGENCIA
} TipoVeiculo;

/* Representa um veículo aguardando atendimento. */
typedef struct
{
    int id;
    int bateria;
    int tempoCarregamentoMs;
    PrioridadeVeiculo prioridade;
} Veiculo;

/**
 * @brief Cria um veículo com os atributos do tipo informado.
 *
 * @param id Identificador do veículo.
 * @param tipo Tipo de veículo.
 * @return Estrutura do veículo criada.
 */
Veiculo criarVeiculoPorTipo(
    int id,
    TipoVeiculo tipo
);

/**
 * @brief Define a prioridade de atendimento a partir da bateria.
 *
 * @param bateria Nível de bateria do veículo.
 * @return Prioridade correspondente.
 */
PrioridadeVeiculo definirPrioridade(int bateria);

/**
 * @brief Calcula o tempo de carregamento em milissegundos.
 *
 * @param bateria Nível de bateria do veículo.
 * @return Tempo estimado de carregamento.
 */
int calcularTempoCarregamentoMs(int bateria);

/**
 * @brief Retorna o texto correspondente à prioridade.
 *
 * @param prioridade Prioridade do veículo.
 * @return Texto descritivo da prioridade.
 */
const char *textoPrioridade(
    PrioridadeVeiculo prioridade
);

#endif /* VEICULO_H */