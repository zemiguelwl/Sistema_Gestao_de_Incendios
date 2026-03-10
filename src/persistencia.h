#ifndef PERSISTENCIA_H
#define PERSISTENCIA_H

#include "modelos.h"

/**
 * @file persistencia.h
 * @brief Funções para carregar e guardar dados em ficheiros binários.
 */

/** Nome do ficheiro binário de ocorrências. */
#define FICHEIRO_OCORRENCIAS   "ocorrencias.bin"
/** Nome do ficheiro binário de bombeiros. */
#define FICHEIRO_BOMBEIROS     "bombeiros.bin"
/** Nome do ficheiro binário de equipamentos. */
#define FICHEIRO_EQUIPAMENTOS  "equipamentos.bin"
/** Nome do ficheiro binário de intervenções. */
#define FICHEIRO_INTERVENCOES  "intervencoes.bin"

/**
 * @brief Carrega todos os dados do sistema a partir dos ficheiros binários.
 *
 * Inicializa as listas dinâmicas e preenche-as com os dados persistidos.
 *
 * @param sistema Ponteiro para o contexto global do sistema.
 */
void carregarDados(SistemaGestaoIncendios *sistema);

/**
 * @brief Guarda todos os dados do sistema em ficheiros binários.
 *
 * Percorre todas as listas (ocorrências, intervenções, bombeiros,
 * equipamentos) e grava o seu conteúdo.
 *
 * @param sistema Ponteiro constante para o contexto global do sistema.
 */
void guardarDados(const SistemaGestaoIncendios *sistema);


/**
 * @brief Carrega apenas as ocorrências a partir do ficheiro binário.
 *
 * @param sistema Ponteiro para o contexto global do sistema.
 */
void carregarOcorrencias(SistemaGestaoIncendios *sistema);

/**
 * @brief Carrega apenas as intervenções a partir do ficheiro binário.
 *
 * @param sistema Ponteiro para o contexto global do sistema.
 */
void carregarIntervencoes(SistemaGestaoIncendios *sistema);

/**
 * @brief Carrega apenas os bombeiros a partir do ficheiro binário.
 *
 * @param sistema Ponteiro para o contexto global do sistema.
 */
void carregarBombeiros(SistemaGestaoIncendios *sistema);

/**
 * @brief Carrega apenas os equipamentos a partir do ficheiro binário.
 *
 * @param sistema Ponteiro para o contexto global do sistema.
 */
void carregarEquipamentos(SistemaGestaoIncendios *sistema);


/**
 * @brief Guarda apenas as ocorrências no ficheiro binário respetivo.
 *
 * @param sistema Ponteiro constante para o contexto global do sistema.
 */
void guardarOcorrencias(const SistemaGestaoIncendios *sistema);

/**
 * @brief Guarda apenas as intervenções no ficheiro binário respetivo.
 *
 * @param sistema Ponteiro constante para o contexto global do sistema.
 */
void guardarIntervencoes(const SistemaGestaoIncendios *sistema);

/**
 * @brief Guarda apenas os bombeiros no ficheiro binário respetivo.
 *
 * @param sistema Ponteiro constante para o contexto global do sistema.
 */
void guardarBombeiros(const SistemaGestaoIncendios *sistema);

/**
 * @brief Guarda apenas os equipamentos no ficheiro binário respetivo.
 *
 * @param sistema Ponteiro constante para o contexto global do sistema.
 */
void guardarEquipamentos(const SistemaGestaoIncendios *sistema);

#endif /* PERSISTENCIA_H */
