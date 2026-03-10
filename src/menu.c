#include <stdio.h>
#include "menu.h"
#include "utils.h"
#include "logs.h"


#include "ocorrencias.h"
#include "intervencoes.h"
#include "bombeiros.h"
#include "equipamentos.h"
#include "relatorios.h"
#include "sugestoes.h"


/**
 * @file menu.c
 * @brief Implementação dos menus principais e navegação da aplicação.
 *
 * Funcionalidades principais:
 *  - Menu principal do sistema
 *  - Menus dedicados a:
 *      * Ocorrências
 *      * Bombeiros
 *      * Equipamentos
 *      * Intervenções
 *      * Relatórios
 */


/* MENU PRINCIPAL */
void mostrarMenuPrincipal(SistemaGestaoIncendios *sistema) {
    int opcao;

    do {
        printf("========================================\n");
        printf("  SISTEMA DE GESTÃO DE INCÊNDIOS (SGI)\n");
        printf("========================================\n");
        printf("1. Gestão de Ocorrências\n");
        printf("2. Gestão de Intervenções\n");
        printf("3. Gestão de Bombeiros\n");
        printf("4. Gestão de Equipamentos\n");
        printf("5. Relatórios\n");
        printf("0. Guardar e Sair\n\n");

        opcao = getInt(0, 5, "Escolha uma opção");

        switch (opcao) {
            case 1: menuOcorrencias(sistema); break;
            case 2: menuIntervencoes(sistema); break;
            case 3: menuBombeiros(sistema); break;
            case 4: menuEquipamentos(sistema); break;
            case 5: menuRelatorios(sistema); break;

            case 0:
                printf("\nA guardar dados e sair...\n");
                registarLog("INFO", "MENU", "SAIR", "Utilizador saiu do programa");
                break;
            default:
                printf("Opção inválida.\n");
        }

    } while (opcao != 0);
}

/* MENU OCORRÊNCIAS */
void menuOcorrencias(SistemaGestaoIncendios *sistema) {
    int opcao;

    do {
        printf("\n=== GESTÃO DE OCORRÊNCIAS ===\n");
        printf("1. Criar Ocorrência\n");
        printf("2. Listar Ocorrências\n");
        printf("3. Atualizar Ocorrência\n");
        printf("4. Inativar Ocorrência\n");
        printf("5. Reativar Ocorrência\n");
        printf("0. Voltar\n\n");

        opcao = getInt(0, 5, "Escolha uma opção");

        switch (opcao) {
            case 1: criarOcorrencia(sistema); break;
            case 2: listarOcorrencias(sistema); break;
            case 3: atualizarOcorrencia(sistema); break;
            case 4: inativarOcorrencia(sistema); break;
            case 5: reativarOcorrencia(sistema); break;
            case 0: break;
            default: printf("Opção inválida.\n");
        }

    } while (opcao != 0);
}

/* MENU BOMBEIROS */
void menuBombeiros(SistemaGestaoIncendios *sistema) {
    int opcao;

    do {
        printf("\n=== GESTÃO DE BOMBEIROS ===\n");
        printf("1. Criar Bombeiro\n");
        printf("2. Listar Bombeiros\n");
        printf("3. Atualizar Bombeiro\n");
        printf("4. Inativar Bombeiro\n");
        printf("5. Reativar Bombeiro\n");
        printf("0. Voltar\n\n");

        opcao = getInt(0, 5, "Escolha uma opção");

        switch (opcao) {
            case 1: criarBombeiro(sistema); break;
            case 2: listarBombeiros(sistema); break;
            case 3: atualizarBombeiro(sistema); break;
            case 4: inativarBombeiro(sistema); break;
            case 5: reativarBombeiro(sistema); break;
            case 0: break;
            default: printf("Opção inválida.\n");
        }

    } while (opcao != 0);
}

/* MENU EQUIPAMENTOS */
void menuEquipamentos(SistemaGestaoIncendios *sistema) {
    int opcao;

    do {
        printf("\n=== GESTÃO DE EQUIPAMENTOS ===\n");
        printf("1. Criar Equipamento\n");
        printf("2. Listar Equipamentos\n");
        printf("3. Atualizar Equipamento\n");
        printf("4. Inativar Equipamento\n");
        printf("5. Reativar Equipamento\n");
        printf("0. Voltar\n\n");

        opcao = getInt(0, 5, "Escolha uma opção");

        switch (opcao) {
            case 1: criarEquipamento(sistema); break;
            case 2: listarEquipamentos(sistema); break;
            case 3: atualizarEquipamento(sistema); break;
            case 4: inativarEquipamento(sistema); break;
            case 5: reativarEquipamento(sistema); break;
            case 0: break;
            default: printf("Opção inválida.\n");
        }

    } while (opcao != 0);
}

