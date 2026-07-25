#ifndef FILA_H
#define FILA_H

#include "freertos/FreeRTOS.h"

#include "veiculo.h"

BaseType_t inicializarFilas(void);

BaseType_t inserirVeiculoNaFila(
    const Veiculo *veiculo
);

/*
 * Bloqueia a tarefa até existir pelo menos
 * um veículo aguardando.
 */
BaseType_t aguardarVeiculoDisponivel(void);

/*
 * Retira um veículo sem bloquear, verificando:
 * emergência → bateria crítica → normal.
 */
BaseType_t removerVeiculoPrioritario(
    Veiculo *veiculo
);

#endif