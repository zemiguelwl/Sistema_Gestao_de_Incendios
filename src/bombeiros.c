#include "bombeiros.h"
#include "logs.h"
#include "utils.h"
#include <stdio.h>
#include <string.h>

/**
 * @file bombeiros.c
 * @brief Implementação das operações sobre Bombeiros.
 *
 * Funcionalidades:
 *  - Criar novos bombeiros
 *  - Listar bombeiros ativos
 *  - Atualizar dados, especialidade e estado
 *  - Inativar e reativar bombeiros
 *  - Consultar bombeiro por ID
 *  - Validações quando o bombeiro está em intervenção ativa
 */


/* ========================================================================= */
/*  CRIAR BOMBEIRO                                                           */
/* ========================================================================= */

int criarBombeiro(SistemaGestaoIncendios *sistema) {
    printf("|CRIAR NOVO BOMBEIRO|\n\n");

    Bombeiro novo;

    lerString(novo.nome, MAX_NOME, "Inserir Nome");
    novo.especialidade = escolherEspecialidadeBombeiro();

    novo.idBombeiro = sistema->proximoIdBombeiro;
    novo.estado     = DISPONIVEL;
    novo.ativo      = 1;

    if (!expandirListaBombeirosSeNecessario(&sistema->bombeiros)) {
        printf("\nErro: Não foi possível alocar memória para criar o bombeiro!\n");
        registarLog("ERROR", "BOMBEIROS", "CRIAR", "Falha ao expandir lista");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return -1;
    }

    sistema->bombeiros.dados[sistema->bombeiros.tamanho] = novo;
    sistema->bombeiros.tamanho++;
    sistema->proximoIdBombeiro++;

    printf("\nBombeiro criado com sucesso!\n");
    printf("   ID: %d\n",           novo.idBombeiro);
    printf("   Nome: %s\n",         novo.nome);
    printf("   Especialidade: %s\n", especialidadeBombeiroParaString(novo.especialidade));
    printf("   Estado: %s\n",       estadoBombeiroParaString(novo.estado));
    printf("\n");

    char detalhe[200];
    snprintf(detalhe, sizeof(detalhe), "ID=%d;Nome=%s", novo.idBombeiro, novo.nome);
    registarLog("INFO", "BOMBEIROS", "CRIAR", detalhe);

    printf("Prima ENTER para voltar ao menu...");
    getchar();

    return novo.idBombeiro;
}


/* ========================================================================= */
/*  LISTAR BOMBEIROS                                                         */
/* ========================================================================= */

