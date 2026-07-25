#include <stdio.h>

#include "estacao.h"

void app_main(void)
{
    printf(
        "\n[SISTEMA] Iniciando estacao de carregamento...\n"
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