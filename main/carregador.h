#ifndef CARREGADOR_H
#define CARREGADOR_H

#include "freertos/FreeRTOS.h"

#include "veiculo.h"

BaseType_t inicializarCarregadores(void);

BaseType_t aguardarVagaCarregador(void);

BaseType_t enviarVeiculoParaCarregador(
    const Veiculo *veiculo
);

#endif