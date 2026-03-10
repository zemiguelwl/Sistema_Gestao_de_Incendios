#include "equipamentos.h"
#include "logs.h"
#include <stdio.h>
#include <string.h>

#include "utils.h"

/**
 * @file equipamentos.c
 * @brief Implementação das operações sobre Equipamentos.
 *
 * Funcionalidades principais:
 *  - Criar novos equipamentos
 *  - Listar equipamentos ativos
 *  - Atualizar dados e estado operacional
 *  - Inativar e reativar
 *  - Consultar equipamento por ID
 *  - Validações associadas a intervenções ativas
 */



/*  CRIAR EQUIPAMENTO        */
int criarEquipamento(SistemaGestaoIncendios *sistema) {
    printf("|CRIAR NOVO EQUIPAMENTO|\n");
    printf("\n");

    Equipamento novo;

    lerString(novo.designacao, MAX_DESIGNACAO, "Inserir Nome");

    lerString(novo.localizacao, MAX_LOCAL, "Inserir Localização");

    novo.tipo = escolherTipoEquipamento();

    novo.idEquipamento = sistema->proximoIdEquipamento;
    novo.estado = EQ_DISPONIVEL;
    novo.ativo = 1;

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
    printf("   ID: %d\n", novo.idEquipamento);
    printf("   Nome: %s\n", novo.designacao);
    printf("   Localização: %s\n", novo.localizacao);
    printf("   Tipo: %s\n", tipoEquipamentoParaString(novo.tipo));
    printf("   Estado: %s\n", estadoEquipamentoParaString(novo.estado));
    printf("\n");

    printf("Prima ENTER para voltar ao menu...");
    getchar();

    char detalhe[200];
    sprintf(detalhe, "ID=%d;Designação=%s", novo.idEquipamento, novo.designacao);
    registarLog("INFO", "EQUIPAMENTOS", "CRIAR", detalhe);

    return novo.idEquipamento;
}


/*  LISTAR EQUIPAMENTO        */
void listarEquipamentos(const SistemaGestaoIncendios *sistema) {
    if (sistema->equipamentos.tamanho == 0) {
        printf("Não existem equipamentos registados!\n");
        return;
    }
    int contadorAtivas = 0;

    for (int i = 0; i < sistema->equipamentos.tamanho; i++) {

        if(sistema->equipamentos.dados[i].ativo == 1){
            contadorAtivas++;

            printf("------------------------------------\n");
            printf("Nrº: %d\n", contadorAtivas);
            printf("\n");
            printf("ID de Equipamento: %d\n", sistema->equipamentos.dados[i].idEquipamento);
            printf("Designação: %s\n", sistema->equipamentos.dados[i].designacao);

            printf("Tipo: %s\n",
       tipoEquipamentoParaString(sistema->equipamentos.dados[i].tipo));

            printf("Estado: %s\n",
                   estadoEquipamentoParaString(sistema->equipamentos.dados[i].estado));

            printf("\n");
            printf("\n");
        }
    }
    if (contadorAtivas == 0) {
        printf("Todos os equipamentos estão inativos!");
    }else {
        printf("Total de equipamentos: %d\n", contadorAtivas);
    }

    printf("Prima ENTER para voltar ao menu...");
    getchar();

    //registarLog("INFO", "EQUIPAMENTOS", "LISTAR", "LISTAGEM EXECUTADA!");
    // (Comentado para evitar excesso de logs. Útil apenas em debugging.)
}


