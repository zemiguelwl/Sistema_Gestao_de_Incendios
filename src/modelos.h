#ifndef MODELOS_H
#define MODELOS_H

/**
 * @file modelos.h
 * @brief Estruturas de dados, enumerações e constantes principais do
 * Sistema de Gestão de Incêndios (SGI).
 *
 * Este ficheiro define todos os modelos utilizados pelo sistema,
 * incluindo entidades (Ocorrencia, Intervencao, Bombeiro, Equipamento),
 * listas dinâmicas e o contexto global da aplicação.
 */


/*  CONSTANTES GERAIS                                        */

#define MAX_LOCAL        100   /**< Tamanho máximo para nomes de localidades */
#define MAX_NOME         100   /**< Tamanho máximo para nomes de bombeiros */
#define MAX_DESIGNACAO   100   /**< Tamanho máximo para designações de equipamentos */
#define MAX_LINHA        500   /**< Tamanho para leitura de linhas */

#define CAPACIDADE_INICIAL 10  /**< Capacidade inicial das listas dinâmicas */

#define DATA_NAO_DEFINIDA -1   /**< Valor especial para datas não definidas */

/* --------------------------------------------------------- */
/*  ENUMERAÇÕES PRINCIPAIS                                   */

/**
 * @brief Tipos possíveis de ocorrência.
 */
typedef enum {
    FLORESTAL,
    URBANO,
    INDUSTRIAL
} TipoOcorrencia;

/**
 * @brief Prioridade atribuída a uma ocorrência.
 */
typedef enum {
    BAIXA,
    NORMAL,
    ALTA
} PrioridadeOcorrencia;

/**
 * @brief Estado de uma ocorrência.
 */
typedef enum {
    REPORTADA,
    EM_INTERVENCAO,
    CONCLUIDA
} EstadoOcorrencia;

/**
 * @brief Estado operacional de uma intervenção.
 */
typedef enum {
    EM_PLANEAMENTO,
    EM_EXECUCAO,
    INT_CONCLUIDA
} EstadoIntervencao;

/**
 * @brief Especialidades dos bombeiros.
 */
typedef enum {
    COMBATE_FLORESTAL,
    RESGATE,
    INCENDIO
} EspecialidadeBombeiro;

/**
 * @brief Estado operacional de um bombeiro.
 */
typedef enum {
    DISPONIVEL,
    EM_INTERVENCAO_BOMBEIRO,
    EM_TREINO
} EstadoOperacionalBombeiro;

/**
 * @brief Tipos possíveis de equipamento.
 */
typedef enum {
    VEICULO,
    MANGUEIRA,
    RESPIRADOR
} TipoEquipamento;

/**
 * @brief Estado de um equipamento.
 */
typedef enum {
    EQ_DISPONIVEL,
    EQ_EM_USO,
    EQ_EM_MANUTENCAO
} EstadoEquipamento;

/* --------------------------------------------------------- */
/*  ESTRUTURAS BASE                                           */


/**
 * @brief Representa uma data e hora completas.
 */
typedef struct {
    int dia;
    int mes;
    int ano;
    int hora;
    int minuto;
} DataHora;

/* --------------------------------------------------------- */
/*  ENTIDADES                                                */


/**
 * @brief Entidade Ocorrencia.
 */
typedef struct {
    int idOcorrencia;
    char localizacao[MAX_LOCAL];
    DataHora dataHora;
    TipoOcorrencia tipo;
    PrioridadeOcorrencia prioridade;
    EstadoOcorrencia estado;
    int ativo; /**< 1 ativo, 0 inativo  */
} Ocorrencia;

/**
 * @brief Entidade Intervencao.
 */
typedef struct {
    int idIntervencao;
    int idOcorrencia;

    DataHora inicio;
    DataHora fim;
    int fimDefinido;

    EstadoIntervencao estado;

    /* Bombeiros associados */
    int *idsBombeiros;
    int numBombeiros;
    int capacidadeBombeiros;

    /* Equipamentos associados */
    int *idsEquipamentos;
    int numEquipamentos;
    int capacidadeEquipamentos;

    int ativo; /**< 1 ativo, 0 inativo */
} Intervencao;

/**
 * @brief Entidade Bombeiro.
 */
typedef struct {
    int idBombeiro;
    char nome[MAX_NOME];
    EspecialidadeBombeiro especialidade;
    EstadoOperacionalBombeiro estado;
    int ativo;
} Bombeiro;

/**
 * @brief Entidade Equipamento.
 */
typedef struct {
    int idEquipamento;
    char designacao[MAX_DESIGNACAO];
    TipoEquipamento tipo;
    EstadoEquipamento estado;
    char localizacao[MAX_LOCAL];
    int ativo;
} Equipamento;

/* --------------------------------------------------------- */
/*  LISTAS DINÂMICAS                                          */


/**
 * @brief Lista dinâmica de ocorrências.
 */
typedef struct {
    Ocorrencia *dados;
    int tamanho;
    int capacidade;
} ListaOcorrencias;

/**
 * @brief Lista dinâmica de intervenções.
 */
typedef struct {
    Intervencao *dados;
    int tamanho;
    int capacidade;
} ListaIntervencoes;

/**
 * @brief Lista dinâmica de bombeiros.
 */
typedef struct {
    Bombeiro *dados;
    int tamanho;
    int capacidade;
} ListaBombeiros;

/**
 * @brief Lista dinâmica de equipamentos.
 */
typedef struct {
    Equipamento *dados;
    int tamanho;
    int capacidade;
} ListaEquipamentos;

/* --------------------------------------------------------- */
/*  CONTEXTO GLOBAL                               */


/**
 * @brief Estrutura principal que agrega todas as listas
 * e contadores de IDs do sistema.
 */
typedef struct {
    ListaOcorrencias ocorrencias;
    ListaIntervencoes intervencoes;
    ListaBombeiros bombeiros;
    ListaEquipamentos equipamentos;

    int proximoIdOcorrencia;
    int proximoIdIntervencao;
    int proximoIdBombeiro;
    int proximoIdEquipamento;
} SistemaGestaoIncendios;

#endif /* MODELOS_H */
