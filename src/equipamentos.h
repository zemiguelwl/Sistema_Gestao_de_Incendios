#ifndef EQUIPAMENTOS_H
#define EQUIPAMENTOS_H

#include "modelos.h"

/**
 * @file equipamentos.h
 * @brief Operações de gestão (CRUD) da entidade Equipamento.
 *
 * Este módulo permite criar, listar, atualizar, inativar, reativar
 * e pesquisar equipamentos que podem ser associados a intervenções.
 */

/**
 * @brief Cria um novo equipamento.
 *
 * Solicita ao utilizador a designação, tipo, localização e estado inicial.
 *
 * @param sistema Ponteiro para o sistema de gestão de incêndios.
 * @return ID do equipamento criado, ou -1 em caso de erro.
 */
int criarEquipamento(SistemaGestaoIncendios *sistema);

/**
 * @brief Lista todos os equipamentos ativos.
 *
 * Mostra ID, designação, tipo, estado e localização.
 *
 * @param sistema Ponteiro para o sistema de gestão de incêndios.
 */
void listarEquipamentos(const SistemaGestaoIncendios *sistema);

/**
 * @brief Atualiza os dados de um equipamento existente.
 *
 * Permite alterar designação, tipo, estado e localização.
 *
 * @param sistema Ponteiro para o sistema de gestão de incêndios.
 */
void atualizarEquipamento(SistemaGestaoIncendios *sistema);

/**
 * @brief Inativa logicamente um equipamento.
 *
 * Equipamentos inativos deixam de estar disponíveis para intervenções.
 *
 * @param sistema Ponteiro para o sistema de gestão de incêndios.
 */
void inativarEquipamento(SistemaGestaoIncendios *sistema);

/**
 * @brief Reativa um equipamento previamente inativado.
 *
 * O equipamento volta a estar disponível para intervenções.
 *
 * @param sistema Ponteiro para o sistema de gestão de incêndios.
 */
void reativarEquipamento(SistemaGestaoIncendios *sistema);

/**
 * @brief Procura um equipamento pelo respetivo ID.
 *
 * @param sistema Ponteiro para o sistema de gestão de incêndios.
 * @param id ID do equipamento a procurar.
 * @return Ponteiro para o equipamento encontrado, ou NULL se não existir.
 */
Equipamento* procurarEquipamentoPorId(const SistemaGestaoIncendios *sistema, int id);

/**
 * @brief Elimina definitivamente um equipamento se não existir qualquer relação.
 *
 * A eliminação física só ocorre se:
 *  - o equipamento estiver inativo (ativo == 0)
 *  - não estiver associado a nenhuma intervenção
 *
 * Caso contrário, mantém-se apenas como inativo.
 *
 * @param sistema Estrutura principal do sistema
 * @param idEquipamento ID do equipamento a tentar eliminar
 */
void tentarEliminarEquipamento(SistemaGestaoIncendios *sistema, int idEquipamento);


#endif /* EQUIPAMENTOS_H */
