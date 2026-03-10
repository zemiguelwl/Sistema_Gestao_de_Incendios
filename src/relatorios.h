#ifndef RELATORIOS_H
#define RELATORIOS_H

#include "modelos.h"

/**
 * @file relatorios.h
 * @brief Funções de geração de relatórios estatísticos e operacionais
 *        do Sistema de Gestão de Incêndios.
 *
 * Este módulo disponibiliza relatórios sobre:
 *  - Ocorrências (estado, tipo, prioridade, tempos de resposta, localização, frequência)
 *  - Intervenções (estado, duração média, eficiência, recursos utilizados)
 *  - Bombeiros (disponibilidade por especialidade, ranking de desempenho)
 *  - Equipamentos (inventário, manutenção, utilização)
 *  - Indicadores estratégicos globais (relatórios extra)
 *  - Capacidade operacional (relatórios extra)
 */


/**
 * @brief Apresenta o número de ocorrências agrupado por estado.
 *
 * Estados considerados:
 *  - REPORTADA
 *  - EM_INTERVENCAO
 *  - CONCLUIDA
 *
 * Apenas ocorrências ativas são contabilizadas.
 */
void relatorioOcorrenciasPorEstado(const SistemaGestaoIncendios *sistema);

/**
 * @brief Apresenta a distribuição de ocorrências por tipo.
 *
 * Tipos considerados:
 *  - FLORESTAL
 *  - URBANO
 *  - INDUSTRIAL
 */
void relatorioOcorrenciasPorTipo(const SistemaGestaoIncendios *sistema);

/**
 * @brief Apresenta o número de ocorrências por prioridade.
 *
 * Prioridades consideradas:
 *  - BAIXA
 *  - NORMAL
 *  - ALTA
 */
void relatorioOcorrenciasPorPrioridade(const SistemaGestaoIncendios *sistema);

/**
 * @brief Calcula estatísticas de tempo de resposta às ocorrências.
 *
 * O tempo de resposta corresponde à diferença (em minutos)
 * entre a data/hora da ocorrência e o início da intervenção associada.
 *
 */
void relatorioTempoRespostaOcorrencias(const SistemaGestaoIncendios *sistema);

/**
 * @brief Analisa ocorrências agrupadas por localização.
 *
 */
void relatorioOcorrenciasPorLocalizacao(const SistemaGestaoIncendios *sistema);

/**
 * @brief Analisa a frequência de incidentes ao longo de um ano.
 *
 */
void relatorioFrequenciaIncidentes(const SistemaGestaoIncendios *sistema);



/**
 * @brief Apresenta o número de intervenções agrupado por estado.
 *
 * Estados considerados:
 *  - EM_PLANEAMENTO
 *  - EM_EXECUCAO
 *  - INT_CONCLUIDA
 */
void relatorioIntervencoesPorEstado(const SistemaGestaoIncendios *sistema);

/**
 * @brief Calcula estatísticas de duração das intervenções concluídas.
 *
 * A duração corresponde à diferença (em minutos)
 * entre a data/hora de início e a data/hora de fim.
 *
 */
void relatorioDuracaoMediaIntervencoes(const SistemaGestaoIncendios *sistema);

/**
 * @brief Apresenta os recursos humanos e materiais mais utilizados.
 *
 */
void relatorioRecursosMaisUtilizados(const SistemaGestaoIncendios *sistema);

/**
 * @brief Analisa a eficiência das intervenções por tipo de ocorrência.
 *
 */
void relatorioEficienciaIntervencoes(const SistemaGestaoIncendios *sistema);



/**
 * @brief Apresenta a disponibilidade de bombeiros por especialidade.
 *
 */
void relatorioDisponibilidadeBombeiros(const SistemaGestaoIncendios *sistema);

/**
 * @brief Apresenta um ranking de desempenho dos bombeiros.
 *
 */
void relatorioRankingDesempenhoBombeiros(const SistemaGestaoIncendios *sistema);



/**
 * @brief Apresenta o inventário completo de equipamentos.
 *
 */
void relatorioInventarioEquipamentos(const SistemaGestaoIncendios *sistema);

/**
 * @brief Lista os equipamentos que se encontram em manutenção.
 */
void relatorioEquipamentosEmManutencao(const SistemaGestaoIncendios *sistema);

/**
 * @brief Apresenta a utilização de equipamentos por tipo de intervenção.
 */
void relatorioUtilizacaoEquipamentosPorTipoIntervencao(const SistemaGestaoIncendios *sistema);

/**
 * @brief Apresenta um ranking dos equipamentos mais utilizados.
 */
void relatorioRankingEquipamentos(const SistemaGestaoIncendios *sistema);

/* RELATÓRIOS EXTRA */

/**
 * @brief Avalia a capacidade operacional e prontidão do sistema.
 *
 * Relatório estratégico que analisa:
 *  - Disponibilidade de bombeiros
 *  - Disponibilidade de equipamentos
 *  - Carga operacional atual
 *
 * (Relatório EXTRA)
 */
void relatorioCapacidadeOperacional(const SistemaGestaoIncendios *sistema);

/**
 * @brief Analisa a carga de trabalho ao longo de um ano.
 *
 * Apresenta, por mês:
 *  - Ocorrências reportadas
 *  - Intervenções iniciadas
 *  - Intervenções concluídas
 *
 * (Relatório EXTRA)
 */
void relatorioCargaTrabalhoPeriodo(const SistemaGestaoIncendios *sistema);

#endif
