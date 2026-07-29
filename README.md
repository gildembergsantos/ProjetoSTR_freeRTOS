# Estação de Carregamento de Veículos Elétricos utilizando FreeRTOS e ESP32

## Descrição

Este repositório apresenta o desenvolvimento de uma estação inteligente de carregamento de veículos elétricos implementada utilizando o sistema operacional de tempo real FreeRTOS executando em uma placa ESP32.

O projeto foi desenvolvido para a disciplina de Sistemas em Tempo Real e tem como objetivo aplicar, em uma plataforma embarcada, os principais mecanismos disponibilizados pelo FreeRTOS, incluindo criação de tarefas concorrentes, comunicação entre tarefas por filas, sincronização por semáforos contadores e tratamento de interrupções de hardware.

Inicialmente, a arquitetura da aplicação foi desenvolvida em ambiente Linux utilizando POSIX Threads (Pthreads), permitindo validar toda a lógica de funcionamento da estação. Posteriormente, a implementação foi migrada para a plataforma ESP32 utilizando o framework ESP-IDF, possibilitando sua execução em hardware real.

---

## Objetivos

O projeto possui os seguintes objetivos:

- implementar uma estação de carregamento de veículos elétricos utilizando FreeRTOS;
- aplicar conceitos de sistemas operacionais de tempo real em uma aplicação embarcada;
- desenvolver uma arquitetura modular baseada em tarefas concorrentes;
- integrar periféricos da ESP32 à lógica do sistema;
- validar o funcionamento da aplicação em hardware real.

---

## Tecnologias Utilizadas

- Linguagem C
- ESP32 DevKit (ESP-WROOM-32)
- ESP-IDF
- FreeRTOS
- Visual Studio Code
- Git
- GitHub

---

## Organização do Projeto

A aplicação foi estruturada em módulos independentes, buscando facilitar a manutenção e a expansão do software.

```
ProjetoSTR_freeRTOS
│
├── main
│   ├── main.c
│   ├── config.h
│   ├── hardware.c
│   ├── hardware.h
│   ├── veiculo.c
│   ├── veiculo.h
│   ├── fila.c
│   ├── fila.h
│   ├── estacao.c
│   ├── estacao.h
│   ├── carregador.c
│   └── carregador.h
│
├── CMakeLists.txt
├── sdkconfig
└── README.md
```

Cada módulo possui responsabilidades específicas.

| Arquivo | Responsabilidade |
|----------|------------------|
| `main.c` | Inicialização da aplicação |
| `config.h` | Configuração geral do sistema |
| `hardware.c` | Configuração dos GPIOs, botões e LEDs |
| `veiculo.c` | Estrutura dos veículos e cálculo do tempo de carregamento |
| `fila.c` | Gerenciamento das filas de prioridade |
| `estacao.c` | Gerenciamento da estação de carregamento |
| `carregador.c` | Controle das tarefas dos carregadores |

---

## Funcionamento da Aplicação

A estação possui três carregadores independentes.

A chegada dos veículos é simulada através de três botões físicos conectados às entradas digitais da ESP32.

Cada botão representa uma categoria distinta de veículo:

| Botão | Categoria |
|--------|-----------|
| Botão 1 | Veículo Normal |
| Botão 2 | Veículo com Bateria Crítica |
| Botão 3 | Veículo de Emergência |

Quando um botão é pressionado, ocorre uma interrupção de hardware (GPIO Interrupt). A rotina de interrupção apenas envia um evento para uma fila do FreeRTOS, sendo a criação efetiva do veículo realizada posteriormente por uma tarefa dedicada.

Após sua criação, o veículo é classificado conforme sua prioridade e inserido na fila correspondente.

Sempre que um carregador torna-se disponível, a tarefa responsável pelo gerenciamento da estação seleciona o próximo veículo de acordo com a política de prioridades implementada.

---

## Política de Atendimento

A aplicação utiliza três filas independentes.

A ordem de atendimento é definida pela seguinte prioridade:

1. Veículos de Emergência
2. Veículos com Bateria Crítica
3. Veículos Normais

Dentro de cada categoria, os veículos são atendidos em ordem FIFO (First In, First Out).

---

## Tempo de Carregamento

O tempo de carregamento é calculado dinamicamente em função do nível inicial da bateria.

Foi adotado o seguinte modelo:

\[
T=(100-B)\times100
\]

onde:

- **T** representa o tempo de carregamento em milissegundos;
- **B** representa o percentual inicial da bateria.

Assim, veículos com menor carga permanecem mais tempo ocupando os carregadores, tornando a simulação mais próxima de um cenário real.

---

## Utilização dos Periféricos

### Botões

Os botões representam a chegada dos veículos à estação.

Cada acionamento gera uma interrupção de hardware responsável por notificar a tarefa de chegada dos veículos.

### LEDs

Cada carregador possui um LED associado.

| Estado do carregador | LED |
|----------------------|-----|
| Disponível | Aceso |
| Ocupado | Apagado |

Essa sinalização permite acompanhar visualmente o estado da estação durante a execução da aplicação.

---

## Recursos do FreeRTOS Utilizados

O projeto utiliza diversos mecanismos disponibilizados pelo FreeRTOS, entre eles:

- criação de tarefas concorrentes;
- escalonamento por prioridades;
- filas de mensagens;
- semáforos contadores;
- comunicação entre interrupções e tarefas;
- temporização por meio de `vTaskDelay()`;
- sincronização entre tarefas.

---

## Compilação

Após configurar corretamente o ambiente ESP-IDF, utilize os seguintes comandos.

Selecionar o alvo:

```bash
idf.py set-target esp32
```

Compilar o projeto:

```bash
idf.py build
```

Gravar na placa:

```bash
idf.py -p COM3 flash
```

Abrir o monitor serial:

```bash
idf.py monitor
```

Ou realizar gravação e monitoramento em uma única etapa:

```bash
idf.py -p COM3 flash monitor
```

---

## Resultados Obtidos

Os testes realizados permitiram validar:

- criação e execução concorrente das tarefas;
- comunicação entre tarefas utilizando filas;
- sincronização por semáforos contadores;
- tratamento de interrupções provenientes dos botões;
- controle simultâneo de três carregadores;
- funcionamento da política de prioridades;
- cálculo dinâmico do tempo de carregamento;
- integração entre software e hardware utilizando a ESP32.

---
## Vídeo de Demonstração de Funcionamento

- [Vídeo de demonstração](https://youtu.be/d-rbOEdhFGM)

---

## Autores

**Luiz Gustavo de Lima Rodrigues;**
**Gildemberg Barbosa Santos;**
**Vitória Jamille Gonçalves de Oliveira Nóbrega**


Universidade Federal de Campina Grande

Curso de Engenharia Elétrica

Disciplina: Sistemas em Tempo Real

---

