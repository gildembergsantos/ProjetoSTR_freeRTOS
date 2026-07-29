/**
 * @file carregador.c
 * @brief Implementação do gerenciamento dos carregadores da estação.
 *
 * Este módulo cria e controla as tarefas responsáveis pelos carregadores.
 * Cada carregador executa como uma tarefa independente do FreeRTOS e retira
 * veículos de uma fila compartilhada de atendimento.
 *
 * O módulo também utiliza um semáforo contador para representar a quantidade
 * de carregadores disponíveis na estação.
 */

#include <stdio.h>

#include "carregador.h"
#include "config.h"
#include "veiculo.h"
#include "hardware.h"

#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/task.h"

/**
 * @brief Fila compartilhada entre as tarefas dos carregadores.
 *
 * A tarefa responsável pela estação envia os veículos para esta fila.
 * As tarefas dos carregadores permanecem bloqueadas aguardando a chegada
 * de um veículo e, quando disponível, uma delas retira o próximo item.
 *
 * O tamanho da fila é igual ao número total de carregadores, pois um veículo
 * somente é encaminhado após a reserva de uma vaga por meio do semáforo
 * contador.
 */
static QueueHandle_t filaAtendimento = NULL;

/**
 * @brief Semáforo contador que representa os carregadores disponíveis.
 *
 * O valor inicial e máximo do semáforo é definido por NUM_CARREGADORES.
 * Cada atendimento iniciado consome uma unidade do semáforo. Ao final do
 * carregamento, a tarefa devolve essa unidade, indicando que o carregador
 * voltou a ficar disponível.
 */
static SemaphoreHandle_t vagasCarregadores = NULL;

/**
 * @brief Identificadores associados às tarefas dos carregadores.
 *
 * O vetor possui armazenamento estático para garantir que os endereços
 * enviados como parâmetros para xTaskCreate() permaneçam válidos durante
 * toda a execução da aplicação.
 */
static int idsCarregadores[NUM_CARREGADORES];

/**
 * @brief Tarefa responsável por executar o atendimento de um carregador.
 *
 * Cada instância desta tarefa representa um carregador da estação. A tarefa
 * aguarda indefinidamente por um veículo na fila compartilhada, sinaliza o
 * início do carregamento por meio do LED correspondente, aguarda o tempo de
 * carregamento calculado para o veículo e, ao final, libera novamente a vaga
 * ocupada no semáforo contador.
 *
 * @param parametro Ponteiro para o identificador numérico do carregador.
 */
static void tarefaCarregador(void *parametro)
{
    /*
     * Recupera o identificador recebido durante a criação da tarefa.
     * Cada carregador recebe um valor entre 1 e NUM_CARREGADORES.
     */
    int idCarregador = *((int *)parametro);

    /* Estrutura local utilizada para receber veículos da fila de atendimento. */
    Veiculo veiculo;

    while (1)
    {
        /*
         * Mantém a tarefa bloqueada até que um veículo seja disponibilizado.
         * O uso de portMAX_DELAY evita consumo desnecessário de processamento
         * enquanto não houver atendimento pendente.
         */
        if (xQueueReceive(
                filaAtendimento,
                &veiculo,
                portMAX_DELAY
            ) == pdPASS)
        {
            /*
             * Sinaliza visualmente que o carregador está ocupado.
             * Neste projeto, o LED associado ao carregador é acionado durante
             * o período de atendimento.
             */
            ligarLedCarregador(idCarregador);

            printf(
                "[CARREGADOR %d] Veiculo %d iniciado | "
                "Bateria: %d%% | Tempo: %.1f s | Prioridade: %s\n",
                idCarregador,
                veiculo.id,
                veiculo.bateria,
                veiculo.tempoCarregamentoMs / 1000.0,
                textoPrioridade(veiculo.prioridade)
            );

            /*
             * Simula o período de carregamento sem bloquear o escalonador.
             * Durante esse intervalo, a tarefa entra no estado bloqueado e
             * permite que outras tarefas do sistema sejam executadas.
             */
            vTaskDelay(
                pdMS_TO_TICKS(
                    veiculo.tempoCarregamentoMs
                )
            );

            printf(
                "[CARREGADOR %d] Veiculo %d concluido "
                "apos %.1f s.\n",
                idCarregador,
                veiculo.id,
                veiculo.tempoCarregamentoMs / 1000.0
            );

            /*
             * Atualiza a sinalização visual para indicar o encerramento
             * do atendimento deste veículo.
             */
            desligarLedCarregador(idCarregador);

            /*
             * Devolve uma unidade ao semáforo contador, indicando que este
             * carregador está novamente disponível para receber outro veículo.
             */
            xSemaphoreGive(vagasCarregadores);
        }
    }
}

