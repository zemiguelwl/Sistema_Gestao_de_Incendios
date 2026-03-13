#include "relatorios.h"
#include "ocorrencias.h"
#include "utils.h"
#include "logs.h"
#include <stdio.h>
#include <string.h>

/**
 * @file relatorios.c
 * @brief Geração de relatórios estatísticos e operacionais do sistema.
 *
 * Obrigatórios:
 *  - Ocorrências: por estado, tipo e prioridade; tempo médio de resposta;
 *                 análise por localização; frequência de incidentes
 *  - Intervenções: por estado; duração média; recursos mais utilizados; eficiência
 *  - Bombeiros: disponibilidade por especialidade; ranking de desempenho
 *  - Equipamentos: inventário; em manutenção; utilização por tipo; ranking
 *
 * Extras:
 *  - Capacidade Operacional (visão global)
 *  - Carga de Trabalho por Período
 */

#define MAX_STATS_LOCAIS 100
#define MAX_TRACK_IDS    500

/* Pausa comum no fim de cada relatório */
static void pausaEnter(void) {
    printf("Prima ENTER para voltar...");
    getchar();
}


/* ========================================================================= */
/*  1. OCORRÊNCIAS POR ESTADO                                                */
/* ========================================================================= */

void relatorioOcorrenciasPorEstado(const SistemaGestaoIncendios *sistema) {
    int total = 0, reportada = 0, emIntervencao = 0, concluida = 0;

    for (int i = 0; i < sistema->ocorrencias.tamanho; i++) {
        const Ocorrencia *o = &sistema->ocorrencias.dados[i];
        if (!o->ativo) continue;

        total++;
        switch (o->estado) {
            case REPORTADA:      reportada++;      break;
            case EM_INTERVENCAO: emIntervencao++;  break;
            case CONCLUIDA:      concluida++;       break;
            default: break;
        }
    }

    printf("\n    RELATÓRIO: OCORRÊNCIAS POR ESTADO    \n\n");
    printf("Reportadas:        %d\n", reportada);
    printf("Em Intervenção:    %d\n", emIntervencao);
    printf("Concluídas:        %d\n", concluida);
    printf("TOTAL:             %d\n\n", total);

    registarLog("INFO", "RELATORIOS", "OCORRENCIAS_ESTADO", "Relatório executado");
    pausaEnter();
}


/* ========================================================================= */
/*  2. OCORRÊNCIAS POR TIPO                                                  */
/* ========================================================================= */

void relatorioOcorrenciasPorTipo(const SistemaGestaoIncendios *sistema) {
    int florestal = 0, urbano = 0, industrial = 0, total = 0;

    for (int i = 0; i < sistema->ocorrencias.tamanho; i++) {
        const Ocorrencia *o = &sistema->ocorrencias.dados[i];
        if (!o->ativo) continue;

        total++;
        switch (o->tipo) {
            case FLORESTAL:  florestal++;  break;
            case URBANO:     urbano++;     break;
            case INDUSTRIAL: industrial++; break;
            default: break;
        }
    }

    printf("\n    RELATÓRIO: OCORRÊNCIAS POR TIPO    \n\n");
    printf("Florestal:   %d\n", florestal);
    printf("Urbano:      %d\n", urbano);
    printf("Industrial:  %d\n", industrial);
    printf("TOTAL:       %d\n\n", total);

    registarLog("INFO", "RELATORIOS", "OCORRENCIAS_TIPO", "Relatório executado");
    pausaEnter();
}


/* ========================================================================= */
/*  3. OCORRÊNCIAS POR PRIORIDADE                                            */
/* ========================================================================= */

void relatorioOcorrenciasPorPrioridade(const SistemaGestaoIncendios *sistema) {
    int baixa = 0, normal = 0, alta = 0, total = 0;

    for (int i = 0; i < sistema->ocorrencias.tamanho; i++) {
        const Ocorrencia *o = &sistema->ocorrencias.dados[i];
        if (!o->ativo) continue;

        total++;
        switch (o->prioridade) {
            case BAIXA:  baixa++;  break;
            case NORMAL: normal++; break;
            case ALTA:   alta++;   break;
            default: break;
        }
    }

    printf("\n    RELATÓRIO: OCORRÊNCIAS POR PRIORIDADE    \n\n");
    printf("Baixa:   %d\n", baixa);
    printf("Normal:  %d\n", normal);
    printf("Alta:    %d\n", alta);
    printf("TOTAL:   %d\n\n", total);

    registarLog("INFO", "RELATORIOS", "OCORRENCIAS_PRIORIDADE", "Relatório executado");
    pausaEnter();
}


