#ifndef BOMBEIROS_H
#define BOMBEIROS_H

#include "modelos.h"

/**
 * @file bombeiros.h
 * @brief Operações de gestão (CRUD) relacionadas com a entidade Bombeiro.
 *
 * Este módulo disponibiliza funções para criação, listagem, atualização,
 * inativação, reativação e pesquisa de bombeiros no sistema.
 */

/**
 * @brief Cria um novo bombeiro e adiciona-o ao sistema.
 *
 * Solicita ao utilizador os dados necessários para registar um bombeiro,
 * incluindo nome, especialidade e estado operacional.
 *
 * @param sistema Ponteiro para o sistema de gestão de incêndios.
 * @return ID do bombeiro criado, ou -1 em caso de erro.
 */
int criarBombeiro(SistemaGestaoIncendios *sistema);

/**
 * @brief Lista todos os bombeiros ativos do sistema.
 *
 * Apresenta ID, nome, especialidade e estado operacional.
 *
 * @param sistema Ponteiro para o sistema de gestão de incêndios.
 */
void listarBombeiros(const SistemaGestaoIncendios *sistema);

/**
 * @brief Atualiza os dados de um bombeiro existente.
 *
 * Permite alterar nome, estado e especialidade, desde que o bombeiro
 * esteja ativo no sistema.
 *
 * @param sistema Ponteiro para o sistema de gestão de incêndios.
 */
void atualizarBombeiro(SistemaGestaoIncendios *sistema);

/**
 * @brief Inativa logicamente um bombeiro.
 *
 * O bombeiro deixa de aparecer nas listagens e não pode ser selecionado
 * para intervenções, apenas mantém-se na memória para eventual reativação se não possuir relacionamentos que impeçam a sua eliminação direta.
 *
 * @param sistema Ponteiro para o sistema de gestão de incêndios.
 */
void inativarBombeiro(SistemaGestaoIncendios *sistema);

/**
 * @brief Reativa um bombeiro previamente inativado.
 *
 * Torna novamente disponível o bombeiro para seleção e operações.
 *
 * @param sistema Ponteiro para o sistema de gestão de incêndios.
 */
void reativarBombeiro(SistemaGestaoIncendios *sistema);

/**
 * @brief Procura um bombeiro pelo respetivo ID.
 *
 * @param sistema Ponteiro para o sistema de gestão de incêndios.
 * @param id ID do bombeiro a procurar.
 * @return Ponteiro para o bombeiro encontrado, ou NULL se não existir.
 */
Bombeiro* procurarBombeiroPorId(const SistemaGestaoIncendios *sistema, int id);


/**
 * @brief Elimina definitivamente um bombeiro se não existir qualquer relação.
 *
 * A eliminação física só ocorre se:
 *  - o bombeiro estiver inativo (ativo == 0)
 *  - não estiver associado a nenhuma intervenção (qualquer estado)
 *
 * Caso existam relações, o bombeiro permanece apenas inativado.
 *
 * @param sistema Estrutura principal do sistema
 * @param idBombeiro ID do bombeiro a tentar eliminar
 */
void tentarEliminarBombeiro(SistemaGestaoIncendios *sistema, int idBombeiro);


#endif /* BOMBEIROS_H */
