#ifndef SUGESTOES_H
#define SUGESTOES_H

#include "modelos.h"

/**
 * @file sugestoes.h
 * @brief Mecanismo adicional: sugestão automática de recursos para intervenções.
 */

/**
 * @brief Calcula o número recomendado de bombeiros
 * com base em intervenções concluídas semelhantes.
 *
 * @param sistema Ponteiro para o sistema.
 * @param tipo Tipo de ocorrência.
 * @param prioridade Prioridade da ocorrência.
 * @return Número sugerido ou 0 se não houver dados suficientes.
 */
int calcularSugestaoBombeiros(const SistemaGestaoIncendios *sistema,
                              TipoOcorrencia tipo, PrioridadeOcorrencia prioridade);

/**
 * @brief Calcula recomendações de equipamentos com base em intervenções concluídas semelhantes.
 *
 * @param sistema Ponteiro para o sistema.
 * @param tipo Tipo de ocorrência.
 * @param prioridade Prioridade da ocorrência.
 * @param mediaVeiculos Ponteiro para guardar média de veículos.
 * @param mediaMangueiras Ponteiro para guardar média de mangueiras.
 * @param mediaRespiradores Ponteiro para guardar média de respiradores.
 */
void calcularSugestaoEquipamentos(const SistemaGestaoIncendios *sistema,
                                  TipoOcorrencia tipo, PrioridadeOcorrencia prioridade,
                                  int *mediaVeiculos, int *mediaMangueiras, int *mediaRespiradores);

#endif /* SUGESTOES_H */