/* ========================================================================= */
/*  4. TEMPO DE RESPOSTA (Ocorrência → Início Intervenção)                  */
/* ========================================================================= */

void relatorioTempoRespostaOcorrencias(const SistemaGestaoIncendios *sistema) {
    int soma = 0, count = 0;
    int minVal = 999999999, maxVal = 0;

    for (int i = 0; i < sistema->intervencoes.tamanho; i++) {
        const Intervencao *intv = &sistema->intervencoes.dados[i];
        if (!intv->ativo) continue;
        if (intv->estado == EM_PLANEAMENTO) continue; /* ainda não iniciou */

        const Ocorrencia *oc = procurarOcorrenciaPorId(sistema, intv->idOcorrencia);
        if (oc == NULL || !oc->ativo) continue;

        int diff = minutosEntre(oc->dataHora, intv->inicio);
        if (diff < 0) continue;

        soma += diff;
        if (diff < minVal) minVal = diff;
        if (diff > maxVal) maxVal = diff;
        count++;
    }

    printf("\n    RELATÓRIO: TEMPO DE RESPOSTA DAS OCORRÊNCIAS    \n\n");

    if (count == 0) {
        printf("Não existem intervenções iniciadas (ou dados insuficientes).\n\n");
    } else {
        printf("Tempo médio de resposta: %d minutos\n", soma / count);
        printf("Resposta mínima:         %d minutos\n", minVal);
        printf("Resposta máxima:         %d minutos\n\n", maxVal);
    }

    registarLog("INFO", "RELATORIOS", "TEMPO_RESPOSTA", "Relatório executado");
    pausaEnter();
}


/* ========================================================================= */
/*  5. OCORRÊNCIAS POR LOCALIZAÇÃO                                          */
/* ========================================================================= */

void relatorioOcorrenciasPorLocalizacao(const SistemaGestaoIncendios *sistema) {
    typedef struct {
        char localizacao[MAX_LOCAL];
        int  total;
        int  florestal;
        int  urbano;
        int  industrial;
    } LocalizacaoStats;

    LocalizacaoStats stats[MAX_STATS_LOCAIS];
    int numLocais = 0;

    for (int i = 0; i < sistema->ocorrencias.tamanho; i++) {
        const Ocorrencia *oc = &sistema->ocorrencias.dados[i];
        if (!oc->ativo) continue;

        /* Procurar localização já existente */
        int idx = -1;
        for (int j = 0; j < numLocais; j++) {
            if (strcmp(stats[j].localizacao, oc->localizacao) == 0) {
                idx = j;
                break;
            }
        }

        /* Nova localização */
        if (idx == -1) {
            if (numLocais >= MAX_STATS_LOCAIS) continue;
            strncpy(stats[numLocais].localizacao, oc->localizacao, MAX_LOCAL - 1);
            stats[numLocais].localizacao[MAX_LOCAL - 1] = '\0';
            stats[numLocais].total      = 0;
            stats[numLocais].florestal  = 0;
            stats[numLocais].urbano     = 0;
            stats[numLocais].industrial = 0;
            idx = numLocais++;
        }

        stats[idx].total++;
        switch (oc->tipo) {
            case FLORESTAL:  stats[idx].florestal++;  break;
            case URBANO:     stats[idx].urbano++;     break;
            case INDUSTRIAL: stats[idx].industrial++; break;
            default: break;
        }
    }

    printf("\n    RELATÓRIO: ANÁLISE DE OCORRÊNCIAS POR LOCALIZAÇÃO    \n\n");

    if (numLocais == 0) {
        printf("Não existem ocorrências registadas.\n\n");
    } else {
        printf("%-30s %6s %10s %8s %12s\n",
               "Localização", "Total", "Florestal", "Urbano", "Industrial");
        printf("------------------------------------------------------------------------\n");

        for (int i = 0; i < numLocais; i++) {
            printf("%-30s %6d %10d %8d %12d\n",
                   stats[i].localizacao,
                   stats[i].total,
                   stats[i].florestal,
                   stats[i].urbano,
                   stats[i].industrial);
        }
        printf("\n");
    }

    registarLog("INFO", "RELATORIOS", "OCORRENCIAS_LOCALIZACAO", "Relatório executado");
    pausaEnter();
}


