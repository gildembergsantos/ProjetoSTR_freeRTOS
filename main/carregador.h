#ifndef CARREGADOR_H
#define CARREGADOR_H

/**
 * @file carregador.h
 * @brief Interface do módulo responsável pelos carregadores da estação.
 *
 * Este arquivo declara as funções utilizadas para inicializar os
 * carregadores, aguardar a disponibilidade de uma vaga e encaminhar
 * veículos para as tarefas responsáveis pelo carregamento.
 */

#include "freertos/FreeRTOS.h"

#include "veiculo.h"

/**
 * @brief Inicializa os recursos relacionados aos carregadores.
 *
 * Esta função deve criar e configurar os elementos necessários para o
 * funcionamento dos carregadores, como tarefas, filas e semáforos.
 *
 * @return pdPASS em caso de sucesso.
 * @return pdFAIL caso algum recurso não possa ser criado.
 */
BaseType_t inicializarCarregadores(void);

/**
 * @brief Aguarda até que exista pelo menos um carregador disponível.
 *
 * A função bloqueia a tarefa chamadora até que uma vaga de carregador
 * seja liberada. Normalmente, essa sincronização é realizada por meio
 * de um semáforo contador.
 *
 * @return pdTRUE quando uma vaga de carregador é obtida.
 * @return pdFALSE caso não seja possível obter a vaga.
 */
BaseType_t aguardarVagaCarregador(void);

/**
 * @brief Envia um veículo para uma tarefa de carregamento.
 *
 * O veículo recebido é encaminhado para a fila compartilhada pelos
 * carregadores. Uma das tarefas de carregamento disponíveis retira o
 * veículo da fila e inicia o processo de carregamento.
 *
 * @param veiculo Ponteiro para a estrutura que contém os dados do veículo.
 *
 * @return pdPASS se o veículo for enviado com sucesso.
 * @return pdFAIL se o envio para a fila falhar.
 */
BaseType_t enviarVeiculoParaCarregador(
    const Veiculo *veiculo
);

#endif /* CARREGADOR_H */