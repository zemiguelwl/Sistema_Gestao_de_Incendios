#include "bombeiros.h"
#include "logs.h"
#include <stdio.h>
#include <string.h>

#include "utils.h"

/**
 * @file bombeiros.c
 * @brief Implementação das operações sobre Bombeiros.
 *
 * Funcionalidades principais:
 *  - Criar novos bombeiros
 *  - Listar bombeiros ativos
 *  - Atualizar dados, especialidade e estado
 *  - Inativar e reativar bombeiros
 *  - Consultar bombeiro por ID
 *  - Validações quando o bombeiro está em intervenção ativa
 */



/*  CRIAR BOMBEIRO       */
int criarBombeiro(SistemaGestaoIncendios *sistema) {
    printf("|CRIAR NOVO BOMBEIRO|\n");
    printf("\n");

    Bombeiro novo;

    lerString(novo.nome, MAX_NOME, "Inserir Nome");

    novo.especialidade = escolherEspecialidadeBombeiro();

    novo.idBombeiro = sistema->proximoIdBombeiro;
    novo.estado = DISPONIVEL;
    novo.ativo = 1;

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
    printf("   ID: %d\n", novo.idBombeiro);
    printf("   Nome: %s\n", novo.nome);
    printf("   Especialidade: %s\n", especialidadeBombeiroParaString(novo.especialidade));
    printf("   Estado: %s\n", estadoBombeiroParaString(novo.estado));
    printf("\n");

    printf("Prima ENTER para voltar ao menu...");
    getchar();

    char detalhe[200];
    sprintf(detalhe, "ID=%d;Nome=%s", novo.idBombeiro, novo.nome);
    registarLog("INFO", "BOMBEIROS", "CRIAR", detalhe);

    return novo.idBombeiro;
}


/*  LISTAR BOMBEIROS      */
void listarBombeiros(const SistemaGestaoIncendios *sistema) {

    if (sistema->bombeiros.tamanho == 0) {
        printf("Não existem bombeiros registados!\n");
        return;
    }
    int contadorAtivas = 0;

    for (int i = 0; i < sistema->bombeiros.tamanho; i++) {

        if(sistema->bombeiros.dados[i].ativo == 1){
            contadorAtivas++;

            printf("------------------------------------\n");
            printf("Nrº: %d\n", contadorAtivas);
            printf("\n");
            printf("ID de Bombeiro: %d\n", sistema->bombeiros.dados[i].idBombeiro);
            printf("Nome: %s\n", sistema->bombeiros.dados[i].nome);

            printf("Especialidade: %s\n",
       especialidadeBombeiroParaString(sistema->bombeiros.dados[i].especialidade));

            printf("Estado: %s\n",
                   estadoBombeiroParaString(sistema->bombeiros.dados[i].estado));

            printf("\n");
            printf("\n");
        }
    }
    if (contadorAtivas == 0) {
        printf("Todos os bombeiros estão inativos!");
    }else {
        printf("Total de bombeiros: %d\n", contadorAtivas);
    }

    printf("Prima ENTER para voltar ao menu...");
    getchar();

    //registarLog("INFO", "BOMBEIROS", "LISTAR", "LISTAGEM EXECUTADA");
    // log comentado para evitar logs excessivos
}


