# 🔥 Sistema de Gestão de Incêndios (SGI)
> Trabalho Prático — Laboratório de Programação | ESTG · P.PORTO | 2025/2026

---

## 📋 Índice

- [Descrição do Projeto](#-descrição-do-projeto)
- [Funcionalidades](#-funcionalidades)
- [Arquitetura do Sistema](#-arquitetura-do-sistema)
- [Estrutura de Ficheiros](#-estrutura-de-ficheiros)
- [Modelo de Dados](#-modelo-de-dados)
- [Persistência de Dados](#-persistência-de-dados)
- [Sistema de Logs](#-sistema-de-logs)
- [Relatórios Disponíveis](#-relatórios-disponíveis)
- [Funcionalidade Adicional](#-funcionalidade-adicional)
- [Decisões de Design](#-decisões-de-design)
- [Autores](#-autores)

---

## 📌 Descrição do Projeto

O **Sistema de Gestão de Incêndios (SGI)** é um projeto em linguagem **C** desenvolvido no âmbito da unidade curricular de 1º ano da licenciatura, Laboratório de Programação. O sistema destina-se a entidades como **Bombeiros**, permitindo gerir de forma integrada e eficiente:

- Ocorrências de emergência (incêndios florestais, urbanos, industriais)
- Intervenções e alocação de recursos
- Efetivos (bombeiros) e os seus estados operacionais
- Equipamentos e veículos disponíveis

O projeto foi desenvolvido seguindo boas práticas de engenharia de software, nomeadamente **modularização**, **memória dinâmica**, **persistência de dados em ficheiros**, **validação de entradas** e **registo de ações (logs)**.

---

⚠️ Este projeto foi originalmente desenvolvido durante o primeiro ano da licenciatura.

Atualmente está a ser melhorado gradualmente, à medida que aplico e aprofundo boas práticas de engenharia de software.

---
## ✅ Funcionalidades

### Gestão de Ocorrências
- Criar, listar, atualizar e eliminar (soft delete) ocorrências
- Campos: ID sequencial, Localização, Data/Hora, Tipo, Prioridade, Estado
- Tipos suportados: `Florestal`, `Urbano`, `Industrial`
- Prioridades: `Baixa`, `Normal`, `Alta`
- Estados: `Reportada`, `Em Intervenção`, `Concluída`

### Gestão de Intervenções
- Criar e gerir intervenções associadas a ocorrências
- Alocar bombeiros e equipamentos durante o planeamento
- Campos: ID sequencial, ID da Ocorrência, Data/Hora de Início e Fim, Estado
- Estados: `Em Planeamento`, `Em Execução`, `Concluída`

### Gestão de Bombeiros
- Manutenção do cadastro de efetivos operacionais
- Campos: ID sequencial, Nome, Especialidade, Estado Operacional
- Especialidades: `Combate Florestal`, `Resgate`, `Incêndio Urbano`
- Estados: `Disponível`, `Em Intervenção`, `Em Treino`

### Gestão de Equipamentos
- Inventário completo de recursos materiais e veículos
- Campos: ID sequencial, Designação, Tipo, Estado, Localização
- Tipos: `Veículo`, `Mangueira`, `Respirador`
- Estados: `Disponível`, `Em Uso`, `Em Manutenção`

### Eliminação Segura (Soft Delete)
- Entidades com relacionamentos ativos não são removidas fisicamente
- São marcadas como **Inativas** e excluídas das listagens regulares
- Podem ser **reativadas** a qualquer momento

---

## 🏗️ Arquitetura do Sistema

O sistema foi construído segundo os princípios de **Responsabilidade Única (SRP)** e **Modularização por Domínio**:

```
┌─────────────────────────────────────────────────────────────┐
│                          main.c                             │
│          (inicialização, ciclo principal, shutdown)         │
└────────────────────────────┬────────────────────────────────┘
                             │
          ┌──────────────────┼──────────────────┐
          ▼                  ▼                  ▼
     ┌─────────┐       ┌──────────┐       ┌──────────┐
     │  menu   │       │  utils   │       │   logs   │
     └────┬────┘       └──────────┘       └──────────┘
          │
    ┌─────┴──────┬─────────────┬──────────────┐
    ▼            ▼             ▼              ▼
┌──────────┐ ┌──────────┐ ┌──────────┐ ┌────────────┐
│ocorrencias│ │intervencoes│ │bombeiros│ │equipamentos│
└──────────┘ └──────────┘ └──────────┘ └────────────┘
         \         |          |         /
          └────────┴──────────┴────────┘
                         │
              ┌──────────┴──────────┐
              ▼                     ▼
       ┌────────────┐        ┌────────────┐
       │ persistencia│        │ relatorios │
       └────────────┘        └────────────┘
              │                     │
       [ficheiros .dat]       ┌────────────┐
                              │  sugestoes │
                              └────────────┘

  Todos os módulos partilham o contexto global via:
  ┌─────────────────────────────────────────────────────┐
  │  sgi.h  (struct SistemaGestaoIncendios)             │
  │  modelos.h  (wrapper: apenas #include "sgi.h")      │
  │  tipos_comuns.h  (DataHora, constantes base)        │
  └─────────────────────────────────────────────────────┘
```

---

## 📁 Estrutura de Ficheiros

```
SGI/
│
├── cmake-build-debug/          # Binários gerados pelo CMake (não versionar)
│
├── src/
│   ├── main.c                      # Ponto de entrada da aplicação
│   │
│   ├── tipos_comuns.h              # Constantes e struct DataHora (base de tudo)
│   ├── sgi.h                       # Struct SistemaGestaoIncendios (contexto global)
│   ├── modelos.h                   # Wrapper de compatibilidade: #include "sgi.h"
│   │
│   ├── menu.c / menu.h             # Interface de utilizador e navegação
│   ├── utils.c / utils.h           # Funções auxiliares (input, datas, conversões)
│   ├── logs.c / logs.h             # Sistema de registo de ações e erros
│   │
│   ├── ocorrencias.c / ocorrencias.h     # CRUD e lógica de ocorrências
│   ├── intervencoes.c / intervencoes.h   # CRUD e lógica de intervenções
│   ├── bombeiros.c / bombeiros.h         # CRUD e lógica de bombeiros
│   ├── equipamentos.c / equipamentos.h   # CRUD e lógica de equipamentos
│   │
│   ├── relatorios.c / relatorios.h       # Todos os relatórios do sistema
│   ├── sugestoes.c / sugestoes.h         # Funcionalidade adicional: Sugestão Automática
│   └── persistencia.c / persistencia.h   # Carregamento e gravação de dados
│
├── CMakeLists.txt              # Configuração de compilação CMake
│
├── data/                       # Criada automaticamente em runtime
│   ├── ocorrencias.dat
│   ├── intervencoes.dat
│   ├── bombeiros.dat
│   └── equipamentos.dat
│
├── logs/
│   └── logs.txt                # Registo de atividade do sistema
│
└── README.md                   # Este ficheiro
```

---

## 🗃️ Modelo de Dados

### Enumerações

```c
/* ocorrencias.h */
typedef enum { FLORESTAL, URBANO, INDUSTRIAL }                    TipoOcorrencia;
typedef enum { BAIXA, NORMAL, ALTA }                              PrioridadeOcorrencia;
typedef enum { REPORTADA, EM_INTERVENCAO, CONCLUIDA }             EstadoOcorrencia;

/* intervencoes.h */
typedef enum { EM_PLANEAMENTO, EM_EXECUCAO, INT_CONCLUIDA }       EstadoIntervencao;

/* bombeiros.h */
typedef enum { COMBATE_FLORESTAL, RESGATE, INCENDIO_URBANO }      EspecialidadeBombeiro;
typedef enum { DISPONIVEL, EM_INTERVENCAO_BOMBEIRO, EM_TREINO }   EstadoOperacionalBombeiro;

/* equipamentos.h */
typedef enum { VEICULO, MANGUEIRA, RESPIRADOR }                   TipoEquipamento;
typedef enum { EQ_DISPONIVEL, EQ_EM_USO, EQ_EM_MANUTENCAO }      EstadoEquipamento;
```

### Estruturas Principais

```c
/* tipos_comuns.h */
typedef struct {
    int dia, mes, ano;
    int hora, minuto;
} DataHora;

/* ocorrencias.h */
typedef struct {
    int                  idOcorrencia;
    char                 localizacao[MAX_LOCAL];
    DataHora             dataHora;
    TipoOcorrencia       tipo;
    PrioridadeOcorrencia prioridade;
    EstadoOcorrencia     estado;
    int                  ativo;   /* soft delete: 1 = ativo, 0 = inativo */
} Ocorrencia;

/* intervencoes.h */
typedef struct {
    int               idIntervencao;
    int               idOcorrencia;
    DataHora          inicio;
    DataHora          fim;
    int               fimDefinido;        /* 1 se a hora de fim foi definida */
    EstadoIntervencao estado;
    int              *idsBombeiros;       /* array dinâmico */
    int               numBombeiros;
    int               capacidadeBombeiros;
    int              *idsEquipamentos;    /* array dinâmico */
    int               numEquipamentos;
    int               capacidadeEquipamentos;
    int               ativo;
} Intervencao;

/* bombeiros.h */
typedef struct {
    int                       idBombeiro;
    char                      nome[MAX_NOME];
    EspecialidadeBombeiro     especialidade;
    EstadoOperacionalBombeiro estado;
    int                       ativo;
} Bombeiro;

/* equipamentos.h */
typedef struct {
    int               idEquipamento;
    char              designacao[MAX_DESIGNACAO];
    TipoEquipamento   tipo;
    EstadoEquipamento estado;
    char              localizacao[MAX_LOCAL];
    int               ativo;
} Equipamento;
```

### Listas Dinâmicas

```c
typedef struct { Ocorrencia  *dados; int tamanho; int capacidade; } ListaOcorrencias;
typedef struct { Intervencao *dados; int tamanho; int capacidade; } ListaIntervencoes;
typedef struct { Bombeiro    *dados; int tamanho; int capacidade; } ListaBombeiros;
typedef struct { Equipamento *dados; int tamanho; int capacidade; } ListaEquipamentos;
```

### Contexto Global

```c
/* sgi.h */
typedef struct SistemaGestaoIncendios {
    ListaOcorrencias   ocorrencias;
    ListaIntervencoes  intervencoes;
    ListaBombeiros     bombeiros;
    ListaEquipamentos  equipamentos;

    int proximoIdOcorrencia;
    int proximoIdIntervencao;
    int proximoIdBombeiro;
    int proximoIdEquipamento;
} SistemaGestaoIncendios;
```

### Constantes

```c
/* tipos_comuns.h */
#define MAX_LOCAL          100   /* Tamanho máximo para nomes de localidades */
#define MAX_NOME           100   /* Tamanho máximo para nomes de bombeiros   */
#define MAX_DESIGNACAO     100   /* Tamanho máximo para designações           */
#define MAX_LINHA          500   /* Tamanho para leitura de linhas            */
#define CAPACIDADE_INICIAL  10   /* Capacidade inicial das listas dinâmicas  */
#define DATA_NAO_DEFINIDA   -1   /* Valor especial para datas não definidas  */
```

---

## 💾 Persistência de Dados

Os dados são guardados em **ficheiros binários** (formato `.dat`), por ser o formato mais eficiente para leitura/escrita direta de structs em C.

| Momento          | Ação                                               |
|------------------|----------------------------------------------------|
| **Arranque**     | Leitura dos ficheiros `.dat` para memória           |
| **Encerramento** | Escrita de todos os dados da memória para ficheiros |

**Ordem de carregamento:** `ocorrencias → bombeiros → equipamentos → intervencoes`
(as intervenções são carregadas por último pois alocam arrays internos que referenciam bombeiros e equipamentos)

**Justificação da escolha binária:**
- Leitura e escrita de structs em bloco com `fread`/`fwrite`
- Performance superior a formatos de texto para grandes volumes
- Menor complexidade de parsing comparativamente a CSV ou JSON

**Nota sobre intervenções:** ao contrário das restantes entidades, as intervenções são serializadas campo a campo (não em bloco) porque contêm arrays dinâmicos internos (`idsBombeiros`, `idsEquipamentos`) que necessitam de tratamento especial.

---

## 📝 Sistema de Logs

O sistema regista automaticamente ações críticas no ficheiro `logs.txt` através da função:

```c
void registarLog(const char *nivel, const char *modulo,
                 const char *acao,  const char *detalhe);
```

**Formato de uma entrada no log:**
```
YYYY-MM-DD HH:MM:SS;NIVEL;MODULO;ACAO;DETALHE
```

**Exemplos reais:**
```
2025-12-01 14:32:05;INFO;INTERVENCOES;CRIAR;ID=5;Ocorrencia=2
2025-12-01 14:35:11;INFO;INTERVENCOES;ADD_BOMBEIRO;Intervencao=5;Bombeiro=3
2025-12-01 14:37:00;INFO;INTERVENCOES;INICIAR_EXECUCAO;Intervencao 5 iniciada (bombeiros=2, equipamentos=1)
2025-12-01 14:40:00;WARNING;INTERVENCOES;INATIVAR;Tentativa bloqueada - intervenção em execução
2025-12-01 14:50:00;ERROR;UTILS;EXPANDIR_LISTA_BOMBEIROS;Falha ao expandir
2025-12-01 14:55:00;INFO;UTILS;FREE_INTERVENCOES;Memória libertada (incluindo arrays internos)
```

**Níveis utilizados:** `INFO` | `WARNING` | `ERROR`

**Ações registadas por módulo:**

| Módulo | Ações registadas |
|--------|-----------------|
| `MAIN` | INICIAR, GUARDAR, ENCERRAR |
| `OCORRENCIAS` | CRIAR, ATUALIZAR, INATIVAR, REATIVAR |
| `INTERVENCOES` | CRIAR, ADD_BOMBEIRO, ADD_EQUIPAMENTO, INICIAR_EXECUCAO, CONCLUIR, INATIVAR, REATIVAR |
| `BOMBEIROS` | CRIAR, ATUALIZAR, INATIVAR, REATIVAR |
| `EQUIPAMENTOS` | CRIAR, ATUALIZAR, INATIVAR, REATIVAR |
| `PERSISTENCIA` | CARREGAR_INICIO, CARREGAR_FIM, GUARDAR_INICIO, GUARDAR_FIM (+ variantes por entidade) |
| `UTILS` | INIT_\*, EXPANDIR_LISTA_\*, FREE_\* |
| `SISTEMA` | INICIAR, ENCERRAR (cabeçalhos de sessão) |

> **Nota:** Operações de leitura (ex.: listar) têm log intencionalmente comentado para evitar ruído excessivo. Podem ser reativados para fins de debugging.

---

## 📊 Relatórios Disponíveis

### Relatórios de Ocorrências
1. `relatorioOcorrenciasPorEstado` — contagem por estado (Reportada / Em Intervenção / Concluída)
2. `relatorioOcorrenciasPorTipo` — distribuição por tipo (Florestal / Urbano / Industrial)
3. `relatorioOcorrenciasPorPrioridade` — contagem por prioridade (Baixa / Normal / Alta)
4. `relatorioTempoRespostaOcorrencias` — diferença em minutos entre a ocorrência e o início da intervenção (média, mín., máx.)
5. `relatorioOcorrenciasPorLocalizacao` — agrupamento e frequência por localidade
6. `relatorioFrequenciaIncidentes` — frequência mensal de incidentes num ano à escolha

### Relatórios de Intervenções
7. `relatorioIntervencoesPorEstado` — contagem por estado
8. `relatorioDuracaoMediaIntervencoes` — duração média, mínima e máxima (em minutos)
9. `relatorioRecursosMaisUtilizados` — bombeiros e equipamentos com maior presença em intervenções
10. `relatorioEficienciaIntervencoes` — duração média e recursos médios cruzados com o tipo de ocorrência

### Relatórios de Bombeiros
11. `relatorioDisponibilidadeBombeiros` — disponibilidade atual por especialidade (com percentagem)
12. `relatorioRankingDesempenhoBombeiros` — ranking por número de intervenções concluídas e tempo médio de resposta

### Relatórios de Equipamentos
13. `relatorioInventarioEquipamentos` — inventário completo com estado atual e resumos por tipo/estado
14. `relatorioEquipamentosEmManutencao` — listagem de equipamentos em manutenção
15. `relatorioUtilizacaoEquipamentosPorTipoIntervencao` — total de equipamentos utilizados por tipo de ocorrência
16. `relatorioRankingEquipamentos` — ranking dos equipamentos mais utilizados em intervenções

### Relatórios Estratégicos Adicionais

**`relatorioCapacidadeOperacional`** *(Relatório EXTRA)*
> Avalia a prontidão operacional global: percentagem de bombeiros e equipamentos disponíveis, carga atual do sistema (intervenções em execução e planeadas, ocorrências pendentes) e nota de prontidão calculada (0–100).

**`relatorioCargaTrabalhoPeriodo`** *(Relatório EXTRA)*
> Análise mensal ao longo de um ano à escolha: ocorrências reportadas, intervenções iniciadas e concluídas por mês — útil para identificar sazonalidade e picos operacionais.

---

## 💡 Funcionalidade Adicional

### Sugestão Automática de Recursos (`sugestoes.c`)

O módulo de sugestões é um **mecanismo de apoio à decisão** baseado em dados históricos de intervenções concluídas. É invocado durante o planeamento de uma nova intervenção e disponibiliza dois cálculos distintos:

**`calcularSugestaoBombeiros`** — devolve a média de bombeiros utilizados em intervenções anteriores com o mesmo tipo de ocorrência e prioridade.

**`calcularSugestaoEquipamentos`** — devolve a média de veículos, mangueiras e respiradores utilizados nas mesmas condições.

**Regras de funcionamento:**
- Exige um **mínimo de 3 intervenções concluídas** com o mesmo tipo e prioridade para produzir uma sugestão (abaixo desse limiar devolve `-1` e o sistema informa o utilizador)
- As sugestões são **apenas consultivas** — não alteram o sistema automaticamente
- O utilizador pode aceitar a informação ou ignorá-la e selecionar recursos manualmente
- Os cálculos baseiam-se exclusivamente em intervenções `INT_CONCLUIDA` e ativas

**Fluxo durante o planeamento:**
```
Criar intervenção
    └─> Adicionar bombeiros
            └─> [Sugestão] "Recomendado: X bombeiros (baseado em Y intervenções semelhantes)"
                           (apenas se Y >= 3; caso contrário: "Dados insuficientes")
    └─> Adicionar equipamentos
            └─> [Sugestão] "Recomendado: V veículos, M mangueiras, R respiradores"
```

**Valor acrescentado:** Apoia decisões de planeamento em situações de pressão, evitando sub ou sobre-alocação de recursos com base em histórico real de operações semelhantes.

---

## 🎯 Decisões de Design

| Decisão | Justificação |
|---------|-------------|
| **Soft delete** (`ativo = 0/1`) | Preserva integridade referencial sem eliminar dados históricos |
| **Eliminação física condicional** (`tentarEliminarIntervencao`) | Intervenções inativas e já concluídas são removidas da memória; não há valor histórico em manter uma intervenção cancelada |
| **Máquina de estados explícita** nas intervenções | Impede transições inválidas (ex.: concluir sem executar, inativar em execução) |
| **Contexto global único** (`SistemaGestaoIncendios`) | Evita variáveis globais dispersas; facilita passagem de estado entre módulos |
| **Arrays dinâmicos** para listas de recursos | Flexibilidade sem limites artificiais; geridos com `malloc`/`realloc`/`free` |
| **Capacidade em duas camadas** nas intervenções | `capacidadeBombeiros` e `capacidadeEquipamentos` independentes — expande apenas o que precisa |
| **Hierarquia de headers modular** (`tipos_comuns.h` → módulos → `sgi.h` → `modelos.h`) | Elimina dependências circulares; cada módulo declara `typedef struct SistemaGestaoIncendios` como forward declaration |
| **Ficheiros binários** para persistência | Maior eficiência de I/O; escrita direta de structs com `fread`/`fwrite` |
| **Serialização campo a campo nas intervenções** | Necessário porque `idsBombeiros` e `idsEquipamentos` são arrays dinâmicos que não podem ser escritos em bloco |
| **IDs sequenciais gerados internamente** | Unicidade garantida; recalculados ao carregar dados (maxId + 1) |
| **Módulo `utils.c`** para funções partilhadas | Conversões enum→string, validações de data, expansão de listas — evita duplicação |
| **Sugestões baseadas em mínimo de 3 amostras** | Garante relevância estatística mínima antes de apresentar sugestão |
| **Logs de leitura comentados** | Evita ruído no ficheiro de log em operações não destrutivas; facilmente reativáveis para debugging |
| **`#ifdef _WIN32`** em torno de `windows.h`** | Mantém portabilidade do código para sistemas não-Windows |

---

## 👥 Autores

- **José Carneiro** 

| Campo            | Informação                                     |
|------------------|------------------------------------------------|
| **Curso**        | Licenciatura em Engenharia Informática         |
| **Instituição**  | ESTG · Politécnico do Porto                   |
| **UC**           | Laboratório de Programação                     |
| **Ano letivo**   | 2025/2026                                      |

---

> *Projeto desenvolvido para fins académicos no âmbito da avaliação em período letivo da UC de Laboratório de Programação.*