void listarBombeiros(const SistemaGestaoIncendios *sistema) {

    if (sistema->bombeiros.tamanho == 0) {
        printf("Não existem bombeiros registados!\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    int contadorAtivos = 0;

    for (int i = 0; i < sistema->bombeiros.tamanho; i++) {
        if (sistema->bombeiros.dados[i].ativo == 1) {
            contadorAtivos++;
            printf("------------------------------------\n");
            printf("Nrº: %d\n\n", contadorAtivos);
            printf("ID de Bombeiro: %d\n", sistema->bombeiros.dados[i].idBombeiro);
            printf("Nome: %s\n",           sistema->bombeiros.dados[i].nome);
            printf("Especialidade: %s\n",
                   especialidadeBombeiroParaString(sistema->bombeiros.dados[i].especialidade));
            printf("Estado: %s\n",
                   estadoBombeiroParaString(sistema->bombeiros.dados[i].estado));
            printf("\n");
        }
    }

    if (contadorAtivos == 0) {
        printf("Todos os bombeiros estão inativos!\n");
    } else {
        printf("Total de bombeiros ativos: %d\n", contadorAtivos);
    }

    printf("Prima ENTER para voltar ao menu...");
    getchar();

    /* Log comentado para evitar logs excessivos em operações de leitura */
    /* registarLog("INFO", "BOMBEIROS", "LISTAR", "Listagem executada"); */
}


/* ========================================================================= */
/*  ATUALIZAR BOMBEIRO                                                       */
/* ========================================================================= */

void atualizarBombeiro(SistemaGestaoIncendios *sistema) {
    printf("|ATUALIZAR BOMBEIRO|\n\n");

    if (sistema->bombeiros.tamanho == 0) {
        printf("Não existem bombeiros registados.\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    /* Listar bombeiros ativos antes de pedir o ID */
    printf("BOMBEIROS ATIVOS:\n");
    printf("==========================================\n");

    int encontrouAtivos = 0;
    for (int i = 0; i < sistema->bombeiros.tamanho; i++) {
        Bombeiro *b = &sistema->bombeiros.dados[i];
        if (b->ativo == 1) {
            printf("ID %d | %-20s | %-20s | %s\n",
                   b->idBombeiro,
                   b->nome,
                   especialidadeBombeiroParaString(b->especialidade),
                   estadoBombeiroParaString(b->estado));
            encontrouAtivos = 1;
        }
    }

    if (!encontrouAtivos) {
        printf("Não existem bombeiros ativos para atualizar.\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    printf("==========================================\n\n");

    int id = getInt(1, 9999, "ID do bombeiro a atualizar");

    Bombeiro *b = procurarBombeiroPorId(sistema, id);

    if (b == NULL) {
        printf("\nBombeiro com ID %d não encontrado!\n", id);
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    if (b->ativo == 0) {
        printf("\nEste bombeiro está inativo. Não pode ser atualizado.\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    /* Mostrar dados atuais */
    printf("\n==========================================\n");
    printf("          DADOS ATUAIS\n");
    printf("==========================================\n");
    printf("| ID: %d\n",              b->idBombeiro);
    printf("| Nome: %s\n",            b->nome);
    printf("| Especialidade: %s\n",   especialidadeBombeiroParaString(b->especialidade));
    printf("| Estado: %s\n",          estadoBombeiroParaString(b->estado));
    printf("==========================================\n\n");

    /* Cópia temporária para preview */
    Bombeiro temp = *b;
    int alterou = 0;

    printf("ATUALIZAÇÃO DE CAMPOS:\n");
    printf("(Deixe vazio/escolha Não para manter o valor atual)\n\n");

    /* 1. Nome */
    char novoNome[MAX_NOME];
    printf("Nome [%s]: ", temp.nome);
    if (fgets(novoNome, MAX_NOME, stdin) != NULL) {
        size_t len = strlen(novoNome);
        if (len > 0 && novoNome[len - 1] == '\n') {
            novoNome[len - 1] = '\0';
        }
        if (strlen(novoNome) > 0) {
            strncpy(temp.nome, novoNome, MAX_NOME - 1);
            temp.nome[MAX_NOME - 1] = '\0';
            alterou = 1;
        }
    }

    /* 2. Especialidade */
    printf("\nAlterar especialidade? (1=Sim, 0=Não): ");
    if (getInt(0, 1, "") == 1) {
        temp.especialidade = escolherEspecialidadeBombeiro();
        alterou = 1;
    }

    /* 3. Estado — bloquear apenas se estiver em intervenção EM_EXECUCAO.
     *    Intervenções EM_PLANEAMENTO não implicam mobilização do bombeiro,
     *    pelo que o estado pode ser alterado livremente nesse caso. */
    printf("\nAlterar estado? (1=Sim, 0=Não): ");
    if (getInt(0, 1, "") == 1) {

        int estaEmExecucao = 0;
        for (int i = 0; i < sistema->intervencoes.tamanho; i++) {
            Intervencao *intv = &sistema->intervencoes.dados[i];
            if (intv->ativo == 1 && intv->estado == EM_EXECUCAO) {
                for (int j = 0; j < intv->numBombeiros; j++) {
                    if (intv->idsBombeiros[j] == b->idBombeiro) {
                        estaEmExecucao = 1;
                        break;
                    }
                }
            }
            if (estaEmExecucao) break;
        }

        if (estaEmExecucao) {
            printf("\nAVISO: Este bombeiro está numa intervenção em execução!\n");
            printf("Não é possível alterar o estado enquanto a intervenção estiver ativa.\n");
            printf("Estado mantido como: %s\n", estadoBombeiroParaString(b->estado));
        } else {
            temp.estado = escolherEstadoBombeiro();
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

    if (strcmp(b->nome, temp.nome) != 0) {
        printf("| Nome:\n");
        printf("|   Antes: %s\n",  b->nome);
        printf("|   Depois: %s\n", temp.nome);
    }
    if (b->especialidade != temp.especialidade) {
        printf("| Especialidade:\n");
        printf("|   Antes: %s\n",  especialidadeBombeiroParaString(b->especialidade));
        printf("|   Depois: %s\n", especialidadeBombeiroParaString(temp.especialidade));
    }
    if (b->estado != temp.estado) {
        printf("| Estado:\n");
        printf("|   Antes: %s\n",  estadoBombeiroParaString(b->estado));
        printf("|   Depois: %s\n", estadoBombeiroParaString(temp.estado));
    }

    printf("==========================================\n\n");

    printf("Confirmar alterações? (1=Sim, 0=Não): ");
    if (getInt(0, 1, "") == 1) {
        *b = temp;
        printf("\nBombeiro atualizado com sucesso!\n");

        char detalhe[200];
        snprintf(detalhe, sizeof(detalhe), "ID=%d atualizado", b->idBombeiro);
        registarLog("INFO", "BOMBEIROS", "ATUALIZAR", detalhe);
    } else {
        printf("\nAlterações canceladas. Nada foi modificado.\n");
    }

    printf("Prima ENTER para voltar ao menu...");
    getchar();
}


/* ========================================================================= */
/*  INATIVAR BOMBEIRO                                                        */
/* ========================================================================= */

void inativarBombeiro(SistemaGestaoIncendios *sistema) {
    printf("|INATIVAR BOMBEIRO|\n\n");

    if (sistema->bombeiros.tamanho == 0) {
        printf("Não há bombeiros registados!\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    /* Listar bombeiros ativos antes de pedir o ID — consistência com outros módulos */
    printf("BOMBEIROS ATIVOS:\n");
    printf("==========================================\n");

    int encontrouAtivos = 0;
    for (int i = 0; i < sistema->bombeiros.tamanho; i++) {
        Bombeiro *b = &sistema->bombeiros.dados[i];
        if (b->ativo == 1) {
            printf("ID %d | %-20s | %s\n",
                   b->idBombeiro,
                   b->nome,
                   estadoBombeiroParaString(b->estado));
            encontrouAtivos = 1;
        }
    }

    if (!encontrouAtivos) {
        printf("Não existem bombeiros ativos para inativar.\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    printf("==========================================\n\n");

    int id = getInt(1, 9999, "ID do bombeiro que quer inativar");

    Bombeiro *b = procurarBombeiroPorId(sistema, id);

    if (b == NULL) {
        printf("\nID não encontrado!\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    if (b->ativo == 0) {
        printf("\nBombeiro já se encontra inativo.\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    /* Bloquear se estiver em intervenção EM_EXECUCAO */
    for (int i = 0; i < sistema->intervencoes.tamanho; i++) {
        Intervencao *intv = &sistema->intervencoes.dados[i];
        if (intv->ativo == 1 && intv->estado == EM_EXECUCAO) {
            for (int j = 0; j < intv->numBombeiros; j++) {
                if (intv->idsBombeiros[j] == b->idBombeiro) {
                    printf("\nErro: O bombeiro está em intervenção em execução.\n");
                    printf("Não é possível inativar bombeiros em execução.\n");
                    printf("Prima ENTER para voltar ao menu...");
                    getchar();
                    return;
                }
            }
        }
    }

    b->ativo = 0;
    printf("\nBombeiro %d inativado com sucesso!\n", id);

    char detalhe[200];
    snprintf(detalhe, sizeof(detalhe), "Bombeiro %d inativado", id);
    registarLog("INFO", "BOMBEIROS", "INATIVAR", detalhe);

    printf("Prima ENTER para voltar ao menu...");
    getchar();

    /* Tentar eliminação física se não tiver relações */
    tentarEliminarBombeiro(sistema, id);
}


/* ========================================================================= */
/*  PROCURAR BOMBEIRO POR ID                                                 */
/* ========================================================================= */

Bombeiro *procurarBombeiroPorId(const SistemaGestaoIncendios *sistema, int id) {
    for (int i = 0; i < sistema->bombeiros.tamanho; i++) {
        if (sistema->bombeiros.dados[i].idBombeiro == id) {
            return &sistema->bombeiros.dados[i];
        }
    }
    return NULL;
}


/* ========================================================================= */
/*  REATIVAR BOMBEIRO                                                        */
/* ========================================================================= */

void reativarBombeiro(SistemaGestaoIncendios *sistema) {
    printf("|REATIVAR BOMBEIRO|\n\n");

    if (sistema->bombeiros.tamanho == 0) {
        printf("Não há bombeiros registados!\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    printf("BOMBEIROS INATIVOS:\n\n");
    int encontrouInativos = 0;

    for (int i = 0; i < sistema->bombeiros.tamanho; i++) {
        if (sistema->bombeiros.dados[i].ativo == 0) {
            printf("ID %d | %s | %s\n",
                   sistema->bombeiros.dados[i].idBombeiro,
                   sistema->bombeiros.dados[i].nome,
                   especialidadeBombeiroParaString(sistema->bombeiros.dados[i].especialidade));
            encontrouInativos = 1;
        }
    }

    if (!encontrouInativos) {
        printf("Não há bombeiros inativos!\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    printf("\n");
    int id = getInt(1, 9999, "ID do bombeiro que quer reativar");

    Bombeiro *b = procurarBombeiroPorId(sistema, id);

    if (b == NULL) {
        printf("\nID não encontrado!\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    if (b->ativo == 1) {
        printf("\nBombeiro já está ativo!\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    b->ativo  = 1;
    b->estado = DISPONIVEL;

    printf("\nBombeiro %d reativado com sucesso!\n", id);
    printf("   Nome: %s\n",   b->nome);
    printf("   Estado: %s\n", estadoBombeiroParaString(b->estado));

    char detalhe[200];
    snprintf(detalhe, sizeof(detalhe), "Bombeiro %d reativado", id);
    registarLog("INFO", "BOMBEIROS", "REATIVAR", detalhe);

    printf("\nPrima ENTER para voltar ao menu...");
    getchar();
}


/* ========================================================================= */
/*  TENTAR ELIMINAR BOMBEIRO                                                 */
/* ========================================================================= */

void tentarEliminarBombeiro(SistemaGestaoIncendios *sistema, int idBombeiro) {

    Bombeiro *b = procurarBombeiroPorId(sistema, idBombeiro);
    if (b == NULL || b->ativo != 0) return; /* Só elimina se estiver inativo */

    /* Verificar se existe alguma intervenção com este bombeiro */
    for (int i = 0; i < sistema->intervencoes.tamanho; i++) {
        Intervencao *intv = &sistema->intervencoes.dados[i];
        for (int j = 0; j < intv->numBombeiros; j++) {
            if (intv->idsBombeiros[j] == idBombeiro) {
                return; /* Tem relações — não eliminar */
            }
        }
    }

    /* Eliminar fisicamente por "swap and pop" */
    for (int i = 0; i < sistema->bombeiros.tamanho; i++) {
        if (sistema->bombeiros.dados[i].idBombeiro == idBombeiro) {
            sistema->bombeiros.dados[i] =
                sistema->bombeiros.dados[sistema->bombeiros.tamanho - 1];
            sistema->bombeiros.tamanho--;
            return;
        }
    }
}