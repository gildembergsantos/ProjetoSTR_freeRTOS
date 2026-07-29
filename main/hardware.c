/**
 * @file hardware.c
 * @brief Implementação do acesso aos botões e LEDs.
 */

#include <stdint.h>

#include "hardware.h"
#include "config.h"

#include "driver/gpio.h"
#include "esp_attr.h"

#include "freertos/queue.h"

/* Transporta os eventos das interrupções até a tarefa de chegada. */
static QueueHandle_t filaEventosBotoes = NULL;

/**
 * @brief Obtém o GPIO associado a um carregador.
 *
 * @param idCarregador Identificador do carregador.
 * @return GPIO correspondente ou GPIO_NUM_NC para um ID inválido.
 */
static gpio_num_t obterGpioLedCarregador(
    int idCarregador
)
{
    switch (idCarregador)
    {
        case 1:
            return GPIO_NUM_25;

        case 2:
            return GPIO_NUM_26;

        case 3:
            return GPIO_NUM_27;

        default:
            return GPIO_NUM_NC;
    }
}

/**
 * @brief Envia o evento do botão para a fila a partir da interrupção.
 *
 * @param arg Tipo de evento associado ao botão.
 */
static void IRAM_ATTR tratarInterrupcaoBotao(void *arg)
{
    EventoBotao evento =
        (EventoBotao)(intptr_t)arg;

    BaseType_t tarefaMaiorPrioridadeAcordada =
        pdFALSE;

    xQueueSendFromISR(
        filaEventosBotoes,
        &evento,
        &tarefaMaiorPrioridadeAcordada
    );

    if (tarefaMaiorPrioridadeAcordada == pdTRUE)
    {
        portYIELD_FROM_ISR();
    }
}

/**
 * @brief Configura os GPIOs utilizados pelos LEDs.
 *
 * @return ESP_OK em caso de sucesso.
 * @return Código de erro em caso de falha.
 */
static esp_err_t configurarLeds(void)
{
    gpio_config_t configuracaoLeds = {
        .pin_bit_mask =
            (1ULL << GPIO_LED_CARREGADOR_1) |
            (1ULL << GPIO_LED_CARREGADOR_2) |
            (1ULL << GPIO_LED_CARREGADOR_3),

        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };

    esp_err_t resultado =
        gpio_config(&configuracaoLeds);

    if (resultado != ESP_OK)
    {
        return resultado;
    }

    gpio_set_level(GPIO_LED_CARREGADOR_1, 0);
    gpio_set_level(GPIO_LED_CARREGADOR_2, 0);
    gpio_set_level(GPIO_LED_CARREGADOR_3, 0);

    return ESP_OK;
}

/**
 * @brief Configura os botões e suas interrupções.
 *
 * @return ESP_OK em caso de sucesso.
 * @return Código de erro em caso de falha.
 */
static esp_err_t configurarBotoes(void)
{
    /* Os botões são conectados entre o GPIO e o GND. */
    gpio_config_t configuracaoBotoes = {
        .pin_bit_mask =
            (1ULL << GPIO_BOTAO_NORMAL) |
            (1ULL << GPIO_BOTAO_CRITICO) |
            (1ULL << GPIO_BOTAO_EMERGENCIA),

        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE
    };

    esp_err_t resultado =
        gpio_config(&configuracaoBotoes);

    if (resultado != ESP_OK)
    {
        return resultado;
    }

    resultado = gpio_install_isr_service(0);

    if (
        resultado != ESP_OK &&
        resultado != ESP_ERR_INVALID_STATE
    )
    {
        return resultado;
    }

    resultado = gpio_isr_handler_add(
        GPIO_BOTAO_NORMAL,
        tratarInterrupcaoBotao,
        (void *)(intptr_t)EVENTO_BOTAO_NORMAL
    );

    if (resultado != ESP_OK)
    {
        return resultado;
    }

    resultado = gpio_isr_handler_add(
        GPIO_BOTAO_CRITICO,
        tratarInterrupcaoBotao,
        (void *)(intptr_t)EVENTO_BOTAO_CRITICO
    );

    if (resultado != ESP_OK)
    {
        return resultado;
    }

    resultado = gpio_isr_handler_add(
        GPIO_BOTAO_EMERGENCIA,
        tratarInterrupcaoBotao,
        (void *)(intptr_t)EVENTO_BOTAO_EMERGENCIA
    );

    return resultado;
}

/**
 * @brief Inicializa a fila de eventos, os LEDs e os botões.
 *
 * @return ESP_OK em caso de sucesso.
 * @return Código de erro em caso de falha.
 */
esp_err_t inicializarHardware(void)
{
    filaEventosBotoes = xQueueCreate(
        TAMANHO_FILA_BOTOES,
        sizeof(EventoBotao)
    );

    if (filaEventosBotoes == NULL)
    {
        return ESP_ERR_NO_MEM;
    }

    esp_err_t resultado = configurarLeds();

    if (resultado != ESP_OK)
    {
        return resultado;
    }

    resultado = configurarBotoes();

    if (resultado != ESP_OK)
    {
        return resultado;
    }

    return ESP_OK;
}

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
)
{
    if (
        evento == NULL ||
        filaEventosBotoes == NULL
    )
    {
        return pdFAIL;
    }

    return xQueueReceive(
        filaEventosBotoes,
        evento,
        tempoEspera
    );
}

/**
 * @brief Liga o LED de um carregador.
 *
 * @param idCarregador Identificador do carregador.
 * @return ESP_OK em caso de sucesso.
 * @return ESP_ERR_INVALID_ARG para um ID inválido.
 */
esp_err_t ligarLedCarregador(int idCarregador)
{
    gpio_num_t gpio =
        obterGpioLedCarregador(idCarregador);

    if (gpio == GPIO_NUM_NC)
    {
        return ESP_ERR_INVALID_ARG;
    }

    return gpio_set_level(gpio, 1);
}

/**
 * @brief Desliga o LED de um carregador.
 *
 * @param idCarregador Identificador do carregador.
 * @return ESP_OK em caso de sucesso.
 * @return ESP_ERR_INVALID_ARG para um ID inválido.
 */
esp_err_t desligarLedCarregador(int idCarregador)
{
    gpio_num_t gpio =
        obterGpioLedCarregador(idCarregador);

    if (gpio == GPIO_NUM_NC)
    {
        return ESP_ERR_INVALID_ARG;
    }

    return gpio_set_level(gpio, 0);
}