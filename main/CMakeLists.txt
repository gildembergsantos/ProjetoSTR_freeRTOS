idf_component_register(
    SRCS
        "main.c"
        "veiculo.c"
        "fila.c"
        "estacao.c"
        "carregador.c"
        "hardware.c"

    INCLUDE_DIRS
        "."

    REQUIRES
        esp_driver_gpio
)