#include "intervencoes.h"
#include "ocorrencias.h"
#include "bombeiros.h"
#include "equipamentos.h"
#include "sugestoes.h"
#include "logs.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @file intervencoes.c
 * @brief Implementação das operações sobre Intervenções.
 *
 * Funcionalidades:
 *  - Criar intervenções para ocorrências reportadas
 *  - Adicionar bombeiros e equipamentos
 *  - Iniciar execução da intervenção
 *  - Concluir intervenção com validações de data/hora
 *  - Listar intervenções ativas
 *  - Inativar e reativar intervenções
 *  - Consultar intervenção por ID
 *  - Sugestões automáticas de recursos baseadas em histórico
 */


/* ========================================================================= */
/*  PROCURAR INTERVENÇÃO POR ID                                              */
/* ========================================================================= */

Intervencao *procurarIntervencaoPorId(const SistemaGestaoIncendios *sistema, int id) {
    for (int i = 0; i < sistema->intervencoes.tamanho; i++) {
        if (sistema->intervencoes.dados[i].idIntervencao == id) {
            return &sistema->intervencoes.dados[i];
        }
    }
    return NULL;
}


/* ========================================================================= */
/*  CRIAR INTERVENÇÃO                                                        */
/* ========================================================================= */

int criarIntervencao(SistemaGestaoIncendios *sistema) {
    printf("|CRIAR NOVA INTERVENÇÃO|\n\n");

    if (sistema->ocorrencias.tamanho == 0) {
        printf("Não existem ocorrências registadas.\n");
        printf("Crie uma ocorrência primeiro.\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return -1;
    }

    /* Mostrar ocorrências elegíveis (REPORTADAS e ativas) */
    printf("OCORRÊNCIAS DISPONÍVEIS (REPORTADAS):\n\n");

    int encontrou = 0;
    for (int i = 0; i < sistema->ocorrencias.tamanho; i++) {
        Ocorrencia *o = &sistema->ocorrencias.dados[i];
        if (o->ativo && o->estado == REPORTADA) {
            printf("ID %d | %s | %s | %s\n",
                   o->idOcorrencia,
                   o->localizacao,
                   tipoOcorrenciaParaString(o->tipo),
                   prioridadeParaString(o->prioridade));
            encontrou = 1;
        }
    }

    if (!encontrou) {
        printf("\nNão existem ocorrências elegíveis.\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return -1;
    }

    printf("\n");
    int idOcorrencia = getInt(1, 9999, "ID da ocorrência");

    Ocorrencia *o = procurarOcorrenciaPorId(sistema, idOcorrencia);
    if (o == NULL || !o->ativo || o->estado != REPORTADA) {
        printf("\nOcorrência inválida ou não elegível.\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return -1;
    }

    /* Construir nova intervenção */
    Intervencao nova;
    nova.idIntervencao = sistema->proximoIdIntervencao;
    nova.idOcorrencia  = idOcorrencia;
    nova.inicio        = lerDataHora("Data e hora de início da intervenção");

    /* Validar que o início é posterior ao registo da ocorrência */
    if (!validarDataHoraFimMaiorQueInicio(&o->dataHora, &nova.inicio)) {
        printf("\nErro: A data/hora de início deve ser posterior à data/hora da ocorrência.\n");
        printf("Ocorrência registada em: %02d/%02d/%04d %02d:%02d\n",
               o->dataHora.dia, o->dataHora.mes, o->dataHora.ano,
               o->dataHora.hora, o->dataHora.minuto);
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return -1;
    }

    nova.fim.dia    = 0;
    nova.fim.mes    = 0;
    nova.fim.ano    = 0;
    nova.fim.hora   = 0;
    nova.fim.minuto = 0;
    nova.fimDefinido = 0;

    nova.estado = EM_PLANEAMENTO;
    nova.ativo  = 1;

    /* Alocar arrays dinâmicos */
    nova.idsBombeiros    = malloc(CAPACIDADE_INICIAL * sizeof(int));
    nova.idsEquipamentos = malloc(CAPACIDADE_INICIAL * sizeof(int));

    if (nova.idsBombeiros == NULL || nova.idsEquipamentos == NULL) {
        free(nova.idsBombeiros);
        free(nova.idsEquipamentos);
        printf("\nErro de memória ao criar intervenção.\n");
        return -1;
    }

    nova.numBombeiros          = 0;
    nova.numEquipamentos       = 0;
    nova.capacidadeBombeiros   = CAPACIDADE_INICIAL;
    nova.capacidadeEquipamentos = CAPACIDADE_INICIAL;

    if (!expandirListaIntervencoesSeNecessario(&sistema->intervencoes)) {
        free(nova.idsBombeiros);
        free(nova.idsEquipamentos);
        printf("\nErro ao expandir lista de intervenções.\n");
        return -1;
    }

    sistema->intervencoes.dados[sistema->intervencoes.tamanho] = nova;
    sistema->intervencoes.tamanho++;
    sistema->proximoIdIntervencao++;

    /* Ponteiro para a intervenção recém-criada */
    Intervencao *intv = &sistema->intervencoes.dados[sistema->intervencoes.tamanho - 1];

    /* Adicionar recursos durante a criação */
    adicionarBombeirosIntervencao(sistema, intv);
    adicionarEquipamentosIntervencao(sistema, intv);

    if (intv->numBombeiros == 0) {
        printf("\nAviso: A intervenção foi criada sem bombeiros associados.\n");
    }
    if (intv->numEquipamentos == 0) {
        printf("Aviso: A intervenção foi criada sem equipamentos associados.\n");
    }

    printf("\nIntervenção criada com sucesso.\n");
    printf("ID: %d | Bombeiros: %d | Equipamentos: %d\n",
           intv->idIntervencao, intv->numBombeiros, intv->numEquipamentos);
    printf("Estado: %s\n", estadoIntervencaoParaString(intv->estado));

    char detalhe[200];
    snprintf(detalhe, sizeof(detalhe), "ID=%d;Ocorrencia=%d",
             intv->idIntervencao, idOcorrencia);
    registarLog("INFO", "INTERVENCOES", "CRIAR", detalhe);

    printf("\nPrima ENTER para voltar ao menu...");
    getchar();

    return intv->idIntervencao;
}


/* ========================================================================= */
/*  ADICIONAR BOMBEIROS À INTERVENÇÃO                                       */
/* ========================================================================= */

void adicionarBombeirosIntervencao(SistemaGestaoIncendios *sistema,
                                    Intervencao *intvExterna) {

    Intervencao *intv  = NULL;
    int pedirId = (intvExterna == NULL);

    printf("|ADICIONAR BOMBEIROS À INTERVENÇÃO|\n\n");

    if (pedirId) {
        if (sistema->intervencoes.tamanho == 0) {
            printf("Não existem intervenções registadas.\n");
            printf("Prima ENTER para voltar ao menu...");
            getchar();
            return;
        }

        printf("INTERVENÇÕES EM PLANEAMENTO:\n");
        int encontrou = 0;
        for (int i = 0; i < sistema->intervencoes.tamanho; i++) {
            Intervencao *it = &sistema->intervencoes.dados[i];
            if (it->ativo && it->estado == EM_PLANEAMENTO) {
                printf("ID %d | Ocorrência %d | Bombeiros: %d | Equipamentos: %d\n",
                       it->idIntervencao, it->idOcorrencia,
                       it->numBombeiros, it->numEquipamentos);
                encontrou = 1;
            }
        }

        if (!encontrou) {
            printf("Não existem intervenções em planeamento.\n");
            printf("Prima ENTER para voltar ao menu...");
            getchar();
            return;
        }

        int idIntervencao = getInt(1, 9999, "ID da intervenção");
        intv = procurarIntervencaoPorId(sistema, idIntervencao);

        if (intv == NULL || intv->ativo == 0 || intv->estado != EM_PLANEAMENTO) {
            printf("\nIntervenção inválida ou inativa.\n");
            printf("Prima ENTER para voltar ao menu...");
            getchar();
            return;
        }
    } else {
        intv = intvExterna;
    }

    if (intv->estado != EM_PLANEAMENTO) {
        printf("\nSó é possível adicionar bombeiros em intervenções EM_PLANEAMENTO.\n");
        printf("Estado atual: %s\n\n", estadoIntervencaoParaString(intv->estado));
        if (pedirId) {
            printf("Prima ENTER para voltar ao menu...");
            getchar();
        }
        return;
    }

    /* Sugestão automática baseada em histórico */
    Ocorrencia *oc = procurarOcorrenciaPorId(sistema, intv->idOcorrencia);
    if (oc != NULL) {
        int sugestao = calcularSugestaoBombeiros(sistema, oc->tipo, oc->prioridade);
        if (sugestao > 0) {
            if (getInt(0, 1,
                "Deseja consultar a sugestão automática baseada em intervenções anteriores? (1=Sim, 0=Não)")) {
                printf("\nSugestão de apoio ao planeamento:\n");
                printf("  Bombeiros recomendados: %d\n\n", sugestao);
            }
        } else {
            printf("\n(Sem dados históricos suficientes para gerar sugestão de bombeiros.)\n\n");
        }
    }

    /* Listar bombeiros disponíveis */
    printf("BOMBEIROS DISPONÍVEIS:\n");
    int encontrou = 0;
    for (int i = 0; i < sistema->bombeiros.tamanho; i++) {
        Bombeiro *b = &sistema->bombeiros.dados[i];
        if (b->ativo && b->estado == DISPONIVEL) {
            printf("ID %d | %s | %s\n",
                   b->idBombeiro, b->nome,
                   especialidadeBombeiroParaString(b->especialidade));
            encontrou = 1;
        }
    }

    if (!encontrou) {
        printf("\nNão existem bombeiros disponíveis.\n\n");
        if (pedirId) {
            printf("Prima ENTER para voltar ao menu...");
            getchar();
        }
        return;
    }

    int continuar = 1;
    while (continuar) {

        /* Verificar se ainda existem bombeiros disponíveis */
        int existeDisponivel = 0;
        for (int i = 0; i < sistema->bombeiros.tamanho; i++) {
            Bombeiro *bx = &sistema->bombeiros.dados[i];
            if (bx->ativo && bx->estado == DISPONIVEL) {
                existeDisponivel = 1;
                break;
            }
        }

        if (!existeDisponivel) {
            printf("\nNão existem mais bombeiros disponíveis.\n\n");
            break;
        }

        int idBombeiro = getInt(1, 9999, "ID do bombeiro a adicionar");
        Bombeiro *b = procurarBombeiroPorId(sistema, idBombeiro);

        if (b == NULL || !b->ativo || b->estado != DISPONIVEL) {
            printf("\nBombeiro inválido ou indisponível.\n\n");
            continue;
        }

        int jaExiste = 0;
        for (int i = 0; i < intv->numBombeiros; i++) {
            if (intv->idsBombeiros[i] == idBombeiro) {
                jaExiste = 1;
                break;
            }
        }

        if (jaExiste) {
            printf("\nEste bombeiro já está associado.\n\n");
            continue;
        }

        if (!expandirArrayBombeirosIntervencao(intv)) {
            printf("\nErro ao expandir lista de bombeiros.\n\n");
            registarLog("ERROR", "INTERVENCOES", "ADD_BOMBEIRO", "Falha ao expandir array");
            break;
        }

        intv->idsBombeiros[intv->numBombeiros] = idBombeiro;
        intv->numBombeiros++;

        printf("\nBombeiro '%s' adicionado com sucesso.\n", b->nome);
        printf("Total de bombeiros: %d\n\n", intv->numBombeiros);

        char detalhe[200];
        snprintf(detalhe, sizeof(detalhe), "Intervencao=%d;Bombeiro=%d",
                 intv->idIntervencao, idBombeiro);
        registarLog("INFO", "INTERVENCOES", "ADD_BOMBEIRO", detalhe);

        continuar = getInt(0, 1, "Adicionar outro bombeiro? (1=Sim, 0=Não)");
    }

    if (pedirId) {
        printf("\nPrima ENTER para voltar ao menu...");
        getchar();
    }
}

/* ========================================================================= */
/*  ADICIONAR EQUIPAMENTOS À INTERVENÇÃO                                    */
/* ========================================================================= */

void adicionarEquipamentosIntervencao(SistemaGestaoIncendios *sistema,
                                       Intervencao *intvExterna) {

    Intervencao *intv = NULL;
    int pedirId = (intvExterna == NULL);

    printf("|ADICIONAR EQUIPAMENTOS À INTERVENÇÃO|\n\n");

    if (pedirId) {
        if (sistema->intervencoes.tamanho == 0) {
            printf("Não existem intervenções registadas.\n");
            printf("Prima ENTER para voltar ao menu...");
            getchar();
            return;
        }

        printf("INTERVENÇÕES EM PLANEAMENTO:\n");
        int encontrou = 0;
        for (int i = 0; i < sistema->intervencoes.tamanho; i++) {
            Intervencao *it = &sistema->intervencoes.dados[i];
            if (it->ativo && it->estado == EM_PLANEAMENTO) {
                printf("ID %d | Ocorrência %d | Bombeiros: %d | Equipamentos: %d\n",
                       it->idIntervencao, it->idOcorrencia,
                       it->numBombeiros, it->numEquipamentos);
                encontrou = 1;
            }
        }

        if (!encontrou) {
            printf("Não existem intervenções em planeamento.\n");
            printf("Prima ENTER para voltar ao menu...");
            getchar();
            return;
        }

        int idIntervencao = getInt(1, 9999, "ID da intervenção");
        intv = procurarIntervencaoPorId(sistema, idIntervencao);

        /* Corrigido: verificar também o estado, consistente com adicionarBombeirosIntervencao */
        if (intv == NULL || intv->ativo == 0 || intv->estado != EM_PLANEAMENTO) {
            printf("\nIntervenção inválida ou inativa.\n");
            printf("Prima ENTER para voltar ao menu...");
            getchar();
            return;
        }
    } else {
        intv = intvExterna;
    }

    if (intv->estado != EM_PLANEAMENTO) {
        printf("\nSó é possível adicionar equipamentos em intervenções EM_PLANEAMENTO.\n");
        printf("Estado atual: %s\n\n", estadoIntervencaoParaString(intv->estado));
        if (pedirId) {
            printf("Prima ENTER para voltar ao menu...");
            getchar();
        }
        return;
    }

    /* Sugestão automática de equipamentos */
    Ocorrencia *oc = procurarOcorrenciaPorId(sistema, intv->idOcorrencia);
    if (oc != NULL) {
        int v = 0, m = 0, r = 0;
        calcularSugestaoEquipamentos(sistema, oc->tipo, oc->prioridade, &v, &m, &r);
        if (v >= 0) {
            if (getInt(0, 1,
                "Deseja consultar a sugestão automática de equipamentos? (1=Sim, 0=Não)")) {
                printf("\nSugestão de apoio ao planeamento:\n");
                printf("  Veículos:     %d\n", v);
                printf("  Mangueiras:   %d\n", m);
                printf("  Respiradores: %d\n\n", r);
            }
        } else {
            printf("\n(Sem dados históricos suficientes para gerar sugestão de equipamentos.)\n\n");
        }
    }

    /* Listar equipamentos disponíveis */
    printf("EQUIPAMENTOS DISPONÍVEIS:\n");
    int encontrou = 0;
    for (int i = 0; i < sistema->equipamentos.tamanho; i++) {
        Equipamento *e = &sistema->equipamentos.dados[i];
        if (e->ativo && e->estado == EQ_DISPONIVEL) {
            printf("ID %d | %s | %s | %s\n",
                   e->idEquipamento, e->designacao,
                   tipoEquipamentoParaString(e->tipo), e->localizacao);
            encontrou = 1;
        }
    }

    if (!encontrou) {
        printf("\nNão existem equipamentos disponíveis.\n\n");
        if (pedirId) {
            printf("Prima ENTER para voltar ao menu...");
            getchar();
        }
        return;
    }

    int continuar = 1;
    while (continuar) {

        /* verificar se ainda existem equipamentos disponíveis */
        int existeDisponivel = 0;
        for (int i = 0; i < sistema->equipamentos.tamanho; i++) {
            Equipamento *ex = &sistema->equipamentos.dados[i];
            if (ex->ativo && ex->estado == EQ_DISPONIVEL) {
                existeDisponivel = 1;
                break;
            }
        }

        if (!existeDisponivel) {
            printf("\nNão existem mais equipamentos disponíveis.\n\n");
            break;
        }

        int idEq = getInt(1, 9999, "ID do equipamento a adicionar");
        Equipamento *eq = procurarEquipamentoPorId(sistema, idEq);

        if (eq == NULL || !eq->ativo || eq->estado != EQ_DISPONIVEL) {
            printf("\nEquipamento inválido ou indisponível.\n\n");
            continue;
        }

        int jaExiste = 0;
        for (int i = 0; i < intv->numEquipamentos; i++) {
            if (intv->idsEquipamentos[i] == idEq) {
                jaExiste = 1;
                break;
            }
        }

        if (jaExiste) {
            printf("\nEste equipamento já está associado.\n\n");
            continue;
        }

        if (!expandirArrayEquipamentosIntervencao(intv)) {
            printf("\nErro ao expandir lista de equipamentos.\n\n");
            registarLog("ERROR", "INTERVENCOES", "ADD_EQUIPAMENTO", "Falha ao expandir array");
            break;
        }

        intv->idsEquipamentos[intv->numEquipamentos] = idEq;
        intv->numEquipamentos++;

        printf("\nEquipamento '%s' adicionado com sucesso.\n", eq->designacao);
        printf("Total de equipamentos: %d\n\n", intv->numEquipamentos);

        char detalhe[200];
        snprintf(detalhe, sizeof(detalhe), "Intervencao=%d;Equipamento=%d",
                 intv->idIntervencao, idEq);
        registarLog("INFO", "INTERVENCOES", "ADD_EQUIPAMENTO", detalhe);

        continuar = getInt(0, 1, "Adicionar outro equipamento? (1=Sim, 0=Não)");
    }

    if (pedirId) {
        printf("\nPrima ENTER para voltar ao menu...");
        getchar();
    }
}


/* ========================================================================= */
/*  LISTAR INTERVENÇÕES                                                      */
/* ========================================================================= */

void listarIntervencoes(const SistemaGestaoIncendios *sistema) {
    printf("|LISTA DE INTERVENÇÕES|\n\n");

    if (sistema->intervencoes.tamanho == 0) {
        printf("Não existem intervenções registadas.\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    int contadorAtivos = 0;

    for (int i = 0; i < sistema->intervencoes.tamanho; i++) {
        Intervencao *intv = &sistema->intervencoes.dados[i];
        if (intv->ativo != 1) continue;

        contadorAtivos++;
        printf("----------------------------------------\n");
        printf("Nrº: %d\n\n", contadorAtivos);
        printf("ID: %d\n",    intv->idIntervencao);
        printf("Estado: %s\n", estadoIntervencaoParaString(intv->estado));

        Ocorrencia *o = procurarOcorrenciaPorId(sistema, intv->idOcorrencia);
        if (o != NULL) {
            printf("Ocorrência associada: ID %d - %s (%s)\n",
                   o->idOcorrencia, o->localizacao,
                   tipoOcorrenciaParaString(o->tipo));
        } else {
            printf("Ocorrência associada: não encontrada\n");
        }

        printf("Início: %02d/%02d/%04d %02d:%02d\n",
               intv->inicio.dia, intv->inicio.mes, intv->inicio.ano,
               intv->inicio.hora, intv->inicio.minuto);

        if (intv->fimDefinido) {
            printf("Fim:    %02d/%02d/%04d %02d:%02d\n",
                   intv->fim.dia, intv->fim.mes, intv->fim.ano,
                   intv->fim.hora, intv->fim.minuto);
        } else {
            printf("Fim:    (ainda não concluída)\n");
        }

        printf("\nBombeiros atribuídos (%d):\n", intv->numBombeiros);
        if (intv->numBombeiros == 0) {
            printf("  (nenhum bombeiro atribuído)\n");
        } else {
            for (int j = 0; j < intv->numBombeiros; j++) {
                Bombeiro *b = procurarBombeiroPorId(sistema, intv->idsBombeiros[j]);
                if (b != NULL) {
                    printf("  - %s (%s)\n", b->nome,
                           especialidadeBombeiroParaString(b->especialidade));
                }
            }
        }

        printf("\nEquipamentos atribuídos (%d):\n", intv->numEquipamentos);
        if (intv->numEquipamentos == 0) {
            printf("  (nenhum equipamento atribuído)\n");
        } else {
            for (int j = 0; j < intv->numEquipamentos; j++) {
                Equipamento *eq = procurarEquipamentoPorId(sistema, intv->idsEquipamentos[j]);
                if (eq != NULL) {
                    printf("  - %s (%s)\n", eq->designacao,
                           tipoEquipamentoParaString(eq->tipo));
                }
            }
        }

        printf("\n");
    }

    if (contadorAtivos == 0) {
        printf("Todas as intervenções existentes estão inativas.\n");
    } else {
        printf("Total de intervenções ativas: %d\n", contadorAtivos);
    }

    /* Log comentado para evitar logs excessivos em operações de leitura */
    /* registarLog("INFO", "INTERVENCOES", "LISTAR", "Listagem executada"); */

    printf("\nPrima ENTER para voltar ao menu...");
    getchar();
}


/* ========================================================================= */
/*  CONCLUIR INTERVENÇÃO                                                     */
/* ========================================================================= */

void concluirIntervencao(SistemaGestaoIncendios *sistema) {
    printf("CONCLUIR INTERVENÇÃO:\n\n");

    if (sistema->intervencoes.tamanho == 0) {
        printf("Não existem intervenções registadas.\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    printf("INTERVENÇÕES EM EXECUÇÃO:\n\n");
    int encontrouAtivas = 0;
    for (int i = 0; i < sistema->intervencoes.tamanho; i++) {
        Intervencao *intv = &sistema->intervencoes.dados[i];
        if (intv->ativo == 1 && intv->estado == EM_EXECUCAO) {
            printf("ID %d | Estado: %s | Ocorrência ID: %d\n",
                   intv->idIntervencao,
                   estadoIntervencaoParaString(intv->estado),
                   intv->idOcorrencia);
            encontrouAtivas = 1;
        }
    }

    if (!encontrouAtivas) {
        printf("Não há intervenções em execução para concluir.\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    printf("\n");
    int idIntervencao = getInt(1, 9999, "ID da intervenção a concluir");

    Intervencao *intv = procurarIntervencaoPorId(sistema, idIntervencao);

    if (intv == NULL) {
        printf("Intervenção não encontrada.\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    if (intv->ativo == 0) {
        printf("Esta intervenção está inativa.\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    if (intv->estado == INT_CONCLUIDA) {
        printf("Esta intervenção já se encontra concluída.\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    if (intv->estado != EM_EXECUCAO) {
        printf("\nErro: Apenas intervenções EM EXECUÇÃO podem ser concluídas.\n");
        printf("Estado atual: %s\n", estadoIntervencaoParaString(intv->estado));
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    printf("\nREGISTAR FIM DA INTERVENÇÃO:\n");
    DataHora dataFim = lerDataHora("Data e hora de fim da intervenção");

    if (!validarDataHoraFimMaiorQueInicio(&intv->inicio, &dataFim)) {
        printf("\nErro: A data/hora de fim deve ser posterior à de início.\n");
        printf("Início registado: %02d/%02d/%04d %02d:%02d\n",
               intv->inicio.dia, intv->inicio.mes, intv->inicio.ano,
               intv->inicio.hora, intv->inicio.minuto);
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    intv->fim        = dataFim;
    intv->fimDefinido = 1;
    intv->estado     = INT_CONCLUIDA;

    /* Libertar bombeiros */
    for (int i = 0; i < intv->numBombeiros; i++) {
        Bombeiro *b = procurarBombeiroPorId(sistema, intv->idsBombeiros[i]);
        if (b != NULL && b->ativo == 1) b->estado = DISPONIVEL;
    }

    /* Libertar equipamentos */
    for (int i = 0; i < intv->numEquipamentos; i++) {
        Equipamento *eq = procurarEquipamentoPorId(sistema, intv->idsEquipamentos[i]);
        if (eq != NULL && eq->ativo == 1) eq->estado = EQ_DISPONIVEL;
    }

    /* Atualizar ocorrência para CONCLUIDA */
    Ocorrencia *o = procurarOcorrenciaPorId(sistema, intv->idOcorrencia);
    if (o != NULL && o->ativo == 1) o->estado = CONCLUIDA;

    printf("\nIntervenção concluída com sucesso.\n");
    printf("ID: %d\n", intv->idIntervencao);
    printf("Fim registado: %02d/%02d/%04d %02d:%02d\n",
           dataFim.dia, dataFim.mes, dataFim.ano,
           dataFim.hora, dataFim.minuto);
    printf("Bombeiros libertados: %d\n",    intv->numBombeiros);
    printf("Equipamentos libertados: %d\n", intv->numEquipamentos);
    printf("Ocorrência associada atualizada para CONCLUÍDA.\n");

    char detalhe[200];
    snprintf(detalhe, sizeof(detalhe), "Intervencao %d concluída", idIntervencao);
    registarLog("INFO", "INTERVENCOES", "CONCLUIR", detalhe);

    printf("\nPrima ENTER para voltar ao menu...");
    getchar();
}


/* ========================================================================= */
/*  INATIVAR INTERVENÇÃO                                                     */
/* ========================================================================= */

void inativarIntervencao(SistemaGestaoIncendios *sistema) {
    printf("|INATIVAR INTERVENÇÃO|\n\n");

    if (sistema->intervencoes.tamanho == 0) {
        printf("Não há intervenções registadas.\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    printf("INTERVENÇÕES ATIVAS (NÃO EM EXECUÇÃO):\n");
    int encontrou = 0;
    for (int i = 0; i < sistema->intervencoes.tamanho; i++) {
        Intervencao *it = &sistema->intervencoes.dados[i];
        if (it->ativo && it->estado != EM_EXECUCAO) {
            printf("ID %d | Estado: %s | Ocorrência %d\n",
                   it->idIntervencao,
                   estadoIntervencaoParaString(it->estado),
                   it->idOcorrencia);
            encontrou = 1;
        }
    }

    if (!encontrou) {
        printf("Não existem intervenções elegíveis para inativação.\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    int id = getInt(1, 9999, "ID da intervenção que pretende inativar");

    Intervencao *intv = procurarIntervencaoPorId(sistema, id);

    if (intv == NULL) {
        printf("ID não encontrado.\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    if (intv->ativo == 0) {
        printf("A intervenção já se encontra inativa.\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    if (intv->estado == EM_EXECUCAO) {
        printf("\nA intervenção não pode ser inativada enquanto estiver em execução.\n");
        printf("Conclua a intervenção antes de a inativar.\n");
        registarLog("WARNING", "INTERVENCOES", "INATIVAR",
                    "Tentativa de inativar intervenção em execução bloqueada");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    intv->ativo = 0;

    printf("\nIntervenção %d inativada com sucesso.\n", id);
    printf("  Bombeiros associados: %d\n",    intv->numBombeiros);
    printf("  Equipamentos associados: %d\n", intv->numEquipamentos);

    char detalhe[200];
    snprintf(detalhe, sizeof(detalhe), "Intervencao %d inativada (estado=%s)",
             id, estadoIntervencaoParaString(intv->estado));
    registarLog("INFO", "INTERVENCOES", "INATIVAR", detalhe);

    printf("\nPrima ENTER para voltar ao menu...");
    getchar();

    tentarEliminarIntervencao(sistema, id);
}


/* ========================================================================= */
/*  REATIVAR INTERVENÇÃO                                                     */
/* ========================================================================= */

void reativarIntervencao(SistemaGestaoIncendios *sistema) {
    printf("|REATIVAR INTERVENÇÃO|\n\n");

    if (sistema->intervencoes.tamanho == 0) {
        printf("Não há intervenções registadas.\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    printf("INTERVENÇÕES INATIVAS:\n");
    int encontrou = 0;
    for (int i = 0; i < sistema->intervencoes.tamanho; i++) {
        Intervencao *intv = &sistema->intervencoes.dados[i];
        if (intv->ativo == 0) {
            printf("ID %d | Estado: %s | Ocorrência: %d | Bombeiros: %d | Equipamentos: %d\n",
                   intv->idIntervencao,
                   estadoIntervencaoParaString(intv->estado),
                   intv->idOcorrencia,
                   intv->numBombeiros,
                   intv->numEquipamentos);
            encontrou = 1;
        }
    }

    if (!encontrou) {
        printf("Não existem intervenções inativas.\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    int id = getInt(1, 9999, "ID da intervenção que pretende reativar");

    Intervencao *intv = procurarIntervencaoPorId(sistema, id);

    if (intv == NULL) {
        printf("ID não encontrado.\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    if (intv->ativo == 1) {
        printf("A intervenção já se encontra ativa.\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    intv->ativo = 1;

    printf("\nIntervenção reativada com sucesso.\n");
    printf("ID: %d\n",    intv->idIntervencao);
    printf("Estado: %s\n", estadoIntervencaoParaString(intv->estado));
    printf("Ocorrência associada: %d\n", intv->idOcorrencia);
    printf("\nDados preservados:\n");
    printf("  Bombeiros associados: %d\n",    intv->numBombeiros);
    printf("  Equipamentos associados: %d\n", intv->numEquipamentos);

    char detalhe[200];
    snprintf(detalhe, sizeof(detalhe),
             "Intervencao %d reativada (bombeiros=%d, equipamentos=%d)",
             id, intv->numBombeiros, intv->numEquipamentos);
    registarLog("INFO", "INTERVENCOES", "REATIVAR", detalhe);

    printf("\nPrima ENTER para voltar ao menu...");
    getchar();
}


/* ========================================================================= */
/*  INICIAR EXECUÇÃO DA INTERVENÇÃO                                         */
/* ========================================================================= */

void iniciarExecucaoIntervencao(SistemaGestaoIncendios *sistema) {
    printf("INICIAR EXECUÇÃO DA INTERVENÇÃO\n\n");

    if (sistema->intervencoes.tamanho == 0) {
        printf("Não existem intervenções registadas.\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    printf("INTERVENÇÕES EM PLANEAMENTO:\n");
    int encontrou = 0;
    for (int i = 0; i < sistema->intervencoes.tamanho; i++) {
        Intervencao *intv = &sistema->intervencoes.dados[i];
        if (intv->ativo == 1 && intv->estado == EM_PLANEAMENTO) {
            printf("ID %d | Ocorrência: %d | Bombeiros: %d | Equipamentos: %d\n",
                   intv->idIntervencao, intv->idOcorrencia,
                   intv->numBombeiros, intv->numEquipamentos);
            encontrou = 1;
        }
    }

    if (!encontrou) {
        printf("Não existem intervenções em estado EM_PLANEAMENTO.\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    printf("\n");
    int id = getInt(1, 9999, "ID da intervenção a iniciar");

    Intervencao *intv = procurarIntervencaoPorId(sistema, id);

    if (intv == NULL) {
        printf("\nIntervenção não encontrada.\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    if (intv->ativo == 0) {
        printf("\nA intervenção encontra-se inativa. Reative antes de iniciar a execução.\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    if (intv->estado != EM_PLANEAMENTO) {
        printf("\nApenas intervenções EM_PLANEAMENTO podem ser colocadas em execução.\n");
        printf("Estado atual: %s\n", estadoIntervencaoParaString(intv->estado));
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    if (intv->numBombeiros == 0) {
        printf("\nA intervenção não possui bombeiros associados.\n");
        printf("Adicione bombeiros antes de iniciar a execução.\n");
        printf("Prima ENTER para voltar ao menu...");
        getchar();
        return;
    }

    /* Verificar disponibilidade de todos os bombeiros */
    for (int i = 0; i < intv->numBombeiros; i++) {
        Bombeiro *b = procurarBombeiroPorId(sistema, intv->idsBombeiros[i]);
        if (b == NULL || b->ativo == 0) {
            printf("\nErro: Bombeiro associado não existe ou está inativo.\n");
            printf("Execução cancelada.\n");
            printf("Prima ENTER para voltar ao menu...");
            getchar();
            return;
        }
        if (b->estado != DISPONIVEL) {
            printf("\nErro: O bombeiro '%s' já não está disponível (estado: %s).\n",
                   b->nome, estadoBombeiroParaString(b->estado));
            printf("Execução cancelada.\n");
            printf("Prima ENTER para voltar ao menu...");
            getchar();
            return;
        }
    }

    /* Verificar disponibilidade de todos os equipamentos */
    for (int i = 0; i < intv->numEquipamentos; i++) {
        Equipamento *eq = procurarEquipamentoPorId(sistema, intv->idsEquipamentos[i]);
        if (eq == NULL || eq->ativo == 0) {
            printf("\nErro: Equipamento associado não existe ou está inativo.\n");
            printf("Execução cancelada.\n");
            printf("Prima ENTER para voltar ao menu...");
            getchar();
            return;
        }
        if (eq->estado != EQ_DISPONIVEL) {
            printf("\nErro: O equipamento '%s' já não está disponível (estado: %s).\n",
                   eq->designacao, estadoEquipamentoParaString(eq->estado));
            printf("Execução cancelada.\n");
            printf("Prima ENTER para voltar ao menu...");
            getchar();
            return;
        }
    }

    /* Transição de estado */
    intv->estado = EM_EXECUCAO;

    Ocorrencia *o = procurarOcorrenciaPorId(sistema, intv->idOcorrencia);
    if (o != NULL && o->ativo == 1) o->estado = EM_INTERVENCAO;

    int bombeirosAtivados = 0;
    for (int i = 0; i < intv->numBombeiros; i++) {
        Bombeiro *b = procurarBombeiroPorId(sistema, intv->idsBombeiros[i]);
        if (b != NULL && b->ativo == 1) {
            b->estado = EM_INTERVENCAO_BOMBEIRO;
            bombeirosAtivados++;
        }
    }

    int equipamentosAtivados = 0;
    for (int i = 0; i < intv->numEquipamentos; i++) {
        Equipamento *eq = procurarEquipamentoPorId(sistema, intv->idsEquipamentos[i]);
        if (eq != NULL && eq->ativo == 1) {
            eq->estado = EQ_EM_USO;
            equipamentosAtivados++;
        }
    }

    printf("\nIntervenção iniciada com sucesso.\n");
    printf("ID: %d\n",    intv->idIntervencao);
    printf("Estado: %s\n", estadoIntervencaoParaString(intv->estado));
    printf("\nRecursos mobilizados:\n");
    printf("  Bombeiros em intervenção: %d\n", bombeirosAtivados);
    printf("  Equipamentos em uso: %d\n",       equipamentosAtivados);

    char detalhe[200];
    snprintf(detalhe, sizeof(detalhe),
             "Intervencao %d iniciada (bombeiros=%d, equipamentos=%d)",
             id, bombeirosAtivados, equipamentosAtivados);
    registarLog("INFO", "INTERVENCOES", "INICIAR_EXECUCAO", detalhe);

    printf("\nPrima ENTER para voltar ao menu...");
    getchar();
}


/* ========================================================================= */
/*  TENTAR ELIMINAR INTERVENÇÃO                                              */
/* ========================================================================= */

void tentarEliminarIntervencao(SistemaGestaoIncendios *sistema, int idIntervencao) {

    Intervencao *intv = procurarIntervencaoPorId(sistema, idIntervencao);

    /* Só elimina se estiver inativa e concluída */
    if (intv == NULL || intv->ativo != 0)       return;
    if (intv->estado != INT_CONCLUIDA)           return;

    for (int i = 0; i < sistema->intervencoes.tamanho; i++) {
        if (sistema->intervencoes.dados[i].idIntervencao == idIntervencao) {

            /* Libertar arrays internos antes de eliminar */
            free(sistema->intervencoes.dados[i].idsBombeiros);
            free(sistema->intervencoes.dados[i].idsEquipamentos);

            /* Swap and pop: se for o único elemento (tamanho == 1), a cópia
             * é do próprio elemento por cima de si mesmo — inócua, porque
             * o tamanho-- torna-o imediatamente inacessível. */
            sistema->intervencoes.dados[i] =
                sistema->intervencoes.dados[sistema->intervencoes.tamanho - 1];
            sistema->intervencoes.tamanho--;
            return;
        }
    }
}