/*  ATUALIZAR BOMBEIRO       */
void atualizarBombeiro(SistemaGestaoIncendios *sistema) {
    printf("|ATUALIZAR BOMBEIRO|\n");
    printf("\n");

    /* Verificar se existem bombeiros */
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
            printf("ID %d | %-20s | %-15s | %s\n",
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


    /* Pedir ID */
    int id = getInt(1, 9999, "ID do bombeiro a atualizar");

    /* Procurar bombeiro */
    Bombeiro *b = procurarBombeiroPorId(sistema, id);

    if (b == NULL) {
        printf("\nBombeiro com ID %d não encontrado!\n", id);
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    /* Verificar se está ativo */
    if (b->ativo == 0) {
        printf("\nEste bombeiro está inativo. Não pode ser atualizado.\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    /* Mostrar dados atuais */
    printf("\n");
    printf("==========================================\n");
    printf("         DADOS ATUAIS                    \n");
    printf("===========================================\n");
    printf("| ID: %d\n", b->idBombeiro);
    printf("| Nome: %s\n", b->nome);
    printf("| Especialidade: %s\n", especialidadeBombeiroParaString(b->especialidade));
    printf("| Estado: %s\n", estadoBombeiroParaString(b->estado));
    printf("=============================================\n");
    printf("\n");

    /* Criar cópia temporária */
    Bombeiro temp = *b;
    int alterou = 0;

    /* Pedir novos valores */
    printf("ATUALIZAÇÃO DE CAMPOS:\n");
    printf("(Deixe vazio/escolha Não para manter o atual)\n\n");


    /* 1. Nome */
    char novoNome[MAX_NOME];
    printf("Nome [%s]: ", temp.nome);
    if (fgets(novoNome, MAX_NOME, stdin) != NULL) {

        size_t len = strlen(novoNome);
        if (len > 0 && novoNome[len - 1] == '\n') {
            novoNome[len - 1] = '\0';
        }

        /* Se não está vazio, atualizar */
        if (strlen(novoNome) > 0) {
            strcpy(temp.nome, novoNome);
            alterou = 1;
        }
    }

    /* 2. Especialidade */
    printf("\nAlterar especialidade? (1=Sim, 0=Não): ");
    if (getInt(0, 1, "") == 1) {
        temp.especialidade = escolherEspecialidadeBombeiro();
        alterou = 1;
    }

    /* 3. Estado */
    printf("\nAlterar estado? (1=Sim, 0=Não): ");
    if (getInt(0, 1, "") == 1) {

        /* verificar se está em intervenção ativa */
        int estaEmIntervencaoAtiva = 0;

        for (int i = 0; i < sistema->intervencoes.tamanho; i++) {
            Intervencao *intv = &sistema->intervencoes.dados[i];

            if (intv->ativo == 1 && intv->estado != INT_CONCLUIDA) {
                /* Verificar se este bombeiro está nesta intervenção */
                for (int j = 0; j < intv->numBombeiros; j++) {
                    if (intv->idsBombeiros[j] == b->idBombeiro) {
                        estaEmIntervencaoAtiva = 1;
                        break;
                    }
                }
            }

            if (estaEmIntervencaoAtiva) break;
        }

        if (estaEmIntervencaoAtiva) {
            printf("\n AVISO: Este bombeiro está numa intervenção ativa!\n");
            printf("Não pode alterar o estado para DISPONÍVEL ou EM TREINO.\n");
            printf("Apenas pode manter EM_INTERVENCAO_BOMBEIRO.\n\n");

            /* Só permite escolher EM_INTERVENCAO_BOMBEIRO */
            printf("Estado mantido como: %s\n", estadoBombeiroParaString(b->estado));

        } else {
            /* Pode alterar */
            printf("\nNovo estado:\n");
            temp.estado = escolherEstadoBombeiro();
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
    printf("      RESUMO DAS ALTERAÇÕES             \n");
    printf("==========================================\n");

    if (strcmp(b->nome, temp.nome) != 0) {
        printf("| Nome:\n");
        printf("|   Antes: %s\n", b->nome);
        printf("|   Depois: %s\n", temp.nome);
    }

    if (b->especialidade != temp.especialidade) {
        printf("| Especialidade:\n");
        printf("|   Antes: %s\n", especialidadeBombeiroParaString(b->especialidade));
        printf("|   Depois: %s\n", especialidadeBombeiroParaString(temp.especialidade));
    }

    if (b->estado != temp.estado) {
        printf("| Estado:\n");
        printf("|   Antes: %s\n", estadoBombeiroParaString(b->estado));
        printf("|   Depois: %s\n", estadoBombeiroParaString(temp.estado));
    }

    printf("==========================================\n");
    printf("\n");

    /* Confirmar alterações */
    printf("Confirmar alterações? (1=Sim, 0=Não): ");
    if (getInt(0, 1, "") == 1) {
        *b = temp;

        printf("\n Bombeiro atualizado com sucesso!\n");

        char detalhe[200];
        sprintf(detalhe, "ID=%d atualizado", b->idBombeiro);
        registarLog("INFO", "BOMBEIROS", "ATUALIZAR", detalhe);
    } else {
        printf("\nAlterações canceladas. Nada foi modificado.\n");
    }

    printf("Prima ENTER para voltar ao menu...");
    getchar();
}


/*  INATIVAR BOMBEIRO       */
void inativarBombeiro(SistemaGestaoIncendios *sistema) {

    if (sistema->bombeiros.tamanho == 0){
        printf("Não há bombeiros registados!\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    int id = getInt(1, 9999, "Insira o ID do bombeiro que quer inativar: ");

    Bombeiro *b = procurarBombeiroPorId(sistema, id);

    if (b == NULL) {
        printf("ID não encontrado!");
        return;
    }

    if (b->ativo == 0) {
        printf("Bombeiro já se encontra inativo\n");
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
    printf("Bombeiro %d inativado com sucesso!\n", id);

    printf("Prima ENTER para voltar ao menu...");
    getchar();

    char detalhe[200];
    sprintf(detalhe, "Bombeiro %d inativado", id);
    registarLog("INFO", "BOMBEIROS", "INATIVAR", detalhe);

// tentar eliminar, se não tiver relações diretas
    tentarEliminarBombeiro(sistema, id);

}

/*  PROCURAR BOMBEIRO POR ID       */
Bombeiro* procurarBombeiroPorId(const SistemaGestaoIncendios *sistema, int id) {
    for (int i = 0; i < sistema->bombeiros.tamanho; i++) {
        if (sistema->bombeiros.dados[i].idBombeiro == id) {
            return &sistema->bombeiros.dados[i];
        }
    }
    return NULL;
}


/*  REATIVAR BOMBEIRO        */
void reativarBombeiro(SistemaGestaoIncendios *sistema) {
    printf("|REATIVAR BOMBEIRO|\n");
    printf("\n");

    if (sistema->bombeiros.tamanho == 0) {
        printf("Não há bombeiros registados!\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    /* Mostrar bombeiros inativos */
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
    int id = getInt(1, 9999, "Insira o ID do bombeiro que quer reativar");

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

    /* Reativar: colocar ativo e garantir estado DISPONIVEL */
    b->ativo = 1;
    b->estado = DISPONIVEL;

    printf("\n Bombeiro %d reativado com sucesso!\n", id);
    printf("   Nome: %s\n", b->nome);
    printf("   Estado: %s\n", estadoBombeiroParaString(b->estado));

    printf("\nPrima ENTER para voltar ao menu...");
    getchar();

    char detalhe[200];
    sprintf(detalhe, "Bombeiro %d reativado", id);
    registarLog("INFO", "BOMBEIROS", "REATIVAR", detalhe);
}

void tentarEliminarBombeiro(SistemaGestaoIncendios *sistema, int idBombeiro) {

    Bombeiro *b = procurarBombeiroPorId(sistema, idBombeiro);
    if (!b || b->ativo != 0) return; // só tenta eliminar se estiver inativo

    for (int i = 0; i < sistema->intervencoes.tamanho; i++) {
        Intervencao *intv = &sistema->intervencoes.dados[i];

        for (int j = 0; j < intv->numBombeiros; j++) {
            if (intv->idsBombeiros[j] == idBombeiro) {
                return; // se tem relação NÃO eliminar
            }
        }
    }

    /* Eliminar fisicamente */
    for (int i = 0; i < sistema->bombeiros.tamanho; i++) {
        if (sistema->bombeiros.dados[i].idBombeiro == idBombeiro) {
            sistema->bombeiros.dados[i] =
                sistema->bombeiros.dados[sistema->bombeiros.tamanho - 1];
            sistema->bombeiros.tamanho--;
            return;
        }
    }
}
