#ifndef HARDWARE_H
#define HARDWARE_H

#include "esp_err.h"

#include "freertos/FreeRTOS.h"

typedef enum
{
    EVENTO_BOTAO_NORMAL = 0,
    EVENTO_BOTAO_CRITICO,
    EVENTO_BOTAO_EMERGENCIA
} EventoBotao;

esp_err_t inicializarHardware(void);

esp_err_t ligarLedCarregador(int idCarregador);

esp_err_t desligarLedCarregador(int idCarregador);

/*
 * Mantém a tarefa bloqueada até ocorrer o pressionamento
 * de algum dos três botões.
 */
BaseType_t aguardarEventoBotao(
    EventoBotao *evento,
    TickType_t tempoEspera
);

#endif