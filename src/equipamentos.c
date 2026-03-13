#include "equipamentos.h"
#include "logs.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

/**
 * @file equipamentos.c
 * @brief Implementação das operações sobre Equipamentos.
 *
 * Funcionalidades:
 *  - Criar novos equipamentos
 *  - Listar equipamentos ativos
 *  - Atualizar designação, localização, tipo e estado
 *  - Inativar e reativar equipamentos
 *  - Consultar equipamento por ID
 *  - Validações quando o equipamento está em intervenção ativa
 */


/* ========================================================================= */
/*  CRIAR EQUIPAMENTO                                                        */
/* ========================================================================= */

int criarEquipamento(SistemaGestaoIncendios *sistema) {
    printf("|CRIAR NOVO EQUIPAMENTO|\n\n");

    Equipamento novo;

    lerString(novo.designacao, MAX_DESIGNACAO, "Inserir Designação");
    lerString(novo.localizacao, MAX_LOCAL,     "Inserir Localização");
    novo.tipo = escolherTipoEquipamento();

    novo.idEquipamento = sistema->proximoIdEquipamento;
    novo.estado        = EQ_DISPONIVEL;
    novo.ativo         = 1;

    if (!expandirListaEquipamentosSeNecessario(&sistema->equipamentos)) {
        printf("\nErro: Não foi possível alocar memória para criar o equipamento!\n");
        registarLog("ERROR", "EQUIPAMENTOS", "CRIAR", "Falha ao expandir lista");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return -1;
    }

    sistema->equipamentos.dados[sistema->equipamentos.tamanho] = novo;
    sistema->equipamentos.tamanho++;
    sistema->proximoIdEquipamento++;

    printf("\nEquipamento criado com sucesso!\n");
    printf("   ID: %d\n",            novo.idEquipamento);
    printf("   Designação: %s\n",    novo.designacao);  /* corrigido: era "Nome" */
    printf("   Localização: %s\n",   novo.localizacao);
    printf("   Tipo: %s\n",          tipoEquipamentoParaString(novo.tipo));
    printf("   Estado: %s\n",        estadoEquipamentoParaString(novo.estado));
    printf("\n");

    char detalhe[200];
    snprintf(detalhe, sizeof(detalhe), "ID=%d;Designacao=%s",
             novo.idEquipamento, novo.designacao);
    registarLog("INFO", "EQUIPAMENTOS", "CRIAR", detalhe); /* corrigido: antes do getchar */

    printf("Prima ENTER para voltar ao menu...");
    getchar();

    return novo.idEquipamento;
}


/* ========================================================================= */
/*  LISTAR EQUIPAMENTOS                                                      */
/* ========================================================================= */