/*  ATUALIZAR EQUIPAMENTO         */
void atualizarEquipamento(SistemaGestaoIncendios *sistema) {
    printf("|ATUALIZAR EQUIPAMENTO|\n");
    printf("\n");

    /* Verificar se existem equipamentos */
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

    printf("==========================================\n");

    if (!encontrou) {
        printf("Não existem equipamentos ativos.\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    /* Pedir ID */
    int id = getInt(1, 9999, "ID do equipamento a atualizar");

    /* Procurar equipamento */
    Equipamento *e = procurarEquipamentoPorId(sistema, id);

    if (e == NULL) {
        printf("\nEquipamento com ID %d não encontrado!\n", id);
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    /* Verificar se está ativo */
    if (e->ativo == 0) {
        printf("\nEste equipamento está inativo. Não pode ser atualizado.\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    /* Mostrar dados atuais */
    printf("\n");
    printf("==========================================\n");
    printf("           DADOS ATUAIS                    \n");
    printf("==========================================\n");
    printf("| ID: %d\n", e->idEquipamento);
    printf("| Nome: %s\n", e->designacao);
    printf("| Localização: %s\n", e->localizacao);
    printf("| Tipo: %s\n", tipoEquipamentoParaString(e->tipo));
    printf("| Estado: %s\n", estadoEquipamentoParaString(e->estado));
    printf("==========================================\n");
    printf("\n");

    /* Criar cópia temporária */
    Equipamento temp = *e;
    int alterou = 0;

    /* Pedir novos valores */
    printf("ATUALIZAÇÃO DE CAMPOS:\n");
    printf("(Deixe vazio/escolha Não para manter o atual)\n\n");

    /* 1. Designação */
    char novaDesignacao[MAX_DESIGNACAO];
    printf("Designação [%s]: ", temp.designacao);
    if (fgets(novaDesignacao, MAX_DESIGNACAO, stdin) != NULL) {

        size_t len = strlen(novaDesignacao);
        if (len > 0 && novaDesignacao[len - 1] == '\n') {
            novaDesignacao[len - 1] = '\0';
        }

        /* Se não está vazio, atualizar */
        if (strlen(novaDesignacao) > 0) {
            strcpy(temp.designacao, novaDesignacao);
            alterou = 1;
        }
    }

    /* 2. Localização */
    char novaLocalizacao[MAX_LOCAL];
    printf("Localização [%s]: ", temp.localizacao);
    if (fgets(novaLocalizacao, MAX_LOCAL, stdin) != NULL) {

        size_t len = strlen(novaLocalizacao);
        if (len > 0 && novaLocalizacao[len - 1] == '\n') {
            novaLocalizacao[len - 1] = '\0';
        }

        /* Se não está vazio, atualizar */
        if (strlen(novaLocalizacao) > 0) {
            strcpy(temp.localizacao, novaLocalizacao);
            alterou = 1;
        }
    }

    /* 3. Tipo */
    printf("\nAlterar tipo? (1=Sim, 0=Não): ");
    if (getInt(0, 1, "") == 1) {
        temp.tipo = escolherTipoEquipamento();
        alterou = 1;
    }

    /* 4. Estado */
    printf("\nAlterar estado? (1=Sim, 0=Não): ");
    if (getInt(0, 1, "") == 1) {

        /* verificar se está em intervenção ativa */
        int estaEmIntervencaoAtiva = 0;

        for (int i = 0; i < sistema->intervencoes.tamanho; i++) {
            Intervencao *intv = &sistema->intervencoes.dados[i];

            if (intv->ativo == 1 && intv->estado == EM_EXECUCAO) {
                /* Verificar se este equipamento está nesta intervenção */
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
            printf("\n AVISO: Este equipamento está numa intervenção ativa!\n");
            printf("O estado não pode ser alterado enquanto a intervenção estiver em execução.\n");
            /* Só permite escolher EQ_EM_USO */
            printf("Estado mantido como: %s\n", estadoEquipamentoParaString(e->estado));

        } else {
            /* Pode alterar */
            printf("\nNovo estado:\n");
            temp.estado = escolherEstadoEquipamento();
            alterou = 1;
        }
    }

    /* Se não houve alterações */
    if (!alterou) {
        printf("\nNenhuma alteração foi feita.\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    /* Mostrar resumo das alterações */
    printf("\n");
    printf("==========================================\n");
    printf("       RESUMO DAS ALTERAÇÕES             \n");
    printf("==========================================\n");

    /* Comparar cada campo */
    if (strcmp(e->designacao, temp.designacao) != 0) {
        printf("| Designação:\n");
        printf("|   Antes: %s\n", e->designacao);
        printf("|   Depois: %s\n", temp.designacao);
    }

    if (strcmp(e->localizacao, temp.localizacao) != 0) {
        printf("| Localização:\n");
        printf("|   Antes: %s\n", e->localizacao);
        printf("|   Depois: %s\n", temp.localizacao);
    }

    if (e->tipo != temp.tipo) {
        printf("| Tipo:\n");
        printf("|   Antes: %s\n", tipoEquipamentoParaString(e->tipo));
        printf("|   Depois: %s\n", tipoEquipamentoParaString(temp.tipo));
    }

    if (e->estado != temp.estado) {
        printf("| Estado:\n");
        printf("|   Antes: %s\n", estadoEquipamentoParaString(e->estado));
        printf("|   Depois: %s\n", estadoEquipamentoParaString(temp.estado));
    }

    printf("==========================================\n");
    printf("\n");

    /* Confirmar alterações */
    printf("Confirmar alterações? (1=Sim, 0=Não): ");
    if (getInt(0, 1, "") == 1) {

        *e = temp;

        printf("\n Equipamento atualizado com sucesso!\n");

        char detalhe[200];
        sprintf(detalhe, "ID=%d atualizado", e->idEquipamento);
        registarLog("INFO", "EQUIPAMENTOS", "ATUALIZAR", detalhe);
    } else {
        printf("\nAlterações canceladas. Nada foi modificado.\n");
    }

    printf("Prima ENTER para voltar ao menu...");
    getchar();
}


/*  INATIVAR EQUIPAMENTO           */
void inativarEquipamento(SistemaGestaoIncendios *sistema) {
    printf("|INATIVAR EQUIPAMENTO|\n");
    printf("\n");

    if (sistema->equipamentos.tamanho == 0){
        printf("Não há equipamentos registados!\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    int id = getInt(1, 9999, "Insira o ID do equipamento que quer inativar: ");

    Equipamento *e = procurarEquipamentoPorId(sistema, id);

    if (e == NULL) {
        printf("ID não encontrado!");
        return;
    }

    if (e->ativo == 0) {
        printf("Equipamento já se encontra inativo\n");
        return;
    }

    /* Verificar se o equipamento está em intervenção EM_EXECUCAO */
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

    /* Inativar */
    e->ativo = 0;
    printf("Equipamento %d inativado com sucesso!\n", id);

    printf("Prima ENTER para voltar ao menu...");
    getchar();

    char detalhe[200];
    sprintf(detalhe, "Equipamento %d inativado", id);
    registarLog("INFO", "EQUIPAMENTOS", "INATIVAR", detalhe);

    tentarEliminarEquipamento(sistema, id);
}


/*  PROCURAR EQUIPAMENTO POR ID          */
Equipamento* procurarEquipamentoPorId(const SistemaGestaoIncendios *sistema, int id) {

    for (int i = 0; i < sistema->equipamentos.tamanho; i++) {
        if (sistema->equipamentos.dados[i].idEquipamento == id) {
            return &sistema->equipamentos.dados[i];
        }
    }
    return NULL;
}


/*  REATIVAR EQUIPAMENTO           */
void reativarEquipamento(SistemaGestaoIncendios *sistema) {
    printf("|REATIVAR EQUIPAMENTO|\n");
    printf("\n");

    if (sistema->equipamentos.tamanho == 0) {
        printf("Não há equipamentos registados!\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    /* Mostrar equipamentos inativos */
    printf("EQUIPAMENTOS INATIVOS:\n\n");
    int encontrouInativos = 0;

    for (int i = 0; i < sistema->equipamentos.tamanho; i++) {
        if (sistema->equipamentos.dados[i].ativo == 0) {
            printf("ID %d | %s | %s | Local: %s\n",
                   sistema->equipamentos.dados[i].idEquipamento,
                   sistema->equipamentos.dados[i].designacao,
                   tipoEquipamentoParaString(sistema->equipamentos.dados[i].tipo),
                   sistema->equipamentos.dados[i].localizacao);
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
    int id = getInt(1, 9999, "Insira o ID do equipamento que quer reativar");

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


    /* Reativar equipamento */
    e->ativo = 1;

    /* Ao reativar, o equipamento volta a estar disponível */
    if (e->estado != EQ_EM_USO) {
        e->estado = EQ_DISPONIVEL;
    }

    printf("\n Equipamento %d reativado com sucesso!\n", id);
    printf("   Designação: %s\n", e->designacao);
    printf("   Estado: %s\n", estadoEquipamentoParaString(e->estado));

    printf("\nPrima ENTER para voltar ao menu...");
    getchar();

    char detalhe[200];
    sprintf(detalhe, "Equipamento %d reativado", id);
    registarLog("INFO", "EQUIPAMENTOS", "REATIVAR", detalhe);
}

void tentarEliminarEquipamento(SistemaGestaoIncendios *sistema, int idEquipamento) {

    Equipamento *e = procurarEquipamentoPorId(sistema, idEquipamento);
    if (!e || e->ativo != 0) return;  // só elimina se estiver inativo

    for (int i = 0; i < sistema->intervencoes.tamanho; i++) {
        Intervencao *intv = &sistema->intervencoes.dados[i];

        for (int j = 0; j < intv->numEquipamentos; j++) {
            if (intv->idsEquipamentos[j] == idEquipamento) {
                return;
            }
        }
    }

    for (int i = 0; i < sistema->equipamentos.tamanho; i++) {
        if (sistema->equipamentos.dados[i].idEquipamento == idEquipamento) {
            sistema->equipamentos.dados[i] =
                sistema->equipamentos.dados[sistema->equipamentos.tamanho - 1];
            sistema->equipamentos.tamanho--;
            return;
        }
    }
}

