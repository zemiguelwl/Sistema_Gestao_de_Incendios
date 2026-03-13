#ifndef RELATORIOS_H
#define RELATORIOS_H

/**
 * @file relatorios.h
 * @brief Geração de relatórios estatísticos e operacionais do SGI.
 *
 * Este módulo disponibiliza relatórios agrupados por área:
 *
 *  - **Ocorrências**: estado, tipo, prioridade, tempos de resposta,
 *    localização e frequência de incidentes
 *  - **Intervenções**: estado, duração média, eficiência e recursos utilizados
 *  - **Bombeiros**: disponibilidade por especialidade e ranking de desempenho
 *  - **Equipamentos**: inventário, manutenção, utilização e ranking
 *  - **Estratégicos** (extra): capacidade operacional e carga de trabalho
 *
 * Todos os relatórios operam sobre dados em memória (read-only)
 * e apenas consideram registos ativos (ativo == 1).
 *
 * @author José (8220942)
 * @date 2024
 * @version 2.0
 */

#include "modelos.h"


/* ========================================================================= */
/*  RELATÓRIOS DE OCORRÊNCIAS                                               */
/* ========================================================================= */

/**
 * @brief Apresenta o número de ocorrências agrupado por estado.
 *
 * Contabiliza: REPORTADA, EM_INTERVENCAO e CONCLUIDA.
 *
 * @param sistema Ponteiro constante para o contexto global
 */
void relatorioOcorrenciasPorEstado(const SistemaGestaoIncendios *sistema);

/**
 * @brief Apresenta a distribuição de ocorrências por tipo.
 *
 * Contabiliza: FLORESTAL, URBANO e INDUSTRIAL.
 *
 * @param sistema Ponteiro constante para o contexto global
 */
void relatorioOcorrenciasPorTipo(const SistemaGestaoIncendios *sistema);

/**
 * @brief Apresenta o número de ocorrências por nível de prioridade.
 *
 * Contabiliza: BAIXA, NORMAL e ALTA.
 *
 * @param sistema Ponteiro constante para o contexto global
 */
void relatorioOcorrenciasPorPrioridade(const SistemaGestaoIncendios *sistema);

/**
 * @brief Calcula estatísticas de tempo de resposta às ocorrências.
 *
 * O tempo de resposta é a diferença (em minutos) entre a data/hora
 * da ocorrência e o início da intervenção associada.
 * Apresenta mínimo, máximo e média.
 *
 * @param sistema Ponteiro constante para o contexto global
 *
 * @note Apenas considera ocorrências com pelo menos uma intervenção associada
 */
void relatorioTempoRespostaOcorrencias(const SistemaGestaoIncendios *sistema);

/**
 * @brief Analisa a distribuição de ocorrências por localização.
 *
 * Agrupa e contabiliza ocorrências por localização geográfica.
 *
 * @param sistema Ponteiro constante para o contexto global
 */
void relatorioOcorrenciasPorLocalizacao(const SistemaGestaoIncendios *sistema);

/**
 * @brief Analisa a frequência de incidentes ao longo de um ano.
 *
 * Apresenta o número de ocorrências por mês para identificar
 * padrões sazonais.
 *
 * @param sistema Ponteiro constante para o contexto global
 */
void relatorioFrequenciaIncidentes(const SistemaGestaoIncendios *sistema);


/* ========================================================================= */
/*  RELATÓRIOS DE INTERVENÇÕES                                              */
/* ========================================================================= */

/**
 * @brief Apresenta o número de intervenções agrupado por estado.
 *
 * Contabiliza: EM_PLANEAMENTO, EM_EXECUCAO e INT_CONCLUIDA.
 *
 * @param sistema Ponteiro constante para o contexto global
 */
void relatorioIntervencoesPorEstado(const SistemaGestaoIncendios *sistema);

/**
 * @brief Calcula estatísticas de duração das intervenções concluídas.
 *
 * A duração é a diferença (em minutos) entre início e fim.
 * Apresenta mínimo, máximo e média.
 *
 * @param sistema Ponteiro constante para o contexto global
 *
 * @note Apenas considera intervenções com fimDefinido == 1
 */
