#ifndef EQUIPAMENTOS_H
#define EQUIPAMENTOS_H

/**
 * @file equipamentos.h
 * @brief Gestão da entidade Equipamento no Sistema de Gestão de Incêndios.
 *
 * Este módulo é responsável por todas as operações CRUD (Create, Read, Update, Delete)
 * relacionadas com equipamentos que podem ser alocados a intervenções.
 *
 * @author José (8220942)
 * @date 2024
 * @version 2.0
 */

#include "tipos_comuns.h"
#include <stddef.h>


/* ========================================================================= */
/*  ENUMERAÇÕES ESPECÍFICAS DE EQUIPAMENTOS                                 */
/* ========================================================================= */

/**
 * @brief Tipos de equipamento disponíveis no sistema.
 */
typedef enum {
    VEICULO,      /**< Viaturas (ambulâncias, autotanques, etc.)    */
    MANGUEIRA,    /**< Mangueiras de combate a incêndio             */
    RESPIRADOR    /**< Equipamento de proteção respiratória         */
} TipoEquipamento;

/**
 * @brief Estado operacional de um equipamento.
 */
typedef enum {
    EQ_DISPONIVEL,      /**< Disponível para alocação a uma intervenção */
    EQ_EM_USO,          /**< Atualmente em uso numa intervenção ativa   */
    EQ_EM_MANUTENCAO    /**< Em manutenção ou reparação                 */
} EstadoEquipamento;


/* ========================================================================= */
/*  ESTRUTURA DA ENTIDADE EQUIPAMENTO                                       */
/* ========================================================================= */

/**
 * @brief Entidade que representa um equipamento no sistema.
 *
 * O campo `ativo` implementa soft delete, preservando integridade
 * referencial com intervenções históricas.
 */
typedef struct {
    int idEquipamento;               /**< Identificador único (auto-increment) */
    char designacao[MAX_DESIGNACAO]; /**< Nome ou descrição do equipamento     */
    TipoEquipamento tipo;            /**< Categoria do equipamento             */
    EstadoEquipamento estado;        /**< Estado operacional atual             */
    char localizacao[MAX_LOCAL];     /**< Localização física do equipamento    */
    int ativo;                       /**< 1=ativo, 0=inativo (soft delete)     */
} Equipamento;


/* ========================================================================= */
/*  LISTA DINÂMICA DE EQUIPAMENTOS                                          */
/* ========================================================================= */

/**
 * @brief Lista dinâmica que armazena todos os equipamentos do sistema.
 */
typedef struct {
    Equipamento *dados;   /**< Array dinâmico de equipamentos              */
    int tamanho;          /**< Número de equipamentos atualmente guardados */
    int capacidade;       /**< Capacidade máxima atual do array            */
} ListaEquipamentos;


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
 * @brief Cria um novo equipamento e adiciona-o ao sistema.
 *
 * Solicita interativamente: designação, localização e tipo.
 * O estado inicial é sempre EQ_DISPONIVEL.
 *
 * @param sistema Ponteiro para o contexto global do sistema
 * @return ID do equipamento criado, ou -1 em caso de erro
 */
int criarEquipamento(SistemaGestaoIncendios *sistema);

/**
 * @brief Lista todos os equipamentos ativos no sistema.
 *
 * @param sistema Ponteiro para o contexto global (acesso read-only)
 */
void listarEquipamentos(const SistemaGestaoIncendios *sistema);

/**
 * @brief Atualiza os dados de um equipamento existente.
 *
 * Permite modificar: designação, localização, tipo e estado.
 *
 * @param sistema Ponteiro para o contexto global do sistema
 *
 * @warning Se o equipamento estiver em intervenção EM_EXECUCAO, não pode
 *          alterar o estado operacional
 */
void atualizarEquipamento(SistemaGestaoIncendios *sistema);

/**
 * @brief Inativa logicamente um equipamento (soft delete).
 *
 * @param sistema Ponteiro para o contexto global do sistema
 *
 * @warning Não permite inativar equipamentos em intervenções EM_EXECUCAO
 * @see tentarEliminarEquipamento()
 */
void inativarEquipamento(SistemaGestaoIncendios *sistema);

/**
 * @brief Reativa um equipamento previamente inativado.
 *
 * @param sistema Ponteiro para o contexto global do sistema
 */
void reativarEquipamento(SistemaGestaoIncendios *sistema);


/* ========================================================================= */
/*  FUNÇÕES DE PESQUISA                                                     */
/* ========================================================================= */

/**
 * @brief Procura um equipamento pelo seu ID único.
 *
 * @param sistema Ponteiro para o contexto global (acesso read-only)
 * @param id ID do equipamento a procurar
 * @return Ponteiro para o equipamento encontrado, ou NULL se não existir
 *
 * @note Retorna o equipamento mesmo que esteja inativo (ativo == 0)
 */
Equipamento *procurarEquipamentoPorId(const SistemaGestaoIncendios *sistema, int id);

/**
 * @brief Tenta eliminar fisicamente um equipamento da memória.
 *
 * A eliminação só ocorre se o equipamento estiver inativo e sem relações
 * com nenhuma intervenção.
 *
 * @param sistema        Ponteiro para o contexto global do sistema
 * @param idEquipamento  ID do equipamento a tentar eliminar
 *
 * @note Chamada automaticamente por inativarEquipamento()
 */
void tentarEliminarEquipamento(SistemaGestaoIncendios *sistema, int idEquipamento);


/* ========================================================================= */
/*  FUNÇÕES AUXILIARES (CONVERSÕES E MENUS INTERATIVOS)                    */
/* ========================================================================= */

/**
 * @brief Converte um tipo de equipamento para string legível.
 *
 * @param tipo Tipo a converter
 * @return String descritiva; "Desconhecido" se inválido
 */
const char *tipoEquipamentoParaString(TipoEquipamento tipo);

/**
 * @brief Converte um estado de equipamento para string legível.
 *
 * @param estado Estado a converter
 * @return String descritiva; "Desconhecido" se inválido
 */
const char *estadoEquipamentoParaString(EstadoEquipamento estado);

/**
 * @brief Apresenta menu interativo e solicita escolha de tipo de equipamento.
 *
 * @return Tipo de equipamento escolhido pelo utilizador
 */
TipoEquipamento escolherTipoEquipamento(void);

/**
 * @brief Apresenta menu interativo e solicita escolha de estado operacional.
 *
 * @return Estado operacional escolhido pelo utilizador
 */
EstadoEquipamento escolherEstadoEquipamento(void);

/**
 * @brief Expande a lista de equipamentos se a capacidade estiver esgotada.
 *
 * @param lista Ponteiro para a lista a expandir
 * @return 1 se bem-sucedido (ou expansão desnecessária), 0 se falha de memória
 */
int expandirListaEquipamentosSeNecessario(ListaEquipamentos *lista);


#endif /* EQUIPAMENTOS_H */