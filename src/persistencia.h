#ifndef PERSISTENCIA_H
#define PERSISTENCIA_H

/**
 * @file persistencia.h
 * @brief Funções para persistência de dados em ficheiros binários.
 *
 * Este módulo é responsável por guardar e carregar o estado completo
 * do sistema em ficheiros binários, garantindo que os dados sobrevivem
 * entre sessões da aplicação.
 *
 * Cada entidade é persistida num ficheiro separado para permitir
 * carregamento e gravação independentes.
 *
 * @author José (8220942)
 * @date 2024
 * @version 2.0
 */

#include "modelos.h"


/* ========================================================================= */
/*  NOMES DOS FICHEIROS DE PERSISTÊNCIA                                     */
/* ========================================================================= */

#define FICHEIRO_OCORRENCIAS   "ocorrencias.bin"   /**< Ficheiro de ocorrências  */
#define FICHEIRO_BOMBEIROS     "bombeiros.bin"      /**< Ficheiro de bombeiros    */
#define FICHEIRO_EQUIPAMENTOS  "equipamentos.bin"   /**< Ficheiro de equipamentos */
#define FICHEIRO_INTERVENCOES  "intervencoes.bin"   /**< Ficheiro de intervenções */


/* ========================================================================= */
/*  OPERAÇÕES GLOBAIS                                                        */
/* ========================================================================= */

/**
 * @brief Carrega todos os dados do sistema a partir dos ficheiros binários.
 *
 * Invoca sequencialmente as funções de carregamento de cada entidade.
 * Se um ficheiro não existir, a lista correspondente permanece vazia.
 *
 * @param sistema Ponteiro para o contexto global do sistema
 *
 * @note Deve ser chamada após inicializarSistema() e antes de qualquer
 *       operação sobre os dados
 * @note Atualiza os contadores de ID com base no maior ID encontrado
 */
void carregarDados(SistemaGestaoIncendios *sistema);

/**
 * @brief Guarda todos os dados do sistema nos ficheiros binários.
 *
 * Invoca sequencialmente as funções de gravação de cada entidade.
 * Sobrescreve os ficheiros existentes.
 *
 * @param sistema Ponteiro constante para o contexto global do sistema
 *
 * @note Deve ser chamada antes de terminar a aplicação e após
 *       qualquer operação que altere os dados
 */
void guardarDados(const SistemaGestaoIncendios *sistema);


/* ========================================================================= */
/*  CARREGAMENTO INDIVIDUAL                                                  */
/* ========================================================================= */

/**
 * @brief Carrega as ocorrências a partir do ficheiro binário.
 *
 * @param sistema Ponteiro para o contexto global do sistema
 *
 * @note Se o ficheiro não existir, a operação é silenciosamente ignorada
 */
void carregarOcorrencias(SistemaGestaoIncendios *sistema);

/**
 * @brief Carrega as intervenções a partir do ficheiro binário.
 *
 * Reconstrói também os arrays dinâmicos internos de cada intervenção
 * (idsBombeiros e idsEquipamentos).
 *
 * @param sistema Ponteiro para o contexto global do sistema
 *
 * @warning Deve ser chamada após carregarBombeiros() e carregarEquipamentos()
 *          para garantir integridade referencial
 */
void carregarIntervencoes(SistemaGestaoIncendios *sistema);

/**
 * @brief Carrega os bombeiros a partir do ficheiro binário.
 *
 * @param sistema Ponteiro para o contexto global do sistema
 */
void carregarBombeiros(SistemaGestaoIncendios *sistema);

/**
 * @brief Carrega os equipamentos a partir do ficheiro binário.
 *
 * @param sistema Ponteiro para o contexto global do sistema
 */
void carregarEquipamentos(SistemaGestaoIncendios *sistema);


/* ========================================================================= */
/*  GRAVAÇÃO INDIVIDUAL                                                      */
/* ========================================================================= */

/**
 * @brief Guarda as ocorrências no ficheiro binário respetivo.
 *
 * @param sistema Ponteiro constante para o contexto global do sistema
 */
void guardarOcorrencias(const SistemaGestaoIncendios *sistema);

/**
 * @brief Guarda as intervenções no ficheiro binário respetivo.
 *
 * Serializa também os arrays dinâmicos internos de cada intervenção.
 *
 * @param sistema Ponteiro constante para o contexto global do sistema
 *
 * @note O formato inclui o número de elementos antes de cada array,
 *       permitindo reconstrução correta no carregamento
 */
void guardarIntervencoes(const SistemaGestaoIncendios *sistema);

/**
 * @brief Guarda os bombeiros no ficheiro binário respetivo.
 *
 * @param sistema Ponteiro constante para o contexto global do sistema
 */
void guardarBombeiros(const SistemaGestaoIncendios *sistema);

/**
 * @brief Guarda os equipamentos no ficheiro binário respetivo.
 *
 * @param sistema Ponteiro constante para o contexto global do sistema
 */
void guardarEquipamentos(const SistemaGestaoIncendios *sistema);


#endif /* PERSISTENCIA_H */