/* ========================================================================= */
/*  6. FREQUÊNCIA DE INCIDENTES POR ANO                                     */
/* ========================================================================= */

void relatorioFrequenciaIncidentes(const SistemaGestaoIncendios *sistema) {
    int ano = getInt(2000, 2100, "Ano para análise de frequência");

    int porMes[13]     = {0};
    int porTipo[13][3];
    memset(porTipo, 0, sizeof(porTipo));

    for (int i = 0; i < sistema->ocorrencias.tamanho; i++) {
        const Ocorrencia *oc = &sistema->ocorrencias.dados[i];
        if (!oc->ativo)           continue;
        if (oc->dataHora.ano != ano) continue;

        int m = oc->dataHora.mes;
        if (m < 1 || m > 12)     continue;

        porMes[m]++;
        if (oc->tipo >= 0 && oc->tipo <= 2)
            porTipo[m][oc->tipo]++;
    }

    printf("\n    RELATÓRIO: FREQUÊNCIA DE INCIDENTES (ANO %d)    \n\n", ano);
    printf("Mês  Total  Florestal  Urbano  Industrial\n");
    printf("--------------------------------------------\n");

    int totalAno = 0;
    for (int mes = 1; mes <= 12; mes++) {
        if (porMes[mes] == 0) continue;
        printf("%-3d  %-5d  %-9d  %-6d  %-10d\n",
               mes, porMes[mes],
               porTipo[mes][FLORESTAL],
               porTipo[mes][URBANO],
               porTipo[mes][INDUSTRIAL]);
        totalAno += porMes[mes];
    }

    printf("--------------------------------------------\n");
    printf("TOTAL NO ANO: %d ocorrências\n\n", totalAno);

    registarLog("INFO", "RELATORIOS", "FREQUENCIA_INCIDENTES", "Relatório executado");
    pausaEnter();
}


/* ========================================================================= */
/*  7. INTERVENÇÕES POR ESTADO                                              */
/* ========================================================================= */

void relatorioIntervencoesPorEstado(const SistemaGestaoIncendios *sistema) {
    int planeamento = 0, execucao = 0, concluida = 0;

    for (int i = 0; i < sistema->intervencoes.tamanho; i++) {
        const Intervencao *intv = &sistema->intervencoes.dados[i];
        if (!intv->ativo) continue;

        switch (intv->estado) {
            case EM_PLANEAMENTO: planeamento++; break;
            case EM_EXECUCAO:    execucao++;    break;
            case INT_CONCLUIDA:  concluida++;   break;
            default: break;
        }
    }

    printf("\n    RELATÓRIO: INTERVENÇÕES POR ESTADO    \n\n");
    printf("Em Planeamento:  %d\n", planeamento);
    printf("Em Execução:     %d\n", execucao);
    printf("Concluídas:      %d\n\n", concluida);

    registarLog("INFO", "RELATORIOS", "INT_ESTADO", "Relatório executado");
    pausaEnter();
}


/* ========================================================================= */
/*  8. DURAÇÃO MÉDIA DAS INTERVENÇÕES                                       */
/* ========================================================================= */

void relatorioDuracaoMediaIntervencoes(const SistemaGestaoIncendios *sistema) {
    int soma = 0, count = 0;
    int minVal = 999999999, maxVal = 0;

    for (int i = 0; i < sistema->intervencoes.tamanho; i++) {
        const Intervencao *intv = &sistema->intervencoes.dados[i];
        if (!intv->ativo || !intv->fimDefinido) continue;

        int dur = minutosEntre(intv->inicio, intv->fim);
        if (dur < 0) continue;

        soma += dur;
        if (dur < minVal) minVal = dur;
        if (dur > maxVal) maxVal = dur;
        count++;
    }

    printf("\n    RELATÓRIO: DURAÇÃO MÉDIA DAS INTERVENÇÕES    \n\n");

    if (count == 0) {
        printf("Nenhuma intervenção concluída (ou com fim definido).\n\n");
    } else {
        printf("Duração média:  %d minutos\n", soma / count);
        printf("Duração mínima: %d minutos\n", minVal);
        printf("Duração máxima: %d minutos\n\n", maxVal);
    }

    registarLog("INFO", "RELATORIOS", "INT_DURACAO", "Relatório executado");
    pausaEnter();
}


/* ========================================================================= */
/*  9. RECURSOS MAIS UTILIZADOS                                             */
/* ========================================================================= */