/**
 * @brief Inicializa a fila, o semáforo e as tarefas dos carregadores.
 *
 * A função cria:
 *
 * - uma fila compartilhada de veículos;
 * - um semáforo contador de vagas disponíveis;
 * - uma tarefa permanente para cada carregador da estação.
 *
 * O semáforo é inicializado com todas as vagas disponíveis. Cada tarefa recebe
 * um identificador próprio armazenado no vetor idsCarregadores.
 *
 * @return pdPASS se todos os recursos forem criados corretamente.
 * @return pdFAIL se ocorrer falha na criação da fila, do semáforo ou de alguma
 * tarefa.
 */
BaseType_t inicializarCarregadores(void)
{
    /*
     * Cria a fila utilizada para distribuir veículos entre os carregadores.
     * Cada item armazenado corresponde a uma cópia completa da estrutura
     * Veiculo.
     */
    filaAtendimento = xQueueCreate(
        NUM_CARREGADORES,
        sizeof(Veiculo)
    );

    /*
     * Cria um semáforo contador cuja contagem representa a quantidade de
     * carregadores livres. Inicialmente, todos estão disponíveis.
     */
    vagasCarregadores = xSemaphoreCreateCounting(
        NUM_CARREGADORES,
        NUM_CARREGADORES
    );

    /* Interrompe a inicialização caso algum recurso não tenha sido criado. */
    if (
        filaAtendimento == NULL ||
        vagasCarregadores == NULL
    )
    {
        return pdFAIL;
    }

    /*
     * Cria uma tarefa independente para cada carregador configurado
     * na aplicação.
     */
    for (int i = 0; i < NUM_CARREGADORES; i++)
    {
        /*
         * Os identificadores começam em 1 para facilitar a associação
         * entre carregadores, mensagens no terminal e LEDs físicos.
         */
        idsCarregadores[i] = i + 1;

        BaseType_t resultado = xTaskCreate(
            tarefaCarregador,
            "tarefa_carregador",
            PILHA_TAREFA_CARREGADOR,
            &idsCarregadores[i],
            PRIORIDADE_TAREFA_CARREGADOR,
            NULL
        );

        /*
         * Caso uma das tarefas não seja criada, a função informa falha
         * ao módulo responsável pela inicialização geral da aplicação.
         */
        if (resultado != pdPASS)
        {
            return pdFAIL;
        }
    }

    return pdPASS;
}

/**
 * @brief Reserva uma vaga de carregador.
 *
 * A função aguarda indefinidamente até que exista pelo menos um carregador
 * disponível. Quando a operação é concluída, a contagem do semáforo é
 * decrementada, reservando uma vaga antes do envio do veículo para a fila.
 *
 * @return pdTRUE quando uma vaga é obtida.
 * @return pdFAIL se o semáforo ainda não tiver sido inicializado.
 */
BaseType_t aguardarVagaCarregador(void)
{
    /* Evita o uso do semáforo antes da inicialização do módulo. */
    if (vagasCarregadores == NULL)
    {
        return pdFAIL;
    }

    /*
     * Bloqueia a tarefa chamadora enquanto todos os carregadores estiverem
     * ocupados.
     */
    return xSemaphoreTake(
        vagasCarregadores,
        portMAX_DELAY
    );
}

/**
 * @brief Encaminha um veículo para a fila compartilhada dos carregadores.
 *
 * O veículo é copiado para a fila de atendimento. Uma das tarefas de
 * carregador que estiver bloqueada em xQueueReceive() receberá esse veículo
 * e iniciará o processo de carregamento.
 *
 * Esta função deve ser chamada somente após a obtenção de uma vaga por meio
 * de aguardarVagaCarregador().
 *
 * @param veiculo Ponteiro para a estrutura do veículo a ser atendido.
 *
 * @return pdPASS se o veículo for enviado com sucesso.
 * @return pdFAIL se o ponteiro for inválido ou a fila não estiver inicializada.
 */
BaseType_t enviarVeiculoParaCarregador(
    const Veiculo *veiculo
)
{
    /* Valida os parâmetros e o estado de inicialização do módulo. */
    if (
        veiculo == NULL ||
        filaAtendimento == NULL
    )
    {
        return pdFAIL;
    }

    /*
     * Aguarda até que exista espaço na fila. Em condições normais, a reserva
     * prévia de uma vaga no semáforo mantém a quantidade de veículos enviados
     * compatível com o número de carregadores disponíveis.
     */
    return xQueueSend(
        filaAtendimento,
        veiculo,
        portMAX_DELAY
    );
}