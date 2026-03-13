#ifndef INTERVENCOES_H
#define INTERVENCOES_H

/**
 * @file intervencoes.h
 * @brief Gestão da entidade Intervenção no Sistema de Gestão de Incêndios.
 *
 * Este módulo é responsável por todas as operações CRUD (Create, Read, Update, Delete)
 * relacionadas com intervenções, incluindo associação de bombeiros e equipamentos,
 * gestão de estados e coordenação com ocorrências.
 *
 * @author José (8220942)
 * @date 2024
 * @version 2.0
 */

#include "tipos_comuns.h"
#include <stddef.h>


/* ========================================================================= */
/*  ENUMERAÇÕES ESPECÍFICAS DE INTERVENÇÕES                                 */
/* ========================================================================= */

/**
 * @brief Estado operacional de uma intervenção no seu ciclo de vida.
 *
 * Transições válidas:
 *   EM_PLANEAMENTO → EM_EXECUCAO → INT_CONCLUIDA
 *
 * @note Uma intervenção em EM_EXECUCAO bloqueia alterações em bombeiros
 *       e equipamentos que lhe estejam associados.
 */
typedef enum {
    EM_PLANEAMENTO,   /**< Em preparação — recursos ainda podem ser alterados */
    EM_EXECUCAO,      /**< Operação ativa no terreno                          */
    INT_CONCLUIDA     /**< Intervenção terminada                              */
} EstadoIntervencao;


/* ========================================================================= */
/*  ESTRUTURA DA ENTIDADE INTERVENÇÃO                                       */
/* ========================================================================= */

/**
 * @brief Entidade que representa uma intervenção no sistema.
 *
 * Cada intervenção está associada a uma ocorrência e agrega dinamicamente
 * listas de bombeiros e equipamentos alocados. O campo `ativo` implementa
 * soft delete, preservando histórico de operações.
 *
 * @note Os arrays `idsBombeiros` e `idsEquipamentos` são alocados dinamicamente
 *       e crescem automaticamente à medida que recursos são adicionados.
 *       Devem ser libertados com free() quando a intervenção for eliminada.
 */
typedef struct {
    int idIntervencao;   /**< Identificador único (auto-increment)            */
    int idOcorrencia;    /**< Ocorrência à qual esta intervenção responde     */

    DataHora inicio;     /**< Data e hora de início da intervenção            */
    DataHora fim;        /**< Data e hora de conclusão (se fimDefinido == 1)  */
    int fimDefinido;     /**< 1=fim registado, 0=ainda em curso               */

    EstadoIntervencao estado; /**< Estado atual no ciclo de vida              */

    /* --- Bombeiros alocados --- */
    int *idsBombeiros;         /**< Array dinâmico de IDs de bombeiros        */
    int numBombeiros;          /**< Número de bombeiros atualmente alocados   */
    int capacidadeBombeiros;   /**< Capacidade atual do array de bombeiros    */

    /* --- Equipamentos alocados --- */
    int *idsEquipamentos;       /**< Array dinâmico de IDs de equipamentos    */
    int numEquipamentos;        /**< Número de equipamentos alocados          */
    int capacidadeEquipamentos; /**< Capacidade atual do array de equipamentos*/

    int ativo;           /**< 1=ativo, 0=inativo (soft delete)               */
} Intervencao;


/* ========================================================================= */
/*  LISTA DINÂMICA DE INTERVENÇÕES                                          */
/* ========================================================================= */

/**
 * @brief Lista dinâmica que armazena todas as intervenções do sistema.
 */
typedef struct {
    Intervencao *dados;   /**< Array dinâmico de intervenções              */
    int tamanho;          /**< Número de intervenções atualmente guardadas */
    int capacidade;       /**< Capacidade máxima atual do array            */
} ListaIntervencoes;


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
 * @brief Cria uma nova intervenção associada a uma ocorrência.
 *
 * Solicita interativamente: ID da ocorrência e data/hora de início.
 * Invoca adicionarBombeirosIntervencao() e adicionarEquipamentosIntervencao()
 * durante a criação. O estado inicial é sempre EM_PLANEAMENTO.
 *
 * @param sistema Ponteiro para o contexto global do sistema
 * @return ID da intervenção criada, ou -1 em caso de erro
 *
 * @note Apenas ocorrências com estado REPORTADA podem ter novas intervenções
 */
int criarIntervencao(SistemaGestaoIncendios *sistema);

/**
 * @brief Lista todas as intervenções ativas no sistema.
 *
 * Apresenta ID, estado, ocorrência, datas e recursos alocados.
 *
 * @param sistema Ponteiro para o contexto global (acesso read-only)
 */
void listarIntervencoes(const SistemaGestaoIncendios *sistema);

/**
 * @brief Inicia a execução de uma intervenção em planeamento.
 *
 * Transição: EM_PLANEAMENTO → EM_EXECUCAO
 * Altera estado dos bombeiros para EM_INTERVENCAO_BOMBEIRO,
 * equipamentos para EQ_EM_USO e ocorrência para EM_INTERVENCAO.
 *
 * @param sistema Ponteiro para o contexto global do sistema
 *
 * @warning Requer pelo menos 1 bombeiro alocado
 * @warning Valida que todos os recursos ainda estão disponíveis
 */
