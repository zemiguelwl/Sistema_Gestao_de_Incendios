#ifndef SUGESTOES_H
#define SUGESTOES_H

/**
 * @file sugestoes.h
 * @brief Sugestão automática de recursos para novas intervenções.
 *
 * Este módulo analisa o histórico de intervenções concluídas para
 * recomendar o número de bombeiros e equipamentos adequados a uma
 * nova intervenção, com base no tipo e prioridade da ocorrência.
 *
 * O mecanismo baseia-se em médias estatísticas de intervenções
 * semelhantes (mesmo tipo de ocorrência e mesma prioridade).
 *
 * @author José (8220942)
 * @date 2024
 * @version 2.0
 */

#include "modelos.h"


/* ========================================================================= */
/*  SUGESTÃO DE RECURSOS                                                     */
/* ========================================================================= */

/**
 * @brief Calcula o número recomendado de bombeiros para uma intervenção.
 *
 * Analisa o histórico de intervenções concluídas com o mesmo tipo de
 * ocorrência e prioridade, calculando a média de bombeiros utilizados.
 *
 * @param sistema    Ponteiro constante para o contexto global do sistema
 * @param tipo       Tipo da ocorrência a intervencionar
 * @param prioridade Prioridade da ocorrência
 * @return Número de bombeiros sugerido, ou 0 se não houver dados suficientes
 *
 * @note Requer pelo menos uma intervenção concluída semelhante para
 *       produzir uma sugestão significativa
 */
int calcularSugestaoBombeiros(const SistemaGestaoIncendios *sistema,
                               TipoOcorrencia tipo,
                               PrioridadeOcorrencia prioridade);

/**
 * @brief Calcula as quantidades recomendadas de equipamentos por tipo.
 *
 * Analisa o histórico de intervenções concluídas com o mesmo tipo de
 * ocorrência e prioridade, calculando a média de cada tipo de equipamento
 * utilizado.
 *
 * @param sistema          Ponteiro constante para o contexto global
 * @param tipo             Tipo da ocorrência a intervencionar
 * @param prioridade       Prioridade da ocorrência
 * @param mediaVeiculos    Ponteiro onde será guardada a média de veículos
 * @param mediaMangueiras  Ponteiro onde será guardada a média de mangueiras
 * @param mediaRespiradores Ponteiro onde será guardada a média de respiradores
 *
 * @note Os ponteiros de saída são definidos a 0 se não houver dados suficientes
 * @note Todos os ponteiros de saída devem ser não-NULL
 */
void calcularSugestaoEquipamentos(const SistemaGestaoIncendios *sistema,
                                   TipoOcorrencia tipo,
                                   PrioridadeOcorrencia prioridade,
                                   int *mediaVeiculos,
                                   int *mediaMangueiras,
                                   int *mediaRespiradores);


#endif /* SUGESTOES_H */