void relatorioDuracaoMediaIntervencoes(const SistemaGestaoIncendios *sistema);

/**
 * @brief Apresenta os recursos humanos e materiais mais utilizados.
 *
 * Identifica os bombeiros e equipamentos com maior número de
 * participações em intervenções.
 *
 * @param sistema Ponteiro constante para o contexto global
 */
void relatorioRecursosMaisUtilizados(const SistemaGestaoIncendios *sistema);

/**
 * @brief Analisa a eficiência das intervenções por tipo de ocorrência.
 *
 * Cruza tipo de ocorrência com duração média da intervenção associada.
 *
 * @param sistema Ponteiro constante para o contexto global
 */
void relatorioEficienciaIntervencoes(const SistemaGestaoIncendios *sistema);


/* ========================================================================= */
/*  RELATÓRIOS DE BOMBEIROS                                                 */
/* ========================================================================= */

/**
 * @brief Apresenta a disponibilidade de bombeiros por especialidade.
 *
 * Para cada especialidade (COMBATE_FLORESTAL, RESGATE, INCENDIO_URBANO),
 * indica quantos bombeiros estão DISPONIVEL, EM_INTERVENCAO_BOMBEIRO
 * e EM_TREINO.
 *
 * @param sistema Ponteiro constante para o contexto global
 */
void relatorioDisponibilidadeBombeiros(const SistemaGestaoIncendios *sistema);

/**
 * @brief Apresenta um ranking de desempenho dos bombeiros.
 *
 * Ordena os bombeiros pelo número de intervenções em que participaram.
 *
 * @param sistema Ponteiro constante para o contexto global
 */
void relatorioRankingDesempenhoBombeiros(const SistemaGestaoIncendios *sistema);


/* ========================================================================= */
/*  RELATÓRIOS DE EQUIPAMENTOS                                              */
/* ========================================================================= */

/**
 * @brief Apresenta o inventário completo de equipamentos.
 *
 * Lista todos os equipamentos ativos com tipo, estado e localização.
 *
 * @param sistema Ponteiro constante para o contexto global
 */
void relatorioInventarioEquipamentos(const SistemaGestaoIncendios *sistema);

/**
 * @brief Lista os equipamentos que se encontram em manutenção.
 *
 * @param sistema Ponteiro constante para o contexto global
 */
void relatorioEquipamentosEmManutencao(const SistemaGestaoIncendios *sistema);

/**
 * @brief Apresenta a utilização de equipamentos por tipo de intervenção.
 *
 * Cruza tipo de equipamento com tipo de ocorrência das intervenções
 * em que foi utilizado.
 *
 * @param sistema Ponteiro constante para o contexto global
 */
void relatorioUtilizacaoEquipamentosPorTipoIntervencao(const SistemaGestaoIncendios *sistema);

/**
 * @brief Apresenta um ranking dos equipamentos mais utilizados.
 *
 * Ordena os equipamentos pelo número de intervenções em que participaram.
 *
 * @param sistema Ponteiro constante para o contexto global
 */
void relatorioRankingEquipamentos(const SistemaGestaoIncendios *sistema);


/* ========================================================================= */
/*  RELATÓRIOS ESTRATÉGICOS (EXTRA)                                         */
/* ========================================================================= */

/**
 * @brief Avalia a capacidade operacional e prontidão atual do sistema.
 *
 * Analisa em simultâneo:
 *  - Percentagem de bombeiros disponíveis
 *  - Percentagem de equipamentos disponíveis
 *  - Número de intervenções ativas (EM_EXECUCAO)
 *
 * @param sistema Ponteiro constante para o contexto global
 */
void relatorioCapacidadeOperacional(const SistemaGestaoIncendios *sistema);

/**
 * @brief Analisa a distribuição de carga de trabalho ao longo de um ano.
 *
 * Apresenta por mês:
 *  - Ocorrências reportadas
 *  - Intervenções iniciadas
 *  - Intervenções concluídas
 *
 * @param sistema Ponteiro constante para o contexto global
 */
void relatorioCargaTrabalhoPeriodo(const SistemaGestaoIncendios *sistema);


#endif /* RELATORIOS_H */