/* MENU INTERVENÇÕES*/
void menuIntervencoes(SistemaGestaoIncendios *sistema) {
    int opcao;

    do {
        printf("\n=== GESTÃO DE INTERVENÇÕES ===\n");
        printf("1. Criar Intervenção\n");
        printf("2. Listar Intervenções\n");
        printf("3. Adicionar Bombeiros\n");
        printf("4. Adicionar Equipamentos\n");
        printf("5. Iniciar Execução\n");
        printf("6. Concluir Intervenção\n");
        printf("7. Inativar Intervenção\n");
        printf("8. Reativar Intervenção\n");
        printf("0. Voltar\n\n");

        opcao = getInt(0, 8, "Escolha uma opção");

        switch (opcao) {
            case 1: criarIntervencao(sistema); break;
            case 2: listarIntervencoes(sistema); break;
            case 3: adicionarBombeirosIntervencao(sistema, NULL); break;
            case 4: adicionarEquipamentosIntervencao(sistema, NULL); break;
            case 5: iniciarExecucaoIntervencao(sistema); break;
            case 6: concluirIntervencao(sistema); break;
            case 7: inativarIntervencao(sistema); break;
            case 8: reativarIntervencao(sistema); break;
            case 0: break;
            default: printf("Opção inválida.\n");
        }

    } while (opcao != 0);
}

/* MENU RELATÓRIOS */
void menuRelatorios(SistemaGestaoIncendios *sistema) {
    int opcao;

    do {
        printf("\n=== RELATÓRIOS ===\n");

        printf("\n    OCORRÊNCIAS \n");
        printf("1. Ocorrências por Estado\n");
        printf("2. Ocorrências por Tipo\n");
        printf("3. Ocorrências por Prioridade\n");
        printf("4. Tempo de Resposta das Ocorrências\n");
        printf("5. Ocorrências por Localização\n");
        printf("6. Frequência de Incidentes (Ano)\n");

        printf("\n    INTERVENÇÕES    \n");
        printf("7. Intervenções por Estado\n");
        printf("8. Duração Média das Intervenções\n");
        printf("9. Recursos Mais Utilizados\n");
        printf("10. Eficiência das Intervenções\n");

        printf("\n    BOMBEIROS    \n");
        printf("11. Disponibilidade por Especialidade\n");
        printf("12. Ranking de Desempenho dos Bombeiros\n");

        printf("\n    EQUIPAMENTOS    \n");
        printf("13. Inventário de Equipamentos\n");
        printf("14. Equipamentos em Manutenção\n");
        printf("15. Utilização de Equipamentos por Tipo de Intervenção\n");
        printf("16. Ranking de Equipamentos Mais Utilizados\n");

        printf("\n    RELATÓRIOS EXTRA    \n");
        printf("17. Capacidade Operacional\n");
        printf("18. Carga de Trabalho por Período\n");

        printf("\n0. Voltar\n");

        opcao = getInt(0, 18, "Escolha uma opção");

        switch (opcao) {
            case 1:  relatorioOcorrenciasPorEstado(sistema); break;
            case 2:  relatorioOcorrenciasPorTipo(sistema); break;
            case 3:  relatorioOcorrenciasPorPrioridade(sistema); break;
            case 4:  relatorioTempoRespostaOcorrencias(sistema); break;
            case 5:  relatorioOcorrenciasPorLocalizacao(sistema); break;
            case 6:  relatorioFrequenciaIncidentes(sistema); break;

            case 7:  relatorioIntervencoesPorEstado(sistema); break;
            case 8:  relatorioDuracaoMediaIntervencoes(sistema); break;
            case 9:  relatorioRecursosMaisUtilizados(sistema); break;
            case 10: relatorioEficienciaIntervencoes(sistema); break;

            case 11: relatorioDisponibilidadeBombeiros(sistema); break;
            case 12: relatorioRankingDesempenhoBombeiros(sistema); break;

            case 13: relatorioInventarioEquipamentos(sistema); break;
            case 14: relatorioEquipamentosEmManutencao(sistema); break;
            case 15: relatorioUtilizacaoEquipamentosPorTipoIntervencao(sistema); break;
            case 16: relatorioRankingEquipamentos(sistema); break;

            case 17: relatorioCapacidadeOperacional(sistema); break;
            case 18: relatorioCargaTrabalhoPeriodo(sistema); break;

            case 0:  break;
            default: printf("Opção inválida.\n");
        }

    } while (opcao != 0);
}
