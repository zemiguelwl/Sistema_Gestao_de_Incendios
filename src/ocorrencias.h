#ifndef OCORRENCIAS_H
#define OCORRENCIAS_H

/**
 * @file ocorrencias.h
 * @brief Gestão da entidade Ocorrencia no Sistema de Gestão de Incêndios.
 *
 * Este módulo é responsável por todas as operações CRUD (Create, Read, Update, Delete)
 * relacionadas com ocorrências de incêndio registadas no sistema.
 *
 * @author José (8220942)
 * @date 2024
 * @version 2.0
 */

#include "tipos_comuns.h"
#include <stddef.h>


/* ========================================================================= */
/*  ENUMERAÇÕES ESPECÍFICAS DE OCORRÊNCIAS                                  */
/* ========================================================================= */

/**
 * @brief Tipos possíveis de ocorrência.
 */
typedef enum {
    FLORESTAL,    /**< Incêndio em zona florestal ou rural       */
    URBANO,       /**< Incêndio em zona urbana ou habitacional   */
    INDUSTRIAL    /**< Incêndio em instalação industrial         */
} TipoOcorrencia;

/**
 * @brief Prioridade atribuída a uma ocorrência.
 */
typedef enum {
    BAIXA,    /**< Ocorrência de baixo risco ou dimensão reduzida  */
    NORMAL,   /**< Ocorrência de risco moderado                    */
    ALTA      /**< Ocorrência crítica — resposta imediata          */
} PrioridadeOcorrencia;

/**
 * @brief Estado atual de uma ocorrência no seu ciclo de vida.
 *
 * Transições válidas:
 *   REPORTADA → EM_INTERVENCAO → CONCLUIDA
 */
typedef enum {
    REPORTADA,         /**< Registada, ainda sem intervenção associada */
    EM_INTERVENCAO,    /**< Com pelo menos uma intervenção ativa        */
    CONCLUIDA          /**< Totalmente resolvida                        */
} EstadoOcorrencia;


/* ========================================================================= */
/*  ESTRUTURA DA ENTIDADE OCORRÊNCIA                                        */
/* ========================================================================= */

/**
 * @brief Entidade que representa uma ocorrência de incêndio no sistema.
 *
 * O campo `ativo` implementa soft delete, preservando integridade
 * referencial com intervenções históricas.
 */
typedef struct {
    int idOcorrencia;                /**< Identificador único (auto-increment)  */
    char localizacao[MAX_LOCAL];     /**< Localização geográfica da ocorrência  */
    DataHora dataHora;               /**< Data e hora do registo                */
    TipoOcorrencia tipo;             /**< Tipo de ambiente da ocorrência        */
    PrioridadeOcorrencia prioridade; /**< Nível de prioridade de resposta       */
    EstadoOcorrencia estado;         /**< Estado atual no ciclo de vida         */
    int ativo;                       /**< 1=ativo, 0=inativo (soft delete)      */
} Ocorrencia;


/* ========================================================================= */
/*  LISTA DINÂMICA DE OCORRÊNCIAS                                           */
/* ========================================================================= */

/**
 * @brief Lista dinâmica que armazena todas as ocorrências do sistema.
 */
typedef struct {
    Ocorrencia *dados;   /**< Array dinâmico de ocorrências              */
    int tamanho;         /**< Número de ocorrências atualmente guardadas */
    int capacidade;      /**< Capacidade máxima atual do array           */
} ListaOcorrencias;


/* ========================================================================= */
/*  DECLARAÇÃO FORWARD DO CONTEXTO GLOBAL                                   */
/* ========================================================================= */

/*
 * Declaração antecipada para evitar dependência circular com sgi.h.
 * A definição completa está em sgi.h.
 */
typedef struct SistemaGestaoIncendios SistemaGestaoIncendios;


/* ========================================================================= */
/*  FUNÇÕES DE GESTÃO (CRUD)                                                */
/* ========================================================================= */

/**
 * @brief Cria uma nova ocorrência e adiciona-a ao sistema.
 *
 * Solicita interativamente: localização, data/hora, tipo e prioridade.
 * O estado inicial é sempre REPORTADA.
 *
 * @param sistema Ponteiro para o contexto global do sistema
 * @return ID da ocorrência criada, ou -1 em caso de erro
 */
int criarOcorrencia(SistemaGestaoIncendios *sistema);

