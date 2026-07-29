/**
 * @file main.c
 * @brief Ponto de entrada da aplicação.
 */

#include <stdio.h>

#include "estacao.h"
#include "hardware.h"

/**
 * @brief Inicializa o hardware e os recursos da estação.
 */
void app_main(void)
{
    printf(
        "\n[SISTEMA] Iniciando estacao de carregamento...\n"
    );

    if (inicializarHardware() != ESP_OK)
    {
        printf(
            "[ERRO] Falha ao inicializar hardware.\n"
        );

        return;
    }

    printf(
        "[SISTEMA] Hardware inicializado.\n"
    );

    if (inicializarEstacao() != pdPASS)
    {
        printf(
            "[ERRO] Nao foi possivel iniciar o sistema.\n"
        );

        return;
    }

    printf(
        "[SISTEMA] Estacao inicializada com sucesso.\n"
    );
}