#include "sugestoes.h"
#include "intervencoes.h"
#include "ocorrencias.h"
#include "equipamentos.h"
#include "bombeiros.h"
#include <stdio.h>

#include "utils.h"

/**
 * @file sugestoes.c
 * @brief Mecanismo auxiliar de apoio à decisão.
 *
 * Este módulo fornece cálculos estatísticos baseados em intervenções
 * concluídas anteriormente, com o mesmo tipo e prioridade de ocorrência.
 *
 * As sugestões:
 *  - Não alteram automaticamente o sistema
 *  - Não constituem uma funcionalidade independente
 *  - Servem apenas para apoiar o utilizador durante o planeamento
 *
 * É exigido um mínimo de 3 intervenções concluídas para garantir
 * resultados minimamente fiáveis.
 */


/**
 * @brief Calcula a média de bombeiros usados em intervenções semelhantes.
 *
 * @return Média de bombeiros ou -1 se não houver dados suficientes
 */
int calcularSugestaoBombeiros(const SistemaGestaoIncendios *sistema,
                              TipoOcorrencia tipo,
                              PrioridadeOcorrencia prioridade)
{
    int total = 0;
    int n = 0;

    for (int i = 0; i < sistema->intervencoes.tamanho; i++) {

        Intervencao *intv = &sistema->intervencoes.dados[i];

        /* Apenas intervenções concluídas e ativas */
        if (intv->ativo == 0 || intv->estado != INT_CONCLUIDA)
            continue;

        Ocorrencia *o = procurarOcorrenciaPorId(sistema, intv->idOcorrencia);
        if (o == NULL)
            continue;

        if (o->tipo == tipo && o->prioridade == prioridade) {
            total += intv->numBombeiros;
            n++;
        }
    }

    /* Dados insuficientes */
    if (n < 3)
        return -1;

    return total / n;
}



void calcularSugestaoEquipamentos(const SistemaGestaoIncendios *sistema,
                                 TipoOcorrencia tipo,
                                 PrioridadeOcorrencia prioridade,
                                 int *mediaVeiculos,
                                 int *mediaMangueiras,
                                 int *mediaRespiradores)
{
    int totalVeic = 0;
    int totalMang = 0;
    int totalResp = 0;
    int n = 0;  // número de intervenções consideradas

    for (int i = 0; i < sistema->intervencoes.tamanho; i++) {

        Intervencao *intv = &sistema->intervencoes.dados[i];

        /* Apenas intervenções concluídas e ativas */
        if (!intv->ativo || intv->estado != INT_CONCLUIDA)
            continue;

        /* Obter ocorrência associada */
        Ocorrencia *o = procurarOcorrenciaPorId(sistema, intv->idOcorrencia);
        if (!o)
            continue;

        /* Apenas intervenções com o mesmo tipo e prioridade */
        if (o->tipo == tipo && o->prioridade == prioridade) {

            /* Somar TODOS os equipamentos usados nesta intervenção */
            for (int j = 0; j < intv->numEquipamentos; j++) {

                Equipamento *eq =
                    procurarEquipamentoPorId(sistema, intv->idsEquipamentos[j]);

                /* Se o equipamento já não existir ignora */
                if (!eq)
                    continue;

                switch (eq->tipo) {
                    case VEICULO:    totalVeic++; break;
                    case MANGUEIRA:  totalMang++; break;
                    case RESPIRADOR: totalResp++; break;
                }
            }

            n++;
        }
    }

    /* Necessário mínimo de 3 intervenções */
    if (n < 3) {
        *mediaVeiculos = -1;
        *mediaMangueiras = -1;
        *mediaRespiradores = -1;
        return;
    }

    /* Média por intervenção */
    *mediaVeiculos     = totalVeic / n;
    *mediaMangueiras   = totalMang / n;
    *mediaRespiradores = totalResp / n;
}

