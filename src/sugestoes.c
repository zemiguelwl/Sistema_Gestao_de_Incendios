#include "sugestoes.h"
#include "intervencoes.h"
#include "ocorrencias.h"
#include "equipamentos.h"
#include "bombeiros.h"
#include "utils.h"
#include <stdio.h>

/**
 * @file sugestoes.c
 * @brief Mecanismo auxiliar de apoio à decisão no planeamento de intervenções.
 *
 * Este módulo fornece cálculos estatísticos baseados em intervenções
 * concluídas anteriormente com o mesmo tipo e prioridade de ocorrência.
 *
 * As sugestões:
 *  - Não alteram automaticamente o estado do sistema
 *  - Não constituem uma funcionalidade independente (são chamadas por intervencoes.c)
 *  - Servem apenas para apoiar o utilizador durante o planeamento
 *
 * É exigido um mínimo de 3 intervenções concluídas semelhantes para
 * garantir resultados minimamente fiáveis. Abaixo desse limiar, as funções
 * retornam -1 para indicar dados insuficientes.
 */


/* ========================================================================= */
/*  SUGESTÃO DE BOMBEIROS                                                    */
/* ========================================================================= */

int calcularSugestaoBombeiros(const SistemaGestaoIncendios *sistema,
                               TipoOcorrencia tipo,
                               PrioridadeOcorrencia prioridade)
{
    int total = 0;
    int n     = 0;

    for (int i = 0; i < sistema->intervencoes.tamanho; i++) {

        const Intervencao *intv = &sistema->intervencoes.dados[i];

        /* Apenas intervenções concluídas e ativas */
        if (intv->ativo == 0 || intv->estado != INT_CONCLUIDA)
            continue;

        const Ocorrencia *o = procurarOcorrenciaPorId(sistema, intv->idOcorrencia);
        if (o == NULL)
            continue;

        if (o->tipo == tipo && o->prioridade == prioridade) {
            total += intv->numBombeiros;
            n++;
        }
    }

    /* Dados insuficientes — limiar mínimo de 3 intervenções */
    if (n < 3)
        return -1;

    return total / n;
}


/* ========================================================================= */
/*  SUGESTÃO DE EQUIPAMENTOS                                                 */
/* ========================================================================= */

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
    int n         = 0;

    for (int i = 0; i < sistema->intervencoes.tamanho; i++) {

        const Intervencao *intv = &sistema->intervencoes.dados[i];

        /* Apenas intervenções concluídas e ativas */
        if (!intv->ativo || intv->estado != INT_CONCLUIDA)
            continue;

        const Ocorrencia *o = procurarOcorrenciaPorId(sistema, intv->idOcorrencia);
        if (o == NULL)
            continue;

        /* Apenas intervenções com o mesmo tipo e prioridade de ocorrência */
        if (o->tipo == tipo && o->prioridade == prioridade) {

            for (int j = 0; j < intv->numEquipamentos; j++) {

                const Equipamento *eq =
                    procurarEquipamentoPorId(sistema, intv->idsEquipamentos[j]);

                /* Se o equipamento já foi eliminado fisicamente, ignorar */
                if (eq == NULL)
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

    /* Dados insuficientes — limiar mínimo de 3 intervenções */
    if (n < 3) {
        *mediaVeiculos     = -1;
        *mediaMangueiras   = -1;
        *mediaRespiradores = -1;
        return;
    }

    /* Média por intervenção */
    *mediaVeiculos     = totalVeic / n;
    *mediaMangueiras   = totalMang / n;
    *mediaRespiradores = totalResp / n;
}