/**
 * @brief Lista todas as ocorrências ativas no sistema.
 *
 * @param sistema Ponteiro para o contexto global (acesso read-only)
 */
void listarOcorrencias(const SistemaGestaoIncendios *sistema);

/**
 * @brief Atualiza os dados de uma ocorrência existente.
 *
 * Permite modificar: localização, tipo, prioridade e estado.
 *
 * @param sistema Ponteiro para o contexto global do sistema
 *
 * @warning Não permite alterar ocorrências com intervenção em EM_EXECUCAO
 */
void atualizarOcorrencia(SistemaGestaoIncendios *sistema);

/**
 * @brief Inativa logicamente uma ocorrência (soft delete).
 *
 * @param sistema Ponteiro para o contexto global do sistema
 *
 * @warning Não permite inativar ocorrências com intervenção em EM_EXECUCAO
 * @see tentarEliminarOcorrencia()
 */
void inativarOcorrencia(SistemaGestaoIncendios *sistema);

/**
 * @brief Reativa uma ocorrência previamente inativada.
 *
 * @param sistema Ponteiro para o contexto global do sistema
 */
void reativarOcorrencia(SistemaGestaoIncendios *sistema);


/* ========================================================================= */
/*  FUNÇÕES DE PESQUISA                                                     */
/* ========================================================================= */

/**
 * @brief Procura uma ocorrência pelo seu ID único.
 *
 * @param sistema Ponteiro para o contexto global (acesso read-only)
 * @param id ID da ocorrência a procurar
 * @return Ponteiro para a ocorrência encontrada, ou NULL se não existir
 *
 * @note Retorna a ocorrência mesmo que esteja inativa (ativo == 0)
 */
Ocorrencia *procurarOcorrenciaPorId(const SistemaGestaoIncendios *sistema, int id);

/**
 * @brief Tenta eliminar fisicamente uma ocorrência da memória.
 *
 * A eliminação só ocorre se a ocorrência estiver inativa, no estado
 * REPORTADA e sem intervenções associadas.
 *
 * @param sistema       Ponteiro para o contexto global do sistema
 * @param idOcorrencia  ID da ocorrência a tentar eliminar
 *
 * @note Chamada automaticamente por inativarOcorrencia()
 */
void tentarEliminarOcorrencia(SistemaGestaoIncendios *sistema, int idOcorrencia);


/* ========================================================================= */
/*  FUNÇÕES AUXILIARES (CONVERSÕES E MENUS INTERATIVOS)                    */
/* ========================================================================= */

/**
 * @brief Converte um tipo de ocorrência para string legível.
 *
 * @param tipo Tipo a converter
 * @return String descritiva; "Desconhecido" se inválido
 */
const char *tipoOcorrenciaParaString(TipoOcorrencia tipo);

/**
 * @brief Converte uma prioridade para string legível.
 *
 * @param prioridade Prioridade a converter
 * @return String descritiva; "Desconhecida" se inválida
 *
 * @note Também acessível como prioridadeParaString() em utils.h
 *       para uso noutros módulos (ex.: intervencoes.c)
 */
const char *prioridadeOcorrenciaParaString(PrioridadeOcorrencia prioridade);

/**
 * @brief Converte um estado de ocorrência para string legível.
 *
 * @param estado Estado a converter
 * @return String descritiva; "Desconhecido" se inválido
 */
const char *estadoOcorrenciaParaString(EstadoOcorrencia estado);

/**
 * @brief Apresenta menu interativo e solicita escolha de tipo de ocorrência.
 *
 * @return Tipo de ocorrência escolhido pelo utilizador
 */
TipoOcorrencia escolherTipoOcorrencia(void);

/**
 * @brief Apresenta menu interativo e solicita escolha de prioridade.
 *
 * @return Prioridade escolhida pelo utilizador
 */
PrioridadeOcorrencia escolherPrioridadeOcorrencia(void);

/**
 * @brief Apresenta menu interativo e solicita escolha de estado de ocorrência.
 *
 * @return Estado escolhido pelo utilizador
 */
EstadoOcorrencia escolherEstadoOcorrencia(void);

/**
 * @brief Expande a lista de ocorrências se a capacidade estiver esgotada.
 *
 * @param lista Ponteiro para a lista a expandir
 * @return 1 se bem-sucedido (ou expansão desnecessária), 0 se falha de memória
 */
int expandirListaOcorrenciasSeNecessario(ListaOcorrencias *lista);


#endif /* OCORRENCIAS_H */