void listarEquipamentos(const SistemaGestaoIncendios *sistema) {

    if (sistema->equipamentos.tamanho == 0) {
        printf("Não existem equipamentos registados!\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    int contadorAtivos = 0;

    for (int i = 0; i < sistema->equipamentos.tamanho; i++) {
        const Equipamento *e = &sistema->equipamentos.dados[i];

        if (e->ativo == 1) {
            contadorAtivos++;
            printf("------------------------------------\n");
            printf("Nrº: %d\n\n", contadorAtivos);
            printf("ID de Equipamento: %d\n", e->idEquipamento);
            printf("Designação: %s\n",        e->designacao);
            printf("Tipo: %s\n",              tipoEquipamentoParaString(e->tipo));
            printf("Estado: %s\n",            estadoEquipamentoParaString(e->estado));
            printf("Localização: %s\n",       e->localizacao); /* corrigido: estava omitida */
            printf("\n");
        }
    }

    if (contadorAtivos == 0) {
        printf("Todos os equipamentos estão inativos!\n"); /* corrigido: faltava \n */
    } else {
        printf("Total de equipamentos ativos: %d\n", contadorAtivos);
    }

    printf("Prima ENTER para voltar ao menu...");
    getchar();

    /* Comentado para evitar logs excessivos em operações de leitura */
    /* registarLog("INFO", "EQUIPAMENTOS", "LISTAR", "Listagem executada"); */
}


/* ========================================================================= */
/*  ATUALIZAR EQUIPAMENTO                                                    */
/* ========================================================================= */

void atualizarEquipamento(SistemaGestaoIncendios *sistema) {
    printf("|ATUALIZAR EQUIPAMENTO|\n\n");

    if (sistema->equipamentos.tamanho == 0) {
        printf("Não existem equipamentos registados.\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    printf("EQUIPAMENTOS ATIVOS:\n");
    printf("==========================================\n");

    int encontrou = 0;
    for (int i = 0; i < sistema->equipamentos.tamanho; i++) {
        Equipamento *eq = &sistema->equipamentos.dados[i];
        if (eq->ativo) {
            printf("ID %d | %-20s | %-12s | %-15s | %s\n",
                   eq->idEquipamento,
                   eq->designacao,
                   tipoEquipamentoParaString(eq->tipo),
                   estadoEquipamentoParaString(eq->estado),
                   eq->localizacao);
            encontrou = 1;
        }
    }

    if (!encontrou) {
        printf("Não existem equipamentos ativos para atualizar.\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    printf("==========================================\n\n");

    int id = getInt(1, 9999, "ID do equipamento a atualizar");

    Equipamento *e = procurarEquipamentoPorId(sistema, id);

    if (e == NULL) {
        printf("\nEquipamento com ID %d não encontrado!\n", id);
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    if (e->ativo == 0) {
        printf("\nEste equipamento está inativo. Não pode ser atualizado.\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    /* Mostrar dados atuais */
    printf("\n==========================================\n");
    printf("           DADOS ATUAIS\n");
    printf("==========================================\n");
    printf("| ID: %d\n",           e->idEquipamento);
    printf("| Designação: %s\n",   e->designacao);
    printf("| Localização: %s\n",  e->localizacao);
    printf("| Tipo: %s\n",         tipoEquipamentoParaString(e->tipo));
    printf("| Estado: %s\n",       estadoEquipamentoParaString(e->estado));
    printf("==========================================\n\n");

    /* Cópia temporária para preview */
    Equipamento temp = *e;
    int alterou = 0;

    printf("ATUALIZAÇÃO DE CAMPOS:\n");
    printf("(Deixe vazio/escolha Não para manter o valor atual)\n\n");

    /* 1. Designação */
    char novaDesignacao[MAX_DESIGNACAO];
    printf("Designação [%s]: ", temp.designacao);
    if (fgets(novaDesignacao, MAX_DESIGNACAO, stdin) != NULL) {
        size_t len = strlen(novaDesignacao);
        if (len > 0 && novaDesignacao[len - 1] == '\n')
            novaDesignacao[len - 1] = '\0';
        if (strlen(novaDesignacao) > 0) {
            strncpy(temp.designacao, novaDesignacao, MAX_DESIGNACAO - 1);
            temp.designacao[MAX_DESIGNACAO - 1] = '\0';
            alterou = 1;
        }
    }

    /* 2. Localização */
    char novaLocalizacao[MAX_LOCAL];
    printf("Localização [%s]: ", temp.localizacao);
    if (fgets(novaLocalizacao, MAX_LOCAL, stdin) != NULL) {
        size_t len = strlen(novaLocalizacao);
        if (len > 0 && novaLocalizacao[len - 1] == '\n')
            novaLocalizacao[len - 1] = '\0';
        if (strlen(novaLocalizacao) > 0) {
            strncpy(temp.localizacao, novaLocalizacao, MAX_LOCAL - 1);
            temp.localizacao[MAX_LOCAL - 1] = '\0';
            alterou = 1;
        }
    }

    /* 3. Tipo */
    printf("\nAlterar tipo? (1=Sim, 0=Não): ");
    if (getInt(0, 1, "") == 1) {
        temp.tipo = escolherTipoEquipamento();
        alterou = 1;
    }

    /* 4. Estado — bloquear se em intervenção em execução */
    printf("\nAlterar estado? (1=Sim, 0=Não): ");
    if (getInt(0, 1, "") == 1) {

        int estaEmIntervencaoAtiva = 0;
        for (int i = 0; i < sistema->intervencoes.tamanho; i++) {
            Intervencao *intv = &sistema->intervencoes.dados[i];
            if (intv->ativo == 1 && intv->estado == EM_EXECUCAO) {
                for (int j = 0; j < intv->numEquipamentos; j++) {
                    if (intv->idsEquipamentos[j] == e->idEquipamento) {
                        estaEmIntervencaoAtiva = 1;
                        break;
                    }
                }
            }
            if (estaEmIntervencaoAtiva) break;
        }

        if (estaEmIntervencaoAtiva) {
            printf("\nAVISO: Este equipamento está numa intervenção ativa!\n");
            printf("O estado não pode ser alterado enquanto a intervenção estiver em execução.\n");
            printf("Estado mantido como: %s\n", estadoEquipamentoParaString(e->estado));
        } else {
            temp.estado = escolherEstadoEquipamento();
            alterou = 1;
        }
    }

    if (!alterou) {
        printf("\nNenhuma alteração foi feita.\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    /* Preview das alterações */
    printf("\n==========================================\n");
    printf("        RESUMO DAS ALTERAÇÕES\n");
    printf("==========================================\n");

    if (strcmp(e->designacao, temp.designacao) != 0) {
        printf("| Designação:\n");
        printf("|   Antes: %s\n",  e->designacao);
        printf("|   Depois: %s\n", temp.designacao);
    }
    if (strcmp(e->localizacao, temp.localizacao) != 0) {
        printf("| Localização:\n");
        printf("|   Antes: %s\n",  e->localizacao);
        printf("|   Depois: %s\n", temp.localizacao);
    }
    if (e->tipo != temp.tipo) {
        printf("| Tipo:\n");
        printf("|   Antes: %s\n",  tipoEquipamentoParaString(e->tipo));
        printf("|   Depois: %s\n", tipoEquipamentoParaString(temp.tipo));
    }
    if (e->estado != temp.estado) {
        printf("| Estado:\n");
        printf("|   Antes: %s\n",  estadoEquipamentoParaString(e->estado));
        printf("|   Depois: %s\n", estadoEquipamentoParaString(temp.estado));
    }

    printf("==========================================\n\n");

    printf("Confirmar alterações? (1=Sim, 0=Não): ");
    if (getInt(0, 1, "") == 1) {
        *e = temp;
        printf("\nEquipamento atualizado com sucesso!\n");

        char detalhe[200];
        snprintf(detalhe, sizeof(detalhe), "ID=%d atualizado", e->idEquipamento);
        registarLog("INFO", "EQUIPAMENTOS", "ATUALIZAR", detalhe);
    } else {
        printf("\nAlterações canceladas. Nada foi modificado.\n");
    }

    printf("Prima ENTER para voltar ao menu...");
    getchar();
}


/* ========================================================================= */
/*  INATIVAR EQUIPAMENTO                                                     */
/* ========================================================================= */

void inativarEquipamento(SistemaGestaoIncendios *sistema) {
    printf("|INATIVAR EQUIPAMENTO|\n\n");

    if (sistema->equipamentos.tamanho == 0) {
        printf("Não há equipamentos registados!\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    /* Listar equipamentos ativos antes de pedir o ID — consistência com outros módulos */
    printf("EQUIPAMENTOS ATIVOS:\n");
    printf("==========================================\n");

    int encontrouAtivos = 0;
    for (int i = 0; i < sistema->equipamentos.tamanho; i++) {
        Equipamento *eq = &sistema->equipamentos.dados[i];
        if (eq->ativo == 1) {
            printf("ID %d | %-28s | %s\n",
                   eq->idEquipamento,
                   eq->designacao,
                   estadoEquipamentoParaString(eq->estado));
            encontrouAtivos = 1;
        }
    }

    if (!encontrouAtivos) {
        printf("Não existem equipamentos ativos para inativar.\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    printf("==========================================\n\n");

    int id = getInt(1, 9999, "ID do equipamento que quer inativar"); /* corrigido: texto consistente */

    Equipamento *e = procurarEquipamentoPorId(sistema, id);

    if (e == NULL) {
        printf("\nID não encontrado!\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    if (e->ativo == 0) {
        printf("\nEquipamento já se encontra inativo.\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    /* Bloquear se estiver em intervenção EM_EXECUCAO */
    for (int i = 0; i < sistema->intervencoes.tamanho; i++) {
        Intervencao *intv = &sistema->intervencoes.dados[i];
        if (intv->ativo == 1 && intv->estado == EM_EXECUCAO) {
            for (int j = 0; j < intv->numEquipamentos; j++) {
                if (intv->idsEquipamentos[j] == e->idEquipamento) {
                    printf("\nErro: O equipamento está em uso numa intervenção em execução.\n");
                    printf("Não é possível inativar equipamentos em execução.\n");
                    printf("Prima ENTER para voltar ao menu...");
                    getchar();
                    return;
                }
            }
        }
    }

    e->ativo = 0;
    printf("\nEquipamento %d inativado com sucesso!\n", id);
    printf("   Designação: %s\n", e->designacao);

    char detalhe[200];
    snprintf(detalhe, sizeof(detalhe), "Equipamento %d inativado", id);
    registarLog("INFO", "EQUIPAMENTOS", "INATIVAR", detalhe); /* corrigido: antes do getchar */

    printf("Prima ENTER para voltar ao menu...");
    getchar();

    tentarEliminarEquipamento(sistema, id);
}


/* ========================================================================= */
/*  PROCURAR EQUIPAMENTO POR ID                                              */
/* ========================================================================= */

Equipamento *procurarEquipamentoPorId(const SistemaGestaoIncendios *sistema, int id) {
    for (int i = 0; i < sistema->equipamentos.tamanho; i++) {
        if (sistema->equipamentos.dados[i].idEquipamento == id) {
            return &sistema->equipamentos.dados[i];
        }
    }
    return NULL;
}


/* ========================================================================= */
/*  REATIVAR EQUIPAMENTO                                                     */
/* ========================================================================= */

void reativarEquipamento(SistemaGestaoIncendios *sistema) {
    printf("|REATIVAR EQUIPAMENTO|\n\n");

    if (sistema->equipamentos.tamanho == 0) {
        printf("Não há equipamentos registados!\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    printf("EQUIPAMENTOS INATIVOS:\n\n");
    int encontrouInativos = 0;

    for (int i = 0; i < sistema->equipamentos.tamanho; i++) {
        const Equipamento *e = &sistema->equipamentos.dados[i];
        if (e->ativo == 0) {
            printf("ID %d | %-28s | %s | Local: %s\n",
                   e->idEquipamento,
                   e->designacao,
                   tipoEquipamentoParaString(e->tipo),
                   e->localizacao);
            encontrouInativos = 1;
        }
    }

    if (!encontrouInativos) {
        printf("Não há equipamentos inativos!\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    printf("\n");
    int id = getInt(1, 9999, "ID do equipamento que quer reativar");

    Equipamento *e = procurarEquipamentoPorId(sistema, id);

    if (e == NULL) {
        printf("\nID não encontrado!\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    if (e->ativo == 1) {
        printf("\nEquipamento já está ativo!\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    e->ativo  = 1;
    e->estado = EQ_DISPONIVEL; /* corrigido: sempre DISPONIVEL ao reativar */

    printf("\nEquipamento %d reativado com sucesso!\n", id);
    printf("   Designação: %s\n", e->designacao);
    printf("   Estado: %s\n",     estadoEquipamentoParaString(e->estado));

    char detalhe[200];
    snprintf(detalhe, sizeof(detalhe), "Equipamento %d reativado", id);
    registarLog("INFO", "EQUIPAMENTOS", "REATIVAR", detalhe);

    printf("\nPrima ENTER para voltar ao menu...");
    getchar();
}


/* ========================================================================= */
/*  TENTAR ELIMINAR EQUIPAMENTO                                              */
/* ========================================================================= */

void tentarEliminarEquipamento(SistemaGestaoIncendios *sistema, int idEquipamento) {

    Equipamento *e = procurarEquipamentoPorId(sistema, idEquipamento);
    if (e == NULL || e->ativo != 0) return; /* Só elimina se estiver inativo */

    /* Verificar se existe alguma intervenção com este equipamento */
    for (int i = 0; i < sistema->intervencoes.tamanho; i++) {
        Intervencao *intv = &sistema->intervencoes.dados[i];
        for (int j = 0; j < intv->numEquipamentos; j++) {
            if (intv->idsEquipamentos[j] == idEquipamento) {
                return; /* Tem relações — não eliminar */
            }
        }
    }

    /* Eliminar fisicamente por substituição pelo último elemento */
    for (int i = 0; i < sistema->equipamentos.tamanho; i++) {
        if (sistema->equipamentos.dados[i].idEquipamento == idEquipamento) {
            sistema->equipamentos.dados[i] =
                sistema->equipamentos.dados[sistema->equipamentos.tamanho - 1];
            sistema->equipamentos.tamanho--;
            return;
        }
    }
}