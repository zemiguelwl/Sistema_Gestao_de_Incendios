#include <stdio.h>
#include <sys/stat.h>
#include "modelos.h"
#include "menu.h"
#include "persistencia.h"
#include "logs.h"
#include "utils.h"

/* Configuração da consola — apenas compilado em Windows */
#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#define CRIAR_DIRETORIO(p) _mkdir(p)
#else
#define CRIAR_DIRETORIO(p) mkdir((p), 0755)
#endif

/**
 * @file main.c
 * @brief Ponto de entrada do Sistema de Gestão de Incêndios (SGI).
 *
 * Responsável por:
 *  - Configurar a consola (encoding UTF-8, em Windows)
 *  - Inicializar o sistema de logs
 *  - Inicializar todas as listas dinâmicas
 *  - Carregar dados persistidos em disco
 *  - Lançar o menu principal
 *  - Guardar dados antes de sair
 *  - Libertar toda a memória alocada
 *  - Encerrar os logs
 */

int main(void) {

    /* Configuração da consola para UTF-8 (específico de Windows) */
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    setvbuf(stdout, NULL, _IONBF, 0);
#endif

    SistemaGestaoIncendios sistema;

    /* Garantir que os diretórios de dados e logs existem (ignora erro se já existirem) */
    (void)CRIAR_DIRETORIO("data");
    (void)CRIAR_DIRETORIO("logs");

    /* Inicializar sistema de logs */
    inicializarLogs();
    registarLog("INFO", "MAIN", "INICIAR", "Sistema SGI a iniciar");

    /* Inicializar todas as listas dinâmicas */
    inicializarListaOcorrencias(&sistema.ocorrencias);
    inicializarListaIntervencoes(&sistema.intervencoes);
    inicializarListaBombeiros(&sistema.bombeiros);
    inicializarListaEquipamentos(&sistema.equipamentos);

    /* Inicializar contadores de IDs */
    sistema.proximoIdOcorrencia  = 1;
    sistema.proximoIdIntervencao = 1;
    sistema.proximoIdBombeiro    = 1;
    sistema.proximoIdEquipamento = 1;

    /* Carregar dados persistidos (se existirem) */
    carregarDados(&sistema);

    /* Entrar no menu principal */
    mostrarMenuPrincipal(&sistema);

    /* Guardar dados antes de sair */
    guardarDados(&sistema);
    registarLog("INFO", "MAIN", "GUARDAR", "Dados guardados com sucesso");

    /* Libertar toda a memória alocada */
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