void relatorioRecursosMaisUtilizados(const SistemaGestaoIncendios *sistema) {
    int contagemBomb[MAX_TRACK_IDS]  = {0};
    int contagemEquip[MAX_TRACK_IDS] = {0};

    for (int i = 0; i < sistema->intervencoes.tamanho; i++) {
        const Intervencao *intv = &sistema->intervencoes.dados[i];
        if (!intv->ativo) continue;

        for (int j = 0; j < intv->numBombeiros; j++) {
            int id = intv->idsBombeiros[j];
            if (id >= 0 && id < MAX_TRACK_IDS) contagemBomb[id]++;
        }
        for (int j = 0; j < intv->numEquipamentos; j++) {
            int id = intv->idsEquipamentos[j];
            if (id >= 0 && id < MAX_TRACK_IDS) contagemEquip[id]++;
        }
    }

    printf("\n    RELATÓRIO: RECURSOS MAIS UTILIZADOS EM INTERVENÇÕES    \n\n");

    printf("|BOMBEIROS|\n");
    printf("ID    Nome                     Nº Intervenções\n");
    printf("-----------------------------------------------\n");

    int encontrouBomb = 0;
    for (int i = 0; i < sistema->bombeiros.tamanho; i++) {
        const Bombeiro *b = &sistema->bombeiros.dados[i];
        if (!b->ativo) continue;
        int id = b->idBombeiro;
        if (id < 0 || id >= MAX_TRACK_IDS || contagemBomb[id] == 0) continue;
        printf("%-5d %-24s %d\n", id, b->nome, contagemBomb[id]);
        encontrouBomb = 1;
    }
    if (!encontrouBomb)
        printf("Nenhum bombeiro foi utilizado em intervenções.\n");

    printf("\n|EQUIPAMENTOS|\n");
    printf("ID    Designação                Nº Utilizações\n");
    printf("-----------------------------------------------\n");

    int encontrouEquip = 0;
    for (int i = 0; i < sistema->equipamentos.tamanho; i++) {
        const Equipamento *e = &sistema->equipamentos.dados[i];
        if (!e->ativo) continue;
        int id = e->idEquipamento;
        if (id < 0 || id >= MAX_TRACK_IDS || contagemEquip[id] == 0) continue;
        printf("%-5d %-24s %d\n", id, e->designacao, contagemEquip[id]);
        encontrouEquip = 1;
    }
    if (!encontrouEquip)
        printf("Nenhum equipamento foi utilizado em intervenções.\n");

    printf("\n");
    registarLog("INFO", "RELATORIOS", "RECURSOS_UTILIZADOS", "Relatório executado");
    pausaEnter();
}


/* ========================================================================= */
/*  10. EFICIÊNCIA DAS INTERVENÇÕES                                         */
/* ========================================================================= */

void relatorioEficienciaIntervencoes(const SistemaGestaoIncendios *sistema) {
    typedef struct { int count; int somaTempo; int somaRecursos; } EficienciaStats;

    EficienciaStats stats[3];
    memset(stats, 0, sizeof(stats));

    for (int i = 0; i < sistema->intervencoes.tamanho; i++) {
        const Intervencao *intv = &sistema->intervencoes.dados[i];
        if (!intv->ativo || intv->estado != INT_CONCLUIDA || !intv->fimDefinido)
            continue;

        const Ocorrencia *oc = procurarOcorrenciaPorId(sistema, intv->idOcorrencia);
        if (oc == NULL || !oc->ativo || oc->tipo < 0 || oc->tipo > 2) continue;

        int duracao = minutosEntre(intv->inicio, intv->fim);
        if (duracao < 0) continue;

        stats[oc->tipo].count++;
        stats[oc->tipo].somaTempo    += duracao;
        stats[oc->tipo].somaRecursos += (intv->numBombeiros + intv->numEquipamentos);
    }

    printf("\n    RELATÓRIO: ANÁLISE DE EFICIÊNCIA DE INTERVENÇÕES    \n\n");
    printf("Tipo         Intervenções  Duração Média (min)  Recursos Médios\n");
    printf("------------------------------------------------------------------\n");

    const char *nomes[] = {"Florestal", "Urbano", "Industrial"};
    for (int t = 0; t < 3; t++) {
        if (stats[t].count > 0) {
            int   durMed = stats[t].somaTempo    / stats[t].count;
            float recMed = (float)stats[t].somaRecursos / stats[t].count;
            printf("%-12s %-13d %-20d %.1f\n",
                   nomes[t], stats[t].count, durMed, recMed);
        } else {
            printf("%-12s %-13d -                    -\n", nomes[t], 0);
        }
    }

    printf("\n");
    registarLog("INFO", "RELATORIOS", "EFICIENCIA_INT", "Relatório executado");
    pausaEnter();
}


