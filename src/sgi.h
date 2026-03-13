#ifndef SGI_H
#define SGI_H

/**
 * @file sgi.h
 * @brief Contexto global do Sistema de Gestão de Incêndios (SGI).
 *
 * Define a estrutura principal SistemaGestaoIncendios que agrega
 * todas as listas e contadores de IDs do sistema.
 *
 * Este é o único ficheiro que inclui todos os módulos de entidade.
 * Os ficheiros de implementação (.c) devem incluir este ficheiro
 * (diretamente ou através de modelos.h).
 *
 * Hierarquia de inclusão:
 * @code
 *   modelos.h
 *     └── sgi.h
 *           ├── bombeiros.h   → tipos_comuns.h
 *           ├── equipamentos.h → tipos_comuns.h
 *           ├── ocorrencias.h  → tipos_comuns.h
 *           └── intervencoes.h → tipos_comuns.h
 * @endcode
 *
 * @author José (8220942)
 * @date 2024
 * @version 2.0
 */

#include "bombeiros.h"
#include "equipamentos.h"
#include "ocorrencias.h"
#include "intervencoes.h"


/* ========================================================================= */
/*  CONTEXTO GLOBAL DO SISTEMA                                              */
/* ========================================================================= */

/**
 * @brief Estrutura principal que agrega todas as entidades do sistema.
 *
 * Centraliza as quatro listas dinâmicas e os contadores de IDs.
 * A inicialização é feita em main.c chamando as funções de utils.h.
 *
 * @note Os contadores de ID são monotonicamente crescentes — nunca são
 *       reutilizados, mesmo após eliminação de registos.
 *
 * @note A declaração forward `typedef struct SistemaGestaoIncendios`
 *       existe nos ficheiros de entidade para evitar dependência circular.
 *       A definição completa do struct está aqui.
 */
struct SistemaGestaoIncendios {
    ListaOcorrencias   ocorrencias;   /**< Lista de todas as ocorrências   */
    ListaIntervencoes  intervencoes;  /**< Lista de todas as intervenções  */
    ListaBombeiros     bombeiros;     /**< Lista de todos os bombeiros     */
    ListaEquipamentos  equipamentos;  /**< Lista de todos os equipamentos  */

    int proximoIdOcorrencia;   /**< Próximo ID a atribuir a uma ocorrência  */
    int proximoIdIntervencao;  /**< Próximo ID a atribuir a uma intervenção */
    int proximoIdBombeiro;     /**< Próximo ID a atribuir a um bombeiro     */
    int proximoIdEquipamento;  /**< Próximo ID a atribuir a um equipamento  */
};

/*
 * Definição do typedef que complementa as forward declarations nos módulos.
 * Necessário para que SistemaGestaoIncendios possa ser usado como tipo
 * em todo o código sem repetir a keyword struct.
 */
typedef struct SistemaGestaoIncendios SistemaGestaoIncendios;


#endif /* SGI_H */