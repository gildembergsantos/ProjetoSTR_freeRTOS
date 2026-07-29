#ifndef HARDWARE_H
#define HARDWARE_H

/**
 * @file hardware.h
 * @brief Interface de acesso aos botões e LEDs.
 */

#include "esp_err.h"

#include "freertos/FreeRTOS.h"

/* Eventos gerados pelos botões da estação. */
typedef enum
{
    EVENTO_BOTAO_NORMAL = 0,
    EVENTO_BOTAO_CRITICO,
    EVENTO_BOTAO_EMERGENCIA
} EventoBotao;

/**
 * @brief Inicializa os GPIOs, a fila de eventos e as interrupções.
 *
 * @return ESP_OK em caso de sucesso.
 * @return Código de erro em caso de falha.
 */
esp_err_t inicializarHardware(void);

/**
 * @brief Liga o LED do carregador informado.
 *
 * @param idCarregador Identificador do carregador.
 * @return ESP_OK em caso de sucesso.
 * @return Código de erro em caso de falha.
 */
esp_err_t ligarLedCarregador(int idCarregador);

/**
 * @brief Desliga o LED do carregador informado.
 *
 * @param idCarregador Identificador do carregador.
 * @return ESP_OK em caso de sucesso.
 * @return Código de erro em caso de falha.
 */
esp_err_t desligarLedCarregador(int idCarregador);

/**
 * @brief Aguarda um evento gerado por um dos botões.
 *
 * @param evento Ponteiro para armazenar o evento recebido.
 * @param tempoEspera Tempo máximo de espera em ticks.
 * @return pdPASS em caso de sucesso.
 * @return pdFAIL em caso de falha ou timeout.
 */
BaseType_t aguardarEventoBotao(
    EventoBotao *evento,
    TickType_t tempoEspera
);

#endif /* HARDWARE_H */