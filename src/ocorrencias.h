#ifndef OCORRENCIAS_H
#define OCORRENCIAS_H

#include "modelos.h"

/**
 * @file ocorrencias.h
 * @brief Operações CRUD associadas à entidade Ocorrencia.
 */

/**
 * @brief Cria uma nova ocorrência.
 *
 * Solicita ao utilizador localização, data/hora, tipo e prioridade.
 *
 * @param sistema Ponteiro para o sistema.
 * @return ID da ocorrência criada ou -1 em caso de erro.
 */
int criarOcorrencia(SistemaGestaoIncendios *sistema);

/**
 * @brief Lista todas as ocorrências ativas.
 *
 * Inclui ID, localização, tipo, estado e data/hora.
 *
 * @param sistema Ponteiro para o sistema.
 */
void listarOcorrencias(const SistemaGestaoIncendios *sistema);

/**
 * @brief Atualiza os dados de uma ocorrência.
 *
 * Permite alterar campos desde que a ocorrência não tenha intervenção ativa.
 *
 * @param sistema Ponteiro para o sistema.
 */
void atualizarOcorrencia(SistemaGestaoIncendios *sistema);

/**
 * @brief Inativa logicamente uma ocorrência.
 *
 * @param sistema Ponteiro para o sistema.
 */
void inativarOcorrencia(SistemaGestaoIncendios *sistema);

/**
 * @brief Reativa uma ocorrência previamente inativada.
 *
 * @param sistema Ponteiro para o sistema.
 */
void reativarOcorrencia(SistemaGestaoIncendios *sistema);

/**
 * @brief Procura uma ocorrência através do ID.
 *
 * @param sistema Ponteiro para o sistema.
 * @param id ID da ocorrência procurada.
 * @return Ponteiro para a ocorrência ou NULL se não existir.
 */
Ocorrencia* procurarOcorrenciaPorId(const SistemaGestaoIncendios *sistema, int id);


/**
 * @brief Elimina definitivamente uma ocorrência se não existirem relações.
 *
 * A eliminação física só ocorre se:
 *  - a ocorrência estiver inativa (ativo == 0)
 *  - o estado for REPORTADA
 *  - não existir nenhuma intervenção associada
 *
 * Caso contrário, a ocorrência permanece apenas inativada.
 *
 * @param sistema Estrutura principal do sistema
 * @param idOcorrencia ID da ocorrência a tentar eliminar
 */
void tentarEliminarOcorrencia(SistemaGestaoIncendios *sistema, int idOcorrencia);


#endif /* OCORRENCIAS_H */