/* ========================================================================= */
/*  11. DISPONIBILIDADE DE BOMBEIROS POR ESPECIALIDADE                      */
/* ========================================================================= */

void relatorioDisponibilidadeBombeiros(const SistemaGestaoIncendios *sistema) {
    int dispCombate = 0, dispResgate = 0, dispIncendio = 0;
    int totCombate  = 0, totResgate  = 0, totIncendio  = 0;

    for (int i = 0; i < sistema->bombeiros.tamanho; i++) {
        const Bombeiro *b = &sistema->bombeiros.dados[i];
        if (!b->ativo) continue;

        switch (b->especialidade) {
            case COMBATE_FLORESTAL:
                totCombate++;
                if (b->estado == DISPONIVEL) dispCombate++;
                break;
            case RESGATE:
                totResgate++;
                if (b->estado == DISPONIVEL) dispResgate++;
                break;
            case INCENDIO_URBANO:                          /* corrigido: INCENDIO_URBANO */
                totIncendio++;
                if (b->estado == DISPONIVEL) dispIncendio++;
                break;
            default: break;
        }
    }

    printf("\n    RELATÓRIO: DISPONIBILIDADE POR ESPECIALIDADE    \n\n");
    printf("Especialidade       Disponíveis  Total  Percentagem\n");
    printf("-----------------------------------------------------\n");
    printf("Combate Florestal   %-12d %-6d %.1f%%\n",
           dispCombate, totCombate,
           totCombate > 0 ? (100.0 * dispCombate / totCombate) : 0.0);
    printf("Resgate             %-12d %-6d %.1f%%\n",
           dispResgate, totResgate,
           totResgate > 0 ? (100.0 * dispResgate / totResgate) : 0.0);
    printf("Incêndio Urbano     %-12d %-6d %.1f%%\n\n",
           dispIncendio, totIncendio,
           totIncendio > 0 ? (100.0 * dispIncendio / totIncendio) : 0.0);

    registarLog("INFO", "RELATORIOS", "BOMBEIROS_DISP_ESP", "Relatório executado");
    pausaEnter();
}


/* ========================================================================= */
/*  12. RANKING DE DESEMPENHO DOS BOMBEIROS                                 */
/* ========================================================================= */

void relatorioRankingDesempenhoBombeiros(const SistemaGestaoIncendios *sistema) {
    int interv[MAX_TRACK_IDS]   = {0};
    int somaResp[MAX_TRACK_IDS] = {0};

    for (int i = 0; i < sistema->intervencoes.tamanho; i++) {
        const Intervencao *intv = &sistema->intervencoes.dados[i];
        if (!intv->ativo || intv->estado != INT_CONCLUIDA) continue;

        const Ocorrencia *oc = procurarOcorrenciaPorId(sistema, intv->idOcorrencia);
        if (oc == NULL || !oc->ativo) continue;

        int resp = minutosEntre(oc->dataHora, intv->inicio);
        if (resp < 0) continue;

        for (int j = 0; j < intv->numBombeiros; j++) {
            int id = intv->idsBombeiros[j];
            if (id >= 0 && id < MAX_TRACK_IDS) {
                interv[id]++;
                somaResp[id] += resp;
            }
        }
    }

    printf("\n    RELATÓRIO: RANKING DE DESEMPENHO DOS BOMBEIROS    \n\n");
    printf("ID    Nome                     Intervenções  Tempo Médio Resp (min)\n");
    printf("-------------------------------------------------------------------\n");

    for (int i = 0; i < sistema->bombeiros.tamanho; i++) {
        const Bombeiro *b = &sistema->bombeiros.dados[i];
        if (!b->ativo) continue;
        int id = b->idBombeiro;
        if (id < 0 || id >= MAX_TRACK_IDS) continue;
        int n     = interv[id];
        int media = (n > 0) ? (somaResp[id] / n) : 0;
        printf("%-5d %-24s %-13d %d\n", id, b->nome, n, media);
    }

    printf("\n");
    registarLog("INFO", "RELATORIOS", "RANK_BOMBEIROS", "Relatório executado");
    pausaEnter();
}


