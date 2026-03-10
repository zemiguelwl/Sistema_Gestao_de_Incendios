#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "modelos.h"
#include "menu.h"
#include "persistencia.h"
#include "logs.h"
#include "utils.h"

/**
 * @file main.c
 * @brief Ponto de entrada do Sistema de Gestão de Incêndios (SGI).
 *
 * Responsável por:
 *  - Inicializar listas e estruturas do sistema
 *  - Carregar dados persistidos
 *  - Iniciar o menu principal
 *  - Guardar dados antes de sair
 *  - Libertar toda a memória alocada
 *  - Controlar logs de arranque e encerramento
 */

int main(void) {
   // Configuração da consola
   SetConsoleOutputCP(CP_UTF8);
   SetConsoleCP(CP_UTF8);
   setvbuf(stdout, NULL, _IONBF, 0);


    SistemaGestaoIncendios sistema;

    /* Inicializar sistema de logs */

    inicializarLogs();
    registarLog("INFO", "MAIN", "INICIAR", "Sistema SGI a iniciar");

    /* Inicializar todas as listas dinâmicas */

    inicializarListaOcorrencias(&sistema.ocorrencias);
    inicializarListaIntervencoes(&sistema.intervencoes);
    inicializarListaBombeiros(&sistema.bombeiros);
    inicializarListaEquipamentos(&sistema.equipamentos);

    /* Inicializar contadores de IDs */
    sistema.proximoIdOcorrencia     = 1;
    sistema.proximoIdIntervencao    = 1;
    sistema.proximoIdBombeiro       = 1;
    sistema.proximoIdEquipamento    = 1;

    /* Carregar dados do disco (se existirem)*/
    carregarDados(&sistema);


    /* Entrar no menu principal*/
    mostrarMenuPrincipal(&sistema);



    /* Guardar dados */
    guardarDados(&sistema);
    registarLog("INFO", "MAIN", "GUARDAR", "Dados guardados com sucesso");

    /* Libertar memória */
    libertarListaOcorrencias(&sistema.ocorrencias);
    libertarListaIntervencoes(&sistema.intervencoes);
    libertarListaBombeiros(&sistema.bombeiros);
    libertarListaEquipamentos(&sistema.equipamentos);

    /* Encerrar logs */
    registarLog("INFO", "MAIN", "ENCERRAR", "Sistema encerrado com sucesso");
    fecharLogs();

    printf("\nSistema encerrado com sucesso!\n");

    return 0;
}
