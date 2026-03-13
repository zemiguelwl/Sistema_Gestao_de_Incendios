#include "ocorrencias.h"
#include "logs.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

/**
 * @file ocorrencias.c
 * @brief Implementação das operações sobre Ocorrências.
 *
 * Funcionalidades:
 *  - Criar novas ocorrências
 *  - Listar ocorrências ativas
 *  - Atualizar localização, data/hora, tipo e prioridade
 *  - Inativar e reativar ocorrências
 *  - Consultar ocorrência por ID
 *  - Restrições quando associadas a intervenções ativas
 */


/* ========================================================================= */
/*  CRIAR OCORRÊNCIA                                                         */
/* ========================================================================= */

int criarOcorrencia(SistemaGestaoIncendios *sistema) {
    printf("|CRIAR NOVA OCORRÊNCIA|\n\n");

    Ocorrencia nova;

    lerString(nova.localizacao, MAX_LOCAL, "Inserir Localização");
    nova.dataHora   = lerDataHora("Inserir Data e Hora");
    nova.tipo       = escolherTipoOcorrencia();
    nova.prioridade = escolherPrioridadeOcorrencia();

    nova.idOcorrencia = sistema->proximoIdOcorrencia;
    nova.estado       = REPORTADA;
    nova.ativo        = 1;

    if (!expandirListaOcorrenciasSeNecessario(&sistema->ocorrencias)) {
        printf("\nErro: Não foi possível alocar memória para criar a ocorrência!\n");
        registarLog("ERROR", "OCORRENCIAS", "CRIAR", "Falha ao expandir lista");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return -1;
    }

    sistema->ocorrencias.dados[sistema->ocorrencias.tamanho] = nova;
    sistema->ocorrencias.tamanho++;
    sistema->proximoIdOcorrencia++;

    printf("\nOcorrência criada com sucesso!\n");
    printf("   ID: %d\n",    nova.idOcorrencia);
    printf("   Local: %s\n", nova.localizacao);
    printf("   Data: %02d/%02d/%04d\n",
           nova.dataHora.dia, nova.dataHora.mes, nova.dataHora.ano);
    printf("   Hora: %02d:%02d\n", nova.dataHora.hora, nova.dataHora.minuto);
    printf("\n");

    char detalhe[200];
    snprintf(detalhe, sizeof(detalhe),
             "ID=%d;Local=%s;Tipo=%d;Prioridade=%d",
             nova.idOcorrencia, nova.localizacao, nova.tipo, nova.prioridade);
    registarLog("INFO", "OCORRENCIAS", "CRIAR", detalhe);

    printf("Prima ENTER para voltar ao menu...");
    getchar();

    return nova.idOcorrencia;
}


/* ========================================================================= */
/*  LISTAR OCORRÊNCIAS                                                       */
/* ========================================================================= */