/* ========================================================================= */
/*  13. INVENTÁRIO DE EQUIPAMENTOS                                          */
/* ========================================================================= */

void relatorioInventarioEquipamentos(const SistemaGestaoIncendios *sistema) {
    int total = 0;
    int porTipo[3]   = {0};
    int porEstado[3] = {0};

    printf("\n    RELATÓRIO: INVENTÁRIO DE EQUIPAMENTOS    \n\n");
    printf("ID    Designação                    Tipo         Estado         Localização\n");
    printf("----------------------------------------------------------------------------\n");

    for (int i = 0; i < sistema->equipamentos.tamanho; i++) {
        const Equipamento *e = &sistema->equipamentos.dados[i];
        if (!e->ativo) continue;

        total++;
        if (e->tipo   >= 0 && e->tipo   <= 2) porTipo[e->tipo]++;
        if (e->estado >= 0 && e->estado <= 2) porEstado[e->estado]++;

        printf("%-5d %-28s %-12s %-14s %s\n",
               e->idEquipamento,
               e->designacao,
               tipoEquipamentoParaString(e->tipo),
               estadoEquipamentoParaString(e->estado),
               e->localizacao);
    }

    if (total == 0)
        printf("\nNão existem equipamentos registados.\n");

    printf("\n|Resumo por Tipo|\n");
    printf("Veículos:     %d\n", porTipo[VEICULO]);
    printf("Mangueiras:   %d\n", porTipo[MANGUEIRA]);
    printf("Respiradores: %d\n\n", porTipo[RESPIRADOR]);

    printf("|Resumo por Estado|\n");
    printf("Disponíveis:    %d\n", porEstado[EQ_DISPONIVEL]);
    printf("Em Uso:         %d\n", porEstado[EQ_EM_USO]);
    printf("Em Manutenção:  %d\n\n", porEstado[EQ_EM_MANUTENCAO]);

    printf("TOTAL:          %d\n\n", total);

    registarLog("INFO", "RELATORIOS", "INV_EQUIPAMENTOS", "Relatório executado");
    pausaEnter();
}


/* ========================================================================= */
/*  14. EQUIPAMENTOS EM MANUTENÇÃO                                          */
/* ========================================================================= */

void relatorioEquipamentosEmManutencao(const SistemaGestaoIncendios *sistema) {
    int count = 0;

    printf("\n    RELATÓRIO: EQUIPAMENTOS EM MANUTENÇÃO    \n\n");
    printf("ID    Designação                    Tipo         Localização\n");
    printf("----------------------------------------------------------------\n");

    for (int i = 0; i < sistema->equipamentos.tamanho; i++) {
        const Equipamento *e = &sistema->equipamentos.dados[i];
        if (!e->ativo || e->estado != EQ_EM_MANUTENCAO) continue;

        count++;
        printf("%-5d %-28s %-12s %s\n",
               e->idEquipamento,
               e->designacao,
               tipoEquipamentoParaString(e->tipo),
               e->localizacao);
    }

    if (count == 0)
        printf("\nNão existem equipamentos em manutenção.\n");
    else
        printf("\nTOTAL em manutenção: %d\n", count);

    printf("\n");
    registarLog("INFO", "RELATORIOS", "EQ_MANUTENCAO", "Relatório executado");
    pausaEnter();
}


/* ========================================================================= */
/*  15. UTILIZAÇÃO DE EQUIPAMENTOS POR TIPO DE INTERVENÇÃO                 */
/* ========================================================================= */

void relatorioUtilizacaoEquipamentosPorTipoIntervencao(
        const SistemaGestaoIncendios *sistema)
{
    int florestal = 0, urbano = 0, industrial = 0;

    for (int i = 0; i < sistema->intervencoes.tamanho; i++) {
        const Intervencao *intv = &sistema->intervencoes.dados[i];
        if (!intv->ativo) continue;

        const Ocorrencia *oc = procurarOcorrenciaPorId(sistema, intv->idOcorrencia);
        if (oc == NULL || !oc->ativo) continue;

        switch (oc->tipo) {
            case FLORESTAL:  florestal  += intv->numEquipamentos; break;
            case URBANO:     urbano     += intv->numEquipamentos; break;
            case INDUSTRIAL: industrial += intv->numEquipamentos; break;
            default: break;
        }
    }

    printf("\n    RELATÓRIO: UTILIZAÇÃO DE EQUIPAMENTOS POR TIPO DE INTERVENÇÃO    \n\n");
    printf("Florestal:   %d equipamentos utilizados\n", florestal);
    printf("Urbano:      %d equipamentos utilizados\n", urbano);
    printf("Industrial:  %d equipamentos utilizados\n\n", industrial);

    registarLog("INFO", "RELATORIOS", "UTILIZACAO_EQ_TIPO_INT", "Relatório executado");
    pausaEnter();
}