void iniciarExecucaoIntervencao(SistemaGestaoIncendios *sistema);

/**
 * @brief Conclui uma intervenção em execução, registando data/hora de fim.
 *
 * Transição: EM_EXECUCAO → INT_CONCLUIDA
 * Liberta bombeiros (→ DISPONIVEL), equipamentos (→ EQ_DISPONIVEL)
 * e atualiza a ocorrência para CONCLUIDA.
 *
 * @param sistema Ponteiro para o contexto global do sistema
 *
 * @warning Valida que data/hora de fim é posterior ao início
 */
void concluirIntervencao(SistemaGestaoIncendios *sistema);

/**
 * @brief Associa múltiplos bombeiros a uma intervenção em planeamento.
 *
 * Pode ser chamada diretamente do menu (intvExterna == NULL, pede ID)
 * ou internamente durante criarIntervencao() (intvExterna != NULL).
 *
 * @param sistema      Ponteiro para o contexto global do sistema
 * @param intvExterna  Ponteiro para a intervenção alvo, ou NULL para pedir ID
 *
 * @warning Só pode adicionar se estado == EM_PLANEAMENTO
 * @note Apresenta sugestão automática baseada em histórico, se disponível
 */
void adicionarBombeirosIntervencao(SistemaGestaoIncendios *sistema,
                                    Intervencao *intvExterna);

/**
 * @brief Associa múltiplos equipamentos a uma intervenção em planeamento.
 *
 * Pode ser chamada diretamente do menu (intvExterna == NULL, pede ID)
 * ou internamente durante criarIntervencao() (intvExterna != NULL).
 *
 * @param sistema      Ponteiro para o contexto global do sistema
 * @param intvExterna  Ponteiro para a intervenção alvo, ou NULL para pedir ID
 *
 * @warning Só pode adicionar se estado == EM_PLANEAMENTO
 * @note Apresenta sugestão automática baseada em histórico, se disponível
 */
void adicionarEquipamentosIntervencao(SistemaGestaoIncendios *sistema,
                                       Intervencao *intvExterna);

/**
 * @brief Inativa logicamente uma intervenção (soft delete).
 *
 * @param sistema Ponteiro para o contexto global do sistema
 *
 * @warning Não permite inativar intervenções em estado EM_EXECUCAO
 * @see tentarEliminarIntervencao()
 */
void inativarIntervencao(SistemaGestaoIncendios *sistema);

/**
 * @brief Reativa uma intervenção previamente inativada.
 *
 * Estado, bombeiros e equipamentos são mantidos como estavam.
 *
 * @param sistema Ponteiro para o contexto global do sistema
 */
void reativarIntervencao(SistemaGestaoIncendios *sistema);


/* ========================================================================= */
/*  FUNÇÕES DE PESQUISA                                                     */
/* ========================================================================= */

/**
 * @brief Procura uma intervenção pelo seu ID único.
 *
 * @param sistema Ponteiro para o contexto global (acesso read-only)
 * @param id ID da intervenção a procurar
 * @return Ponteiro para a intervenção encontrada, ou NULL se não existir
 *
 * @note Retorna a intervenção mesmo que esteja inativa (ativo == 0)
 */
Intervencao *procurarIntervencaoPorId(const SistemaGestaoIncendios *sistema, int id);

/**
 * @brief Tenta eliminar fisicamente uma intervenção da memória.
 *
 * Só elimina se a intervenção estiver inativa e no estado INT_CONCLUIDA.
 * Liberta sempre os arrays internos (idsBombeiros, idsEquipamentos).
 *
 * @param sistema       Ponteiro para o contexto global do sistema
 * @param idIntervencao ID da intervenção a tentar eliminar
 *
 * @note Chamada automaticamente por inativarIntervencao()
 * @note Utiliza "swap and pop" — eficiente mas não preserva a ordem
 */
void tentarEliminarIntervencao(SistemaGestaoIncendios *sistema, int idIntervencao);


/* ========================================================================= */
/*  FUNÇÕES AUXILIARES                                                      */
/* ========================================================================= */

/**
 * @brief Converte um estado de intervenção para string legível.
 *
 * @param estado Estado a converter
 * @return String descritiva; "Desconhecido" se inválido
 */
const char *estadoIntervencaoParaString(EstadoIntervencao estado);

/**
 * @brief Expande a lista de intervenções se a capacidade estiver esgotada.
 *
 * @param lista Ponteiro para a lista a expandir
 * @return 1 se bem-sucedido (ou expansão desnecessária), 0 se falha de memória
 */
int expandirListaIntervencoesSeNecessario(ListaIntervencoes *lista);

/**
 * @brief Expande o array de IDs de bombeiros de uma intervenção.
 *
 * @param intervencao Ponteiro para a intervenção
 * @return 1 se bem-sucedido, 0 se falha de memória
 */
int expandirArrayBombeirosIntervencao(Intervencao *intervencao);

/**
 * @brief Expande o array de IDs de equipamentos de uma intervenção.
 *
 * @param intervencao Ponteiro para a intervenção
 * @return 1 se bem-sucedido, 0 se falha de memória
 */
int expandirArrayEquipamentosIntervencao(Intervencao *intervencao);


#endif /* INTERVENCOES_H */