void listarOcorrencias(const SistemaGestaoIncendios *sistema) {

    if (sistema->ocorrencias.tamanho == 0) {
        printf("Não existem ocorrências reportadas!\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    int contadorAtivos = 0;

    for (int i = 0; i < sistema->ocorrencias.tamanho; i++) {
        const Ocorrencia *o = &sistema->ocorrencias.dados[i];

        if (o->ativo == 1) {
            contadorAtivos++;
            printf("------------------------------------\n");
            printf("Nrº: %d\n\n", contadorAtivos);
            printf("ID de Ocorrência: %d\n", o->idOcorrencia);
            printf("Local: %s\n",            o->localizacao);
            printf("Tipo: %s\n",             tipoOcorrenciaParaString(o->tipo));
            printf("Estado: %s\n",           estadoOcorrenciaParaString(o->estado));
            printf("Prioridade: %s\n",       prioridadeParaString(o->prioridade));
            printf("Data e Hora: %02d/%02d/%04d %02d:%02d\n",
                   o->dataHora.dia, o->dataHora.mes, o->dataHora.ano,
                   o->dataHora.hora, o->dataHora.minuto);
            printf("\n");
        }
    }

    if (contadorAtivos == 0) {
        printf("Todas as ocorrências estão inativas!\n"); /* Corrigido: \n em falta */
    } else {
        printf("Total de ocorrências ativas: %d\n", contadorAtivos);
    }

    printf("Prima ENTER para voltar ao menu...");
    getchar();

    /* Log comentado para evitar logs excessivos em operações de leitura */
    /* registarLog("INFO", "OCORRENCIAS", "LISTAR", "Listagem executada"); */
}


/* ========================================================================= */
/*  ATUALIZAR OCORRÊNCIA                                                     */
/* ========================================================================= */

void atualizarOcorrencia(SistemaGestaoIncendios *sistema) {
    printf("|ATUALIZAR OCORRÊNCIA|\n\n");

    if (sistema->ocorrencias.tamanho == 0) {
        printf("Não existem ocorrências registadas.\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    /* Listar ocorrências ativas antes de pedir o ID */
    printf("OCORRÊNCIAS ATIVAS:\n");
    printf("==========================================\n");

    int encontrouAtivas = 0;
    for (int i = 0; i < sistema->ocorrencias.tamanho; i++) {
        Ocorrencia *o = &sistema->ocorrencias.dados[i];
        if (o->ativo == 1) {
            printf("ID %d | %-20s | %-10s | %s\n",
                   o->idOcorrencia,
                   o->localizacao,
                   tipoOcorrenciaParaString(o->tipo),
                   estadoOcorrenciaParaString(o->estado));
            encontrouAtivas = 1;
        }
    }

    if (!encontrouAtivas) {
        printf("Não existem ocorrências ativas para atualizar.\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    printf("==========================================\n\n");

    int id = getInt(1, 9999, "ID da ocorrência a atualizar");

    Ocorrencia *o = procurarOcorrenciaPorId(sistema, id);

    if (o == NULL) {
        printf("\nOcorrência com ID %d não encontrada!\n", id);
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    if (o->ativo == 0) {
        printf("\nEsta ocorrência está inativa e não pode ser atualizada.\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    /* Mostrar dados atuais */
    printf("\n==========================================\n");
    printf("            DADOS ATUAIS\n");
    printf("==========================================\n");
    printf("| ID: %d\n",           o->idOcorrencia);
    printf("| Localização: %s\n",  o->localizacao);
    printf("| Data/Hora: %02d/%02d/%04d %02d:%02d\n",
           o->dataHora.dia, o->dataHora.mes, o->dataHora.ano,
           o->dataHora.hora, o->dataHora.minuto);
    printf("| Tipo: %s\n",       tipoOcorrenciaParaString(o->tipo));
    printf("| Estado: %s (gerido automaticamente)\n",
           estadoOcorrenciaParaString(o->estado));
    printf("| Prioridade: %s\n", prioridadeParaString(o->prioridade));
    printf("==========================================\n\n");

    /* Cópia temporária para preview */
    Ocorrencia temp = *o;
    int alterou = 0;

    printf("ATUALIZAÇÃO DE CAMPOS:\n");
    printf("(Deixe vazio/escolha Não para manter o valor atual)\n\n");

    /* 1. Localização */
    char novaLoc[MAX_LOCAL];
    printf("Localização [%s]: ", temp.localizacao);
    if (fgets(novaLoc, MAX_LOCAL, stdin) != NULL) {
        size_t len = strlen(novaLoc);
        if (len > 0 && novaLoc[len - 1] == '\n') {
            novaLoc[len - 1] = '\0';
        }
        if (strlen(novaLoc) > 0) {
            strncpy(temp.localizacao, novaLoc, MAX_LOCAL - 1);
            temp.localizacao[MAX_LOCAL - 1] = '\0';
            alterou = 1;
        }
    }

    /* 2. Data e Hora */
    printf("\nAlterar data e hora? (1=Sim, 0=Não): ");
    if (getInt(0, 1, "") == 1) {
        temp.dataHora = lerDataHora("Nova data e hora");
        alterou = 1;
    }

    /* 3. Tipo */
    printf("\nAlterar tipo? (1=Sim, 0=Não): ");
    if (getInt(0, 1, "") == 1) {
        temp.tipo = escolherTipoOcorrencia();
        alterou = 1;
    }

    /* 4. Prioridade */
    printf("\nAlterar prioridade? (1=Sim, 0=Não): ");
    if (getInt(0, 1, "") == 1) {
        temp.prioridade = escolherPrioridadeOcorrencia();
        alterou = 1;
    }

    if (!alterou) {
        printf("\nNenhuma alteração foi feita.\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    /* Preview das alterações */
    printf("\n==========================================\n");
    printf("         RESUMO DAS ALTERAÇÕES\n");
    printf("==========================================\n");

    if (strcmp(o->localizacao, temp.localizacao) != 0) {
        printf("| Localização:\n");
        printf("|   Antes: %s\n",  o->localizacao);
        printf("|   Depois: %s\n", temp.localizacao);
    }
    if (memcmp(&o->dataHora, &temp.dataHora, sizeof(DataHora)) != 0) {
        printf("| Data/Hora:\n");
        printf("|   Antes: %02d/%02d/%04d %02d:%02d\n",
               o->dataHora.dia, o->dataHora.mes, o->dataHora.ano,
               o->dataHora.hora, o->dataHora.minuto);
        printf("|   Depois: %02d/%02d/%04d %02d:%02d\n",
               temp.dataHora.dia, temp.dataHora.mes, temp.dataHora.ano,
               temp.dataHora.hora, temp.dataHora.minuto);
    }
    if (o->tipo != temp.tipo) {
        printf("| Tipo:\n");
        printf("|   Antes: %s\n",  tipoOcorrenciaParaString(o->tipo));
        printf("|   Depois: %s\n", tipoOcorrenciaParaString(temp.tipo));
    }
    if (o->prioridade != temp.prioridade) {
        printf("| Prioridade:\n");
        printf("|   Antes: %s\n",  prioridadeParaString(o->prioridade));
        printf("|   Depois: %s\n", prioridadeParaString(temp.prioridade));
    }

    printf("==========================================\n\n");

    printf("Confirmar alterações? (1=Sim, 0=Não): ");
    if (getInt(0, 1, "") == 1) {
        *o = temp;
        printf("\nOcorrência atualizada com sucesso!\n");

        char detalhe[200];
        snprintf(detalhe, sizeof(detalhe), "Ocorrencia %d atualizada", id);
        registarLog("INFO", "OCORRENCIAS", "ATUALIZAR", detalhe);
    } else {
        printf("\nAlterações canceladas. Nada foi modificado.\n");
    }

    printf("Prima ENTER para voltar ao menu...");
    getchar();
}


/* ========================================================================= */
/*  INATIVAR OCORRÊNCIA                                                      */
/* ========================================================================= */

void inativarOcorrencia(SistemaGestaoIncendios *sistema) {
    printf("|INATIVAR OCORRÊNCIA|\n\n");

    if (sistema->ocorrencias.tamanho == 0) {
        printf("Não há ocorrências registadas!\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    /* Listar ocorrências ativas antes de pedir o ID — consistência com outros módulos */
    printf("OCORRÊNCIAS ATIVAS:\n");
    printf("==========================================\n");

    int encontrouAtivas = 0;
    for (int i = 0; i < sistema->ocorrencias.tamanho; i++) {
        Ocorrencia *o = &sistema->ocorrencias.dados[i];
        if (o->ativo == 1) {
            printf("ID %d | %-20s | %s\n",
                   o->idOcorrencia,
                   o->localizacao,
                   estadoOcorrenciaParaString(o->estado));
            encontrouAtivas = 1;
        }
    }

    if (!encontrouAtivas) {
        printf("Não existem ocorrências ativas para inativar.\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    printf("==========================================\n\n");

    int id = getInt(1, 9999, "ID da ocorrência que quer inativar");

    Ocorrencia *o = procurarOcorrenciaPorId(sistema, id);

    if (o == NULL) {
        printf("\nID não encontrado!\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    if (o->ativo == 0) {
        printf("\nOcorrência já se encontra inativa.\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    /* Bloquear se tiver intervenção em execução */
    for (int i = 0; i < sistema->intervencoes.tamanho; i++) {
        Intervencao *intv = &sistema->intervencoes.dados[i];
        if (intv->ativo == 1 &&
            intv->idOcorrencia == o->idOcorrencia &&
            intv->estado == EM_EXECUCAO) {

            printf("\nERRO: Esta ocorrência tem uma intervenção em execução.\n");
            printf("   Não é possível inativar ocorrências com intervenções ativas.\n");
            printf("   Intervenção ID: %d (Estado: %s)\n\n",
                   intv->idIntervencao,
                   estadoIntervencaoParaString(intv->estado));
            registarLog("WARNING", "OCORRENCIAS", "INATIVAR",
                        "Tentativa bloqueada - intervenção em execução");
            printf("Prima ENTER para voltar ao menu...");
            getchar();
            return;
        }
    }

    o->ativo = 0;
    printf("\nOcorrência %d inativada com sucesso!\n", id);
    printf("   Local: %s\n", o->localizacao);

    char detalhe[200];
    snprintf(detalhe, sizeof(detalhe), "Ocorrencia %d inativada (estado=%s)",
             id, estadoOcorrenciaParaString(o->estado));
    registarLog("INFO", "OCORRENCIAS", "INATIVAR", detalhe);

    printf("Prima ENTER para voltar ao menu...");
    getchar();

    tentarEliminarOcorrencia(sistema, id);
}


/* ========================================================================= */
/*  PROCURAR OCORRÊNCIA POR ID                                               */
/* ========================================================================= */

Ocorrencia *procurarOcorrenciaPorId(const SistemaGestaoIncendios *sistema, int id) {
    for (int i = 0; i < sistema->ocorrencias.tamanho; i++) {
        if (sistema->ocorrencias.dados[i].idOcorrencia == id) {
            return &sistema->ocorrencias.dados[i];
        }
    }
    return NULL;
}


/* ========================================================================= */
/*  REATIVAR OCORRÊNCIA                                                      */
/* ========================================================================= */

void reativarOcorrencia(SistemaGestaoIncendios *sistema) {
    printf("|REATIVAR OCORRÊNCIA|\n\n");

    if (sistema->ocorrencias.tamanho == 0) {
        printf("Não há ocorrências registadas!\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    printf("OCORRÊNCIAS INATIVAS:\n");
    printf("==========================================\n\n");

    int encontrouInativas = 0;
    for (int i = 0; i < sistema->ocorrencias.tamanho; i++) {
        Ocorrencia *o = &sistema->ocorrencias.dados[i];
        if (o->ativo == 0) {
            printf("ID %d | %-20s | %-10s | Estado: %s\n",
                   o->idOcorrencia,
                   o->localizacao,
                   tipoOcorrenciaParaString(o->tipo),
                   estadoOcorrenciaParaString(o->estado));
            encontrouInativas = 1;
        }
    }

    if (!encontrouInativas) {
        printf("Não há ocorrências inativas!\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    printf("\n");
    int id = getInt(1, 9999, "ID da ocorrência que quer reativar");

    Ocorrencia *o = procurarOcorrenciaPorId(sistema, id);

    if (o == NULL) {
        printf("\nID não encontrado!\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    if (o->ativo == 1) {
        printf("\nOcorrência já está ativa!\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    /* Reativar — mantém o estado que tinha */
    o->ativo = 1;

    printf("\nOcorrência %d reativada com sucesso!\n", id);
    printf("   Local: %s\n",          o->localizacao);
    printf("   Estado mantido: %s\n", estadoOcorrenciaParaString(o->estado));
    printf("\n   A ocorrência foi restaurada exatamente como estava\n");
    printf("   antes de ser inativada.\n\n");

    char detalhe[200];
    snprintf(detalhe, sizeof(detalhe), "Ocorrencia %d reativada (estado=%s)",
             id, estadoOcorrenciaParaString(o->estado));
    registarLog("INFO", "OCORRENCIAS", "REATIVAR", detalhe);

    printf("Prima ENTER para voltar ao menu...");
    getchar();
}


/* ========================================================================= */
/*  TENTAR ELIMINAR OCORRÊNCIA                                               */
/* ========================================================================= */

void tentarEliminarOcorrencia(SistemaGestaoIncendios *sistema, int idOcorrencia) {

    Ocorrencia *o = procurarOcorrenciaPorId(sistema, idOcorrencia);

    /* Só elimina se estiver inativa */
    if (o == NULL || o->ativo != 0)    return;
    /* Só elimina se nunca teve intervenção (estado REPORTADA) */
    if (o->estado != REPORTADA)        return;

    /* Verificar se existe alguma intervenção associada */
    for (int i = 0; i < sistema->intervencoes.tamanho; i++) {
        if (sistema->intervencoes.dados[i].idOcorrencia == idOcorrencia) {
            return; /* Tem relações — não eliminar */
        }
    }

    /* Eliminar fisicamente por "swap and pop" */
    for (int i = 0; i < sistema->ocorrencias.tamanho; i++) {
        if (sistema->ocorrencias.dados[i].idOcorrencia == idOcorrencia) {
            sistema->ocorrencias.dados[i] =
                sistema->ocorrencias.dados[sistema->ocorrencias.tamanho - 1];
            sistema->ocorrencias.tamanho--;
            return;
        }
    }
}