/* ========================================================================= */
/*  16. RANKING DE EQUIPAMENTOS MAIS UTILIZADOS                             */
/* ========================================================================= */

void relatorioRankingEquipamentos(const SistemaGestaoIncendios *sistema) {
    int contagem[MAX_TRACK_IDS] = {0};

    for (int i = 0; i < sistema->intervencoes.tamanho; i++) {
        const Intervencao *intv = &sistema->intervencoes.dados[i];
        if (!intv->ativo) continue;

        for (int j = 0; j < intv->numEquipamentos; j++) {
            int id = intv->idsEquipamentos[j];
            if (id >= 0 && id < MAX_TRACK_IDS) contagem[id]++;
        }
    }

    printf("\n    RELATÓRIO: RANKING DE EQUIPAMENTOS MAIS UTILIZADOS    \n\n");
    printf("ID    Designação                    Nº Utilizações\n");
    printf("---------------------------------------------------\n");

    for (int i = 0; i < sistema->equipamentos.tamanho; i++) {
        const Equipamento *e = &sistema->equipamentos.dados[i];
        if (!e->ativo) continue;
        int id = e->idEquipamento;
        if (id < 0 || id >= MAX_TRACK_IDS) continue;
        printf("%-5d %-28s %d\n", id, e->designacao, contagem[id]);
    }

    printf("\n");
    registarLog("INFO", "RELATORIOS", "RANK_EQ", "Relatório executado");
    pausaEnter();
}


/* ========================================================================= */
/*  17. EXTRA — CAPACIDADE OPERACIONAL                                      */
/* ========================================================================= */

