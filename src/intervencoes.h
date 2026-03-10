#ifndef INTERVENCOES_H
#define INTERVENCOES_H

#include "modelos.h"

/**
 * @file intervencoes.h
 * @brief Operações de gestão relacionadas com a entidade Intervencao.
 *
 * Inclui funções para criação, listagem, execução, conclusão,
 * associação de recursos, inativação, reativação e pesquisa de intervenções.
 */

/**
 * @brief Cria uma nova intervenção associada a uma ocorrência.
 *
 * Solicita a data/hora de início e inicializa os recursos.
 *
 * @param sistema Ponteiro para o sistema de gestão de incêndios.
 * @return ID da intervenção criada, ou -1 em caso de erro.
 */
int criarIntervencao(SistemaGestaoIncendios *sistema);

/**
 * @brief Lista todas as intervenções ativas.
 *
 * Mostra estado, datas, bombeiros e equipamentos associados.
 *
 * @param sistema Ponteiro para o sistema de gestão de incêndios.
 */
void listarIntervencoes(const SistemaGestaoIncendios *sistema);


/**
 * @brief Inicia a execução de uma intervenção.
 *
 * Altera o estado para EM_EXECUCAO, ativa bombeiros e equipamentos
 * associados e valida pré-condições.
 *
 * @param sistema Ponteiro para o sistema de gestão de incêndios.
 */
void iniciarExecucaoIntervencao(SistemaGestaoIncendios *sistema);

/**
 * @brief Conclui uma intervenção registando a data/hora de fim.
 *
 * Liberta bombeiros e equipamentos e atualiza o estado da ocorrência associada.
 *
 * @param sistema Ponteiro para o sistema de gestão de incêndios.
 */
void concluirIntervencao(SistemaGestaoIncendios *sistema);

/**
 * @brief Associa bombeiros a uma intervenção em planeamento.
 *
 * Permite adicionar um ou mais bombeiros disponíveis.
 *
 * @param sistema Ponteiro para o sistema de gestão de incêndios.
 * @param intvExterna Ponteiro para a intervenção alvo.
 */
void adicionarBombeirosIntervencao(SistemaGestaoIncendios *sistema, Intervencao *intvExterna);

/**
 * @brief Associa equipamentos a uma intervenção em planeamento.
 *
 * @param sistema Ponteiro para o sistema de gestão de incêndios.
 */
void adicionarEquipamentosIntervencao(SistemaGestaoIncendios *sistema, Intervencao *intvExterna);

/**
 * @brief Inativa logicamente uma intervenção.
 *
 * Não é permitida a inativação de intervenções em execução.
 *
 * @param sistema Ponteiro para o sistema de gestão de incêndios.
 */
void inativarIntervencao(SistemaGestaoIncendios *sistema);

/**
 * @brief Reativa uma intervenção previamente inativada.
 *
 * Restaura recursos e estado tal como se encontravam.
 *
 * @param sistema Ponteiro para o sistema de gestão de incêndios.
 */
void reativarIntervencao(SistemaGestaoIncendios *sistema);

/**
 * @brief Procura uma intervenção pelo respetivo ID.
 *
 * @param sistema Ponteiro para o sistema de gestão de incêndios.
 * @param id ID da intervenção a procurar.
 * @return Ponteiro para a intervenção encontrada, ou NULL se não existir.
 */
Intervencao* procurarIntervencaoPorId(const SistemaGestaoIncendios *sistema, int id);


/**
 * @brief Elimina definitivamente uma intervenção concluída e inativa.
 *
 * A eliminação física só ocorre se:
 *  - a intervenção estiver inativa (ativo == 0)
 *  - o estado for INT_CONCLUIDA
 *
 * Em caso de eliminação:
 *  - são libertadas todas as estruturas dinâmicas associadas
 *
 * @param sistema Estrutura principal do sistema
 * @param idIntervencao ID da intervenção a tentar eliminar
 */
void tentarEliminarIntervencao(SistemaGestaoIncendios *sistema, int idIntervencao);


#endif /* INTERVENCOES_H */