void relatorioCapacidadeOperacional(const SistemaGestaoIncendios *sistema) {
    int bombDisp = 0, bombTotal = 0;
    int bombEspecCombate = 0, bombEspecResgate = 0, bombEspecIncendio = 0;

    int eqDisp = 0, eqTotal = 0, eqManutencao = 0;
    int veicDisp = 0, mangDisp = 0, respDisp = 0;

    int intervAtivas = 0, intervPlaneamento = 0;
    int ocorrPendentes = 0;

    for (int i = 0; i < sistema->bombeiros.tamanho; i++) {
        const Bombeiro *b = &sistema->bombeiros.dados[i];
        if (!b->ativo) continue;
        bombTotal++;
        if (b->estado == DISPONIVEL) {
            bombDisp++;
            switch (b->especialidade) {
                case COMBATE_FLORESTAL: bombEspecCombate++;  break;
                case RESGATE:           bombEspecResgate++;  break;
                case INCENDIO_URBANO:   bombEspecIncendio++; break; /* corrigido */
                default: break;
            }
        }
    }

    for (int i = 0; i < sistema->equipamentos.tamanho; i++) {
        const Equipamento *e = &sistema->equipamentos.dados[i];
        if (!e->ativo) continue;
        eqTotal++;
        if (e->estado == EQ_DISPONIVEL) {
            eqDisp++;
            switch (e->tipo) {
                case VEICULO:    veicDisp++; break;
                case MANGUEIRA:  mangDisp++; break;
                case RESPIRADOR: respDisp++; break;
                default: break;
            }
        } else if (e->estado == EQ_EM_MANUTENCAO) {
            eqManutencao++;
        }
    }

    for (int i = 0; i < sistema->intervencoes.tamanho; i++) {
        const Intervencao *intv = &sistema->intervencoes.dados[i];
        if (!intv->ativo) continue;
        if (intv->estado == EM_EXECUCAO)    intervAtivas++;
        if (intv->estado == EM_PLANEAMENTO) intervPlaneamento++;
    }

    for (int i = 0; i < sistema->ocorrencias.tamanho; i++) {
        const Ocorrencia *oc = &sistema->ocorrencias.dados[i];
        if (oc->ativo && oc->estado == REPORTADA) ocorrPendentes++;
    }

    float percBombDisp = bombTotal > 0 ? (100.0f * bombDisp / bombTotal) : 0.0f;
    float percEqDisp   = eqTotal   > 0 ? (100.0f * eqDisp   / eqTotal)   : 0.0f;

    float fatorCarga = 100.0f;
    if      (intervAtivas >= 10) fatorCarga = 40.0f;
    else if (intervAtivas >=  5) fatorCarga = 70.0f;

    float notaProntidao = (percBombDisp * 0.5f) +
                          (percEqDisp   * 0.3f) +
                          (fatorCarga   * 0.2f);

    printf("\n    RELATÓRIO EXTRA: CAPACIDADE OPERACIONAL    \n\n");

    printf("|RECURSOS HUMANOS|\n");
    printf("Bombeiros disponíveis:    %d / %d (%.1f%%)\n",
           bombDisp, bombTotal, percBombDisp);
    printf("  - Combate Florestal:    %d\n", bombEspecCombate);
    printf("  - Resgate:              %d\n", bombEspecResgate);
    printf("  - Incêndio Urbano:      %d\n\n", bombEspecIncendio);

    printf("|RECURSOS MATERIAIS|\n");
    printf("Equipamentos disponíveis: %d / %d (%.1f%%)\n",
           eqDisp, eqTotal, percEqDisp);
    printf("  - Veículos:             %d\n", veicDisp);
    printf("  - Mangueiras:           %d\n", mangDisp);
    printf("  - Respiradores:         %d\n", respDisp);
    printf("Em manutenção:            %d\n\n", eqManutencao);

    printf("|CARGA OPERACIONAL|\n");
    printf("Intervenções em execução: %d\n",  intervAtivas);
    printf("Intervenções planeadas:   %d\n",  intervPlaneamento);
    printf("Ocorrências pendentes:    %d\n\n", ocorrPendentes);

    printf("|AVALIAÇÃO GLOBAL|\n");
    printf("NOTA DE PRONTIDÃO:        %.1f/100\n", notaProntidao);
    if      (notaProntidao >= 80) printf("Estado:                   EXCELENTE\n");
    else if (notaProntidao >= 60) printf("Estado:                   BOM\n");
    else if (notaProntidao >= 40) printf("Estado:                   ALERTA\n");
    else                          printf("Estado:                   CRÍTICO\n");
    printf("\n");

    registarLog("INFO", "RELATORIOS", "CAP_OPERACIONAL", "Relatório executado");
    pausaEnter();
}


/* ========================================================================= */
/*  18. EXTRA — CARGA DE TRABALHO POR PERÍODO                              */
/* ========================================================================= */

void relatorioCargaTrabalhoPeriodo(const SistemaGestaoIncendios *sistema) {
    int ano = getInt(2000, 2100, "Ano para análise da carga de trabalho");

    int ocMes[13]   = {0};
    int intIni[13]  = {0};
    int intConc[13] = {0};

    for (int i = 0; i < sistema->ocorrencias.tamanho; i++) {
        const Ocorrencia *oc = &sistema->ocorrencias.dados[i];
        if (!oc->ativo || oc->dataHora.ano != ano) continue;
        int m = oc->dataHora.mes;
        if (m >= 1 && m <= 12) ocMes[m]++;
    }

    for (int i = 0; i < sistema->intervencoes.tamanho; i++) {
        const Intervencao *intv = &sistema->intervencoes.dados[i];
        if (!intv->ativo) continue;

        if (intv->inicio.ano == ano) {
            int m = intv->inicio.mes;
            if (m >= 1 && m <= 12) intIni[m]++;
        }
        if (intv->fimDefinido && intv->fim.ano == ano) {
            int m = intv->fim.mes;
            if (m >= 1 && m <= 12) intConc[m]++;
        }
    }

    printf("\n    RELATÓRIO EXTRA: CARGA DE TRABALHO POR PERÍODO (ANO %d)    \n\n", ano);
    printf("Mês  Ocorrências  Int. Iniciadas  Int. Concluídas\n");
    printf("-------------------------------------------------\n");

    for (int mes = 1; mes <= 12; mes++) {
        printf("%-3d  %-11d  %-14d  %-15d\n",
               mes, ocMes[mes], intIni[mes], intConc[mes]);
    }

    printf("\n");
    registarLog("INFO", "RELATORIOS", "CARGA_TRABALHO", "Relatório executado");
    pausaEnter();
}