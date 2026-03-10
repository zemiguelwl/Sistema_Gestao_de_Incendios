#include <stdio.h>
#include <time.h>
#include "logs.h"

/**
 * @file logs.c
 * @brief Implementação do sistema de logs.
 */

#define FICHEIRO_LOGS "logs.txt"

static FILE *ficheiroLogs = NULL;

void inicializarLogs(void) {

    ficheiroLogs = fopen(FICHEIRO_LOGS, "a");
    
    if (ficheiroLogs == NULL) {
        fprintf(stderr, "AVISO: Não foi possível abrir o ficheiro de logs.\n");
        return;
    }
    

    time_t agora = time(NULL);
    struct tm *tempoLocal = localtime(&agora);
    
    fprintf(ficheiroLogs, "\n SESSÃO INICIADA \n");
    fprintf(ficheiroLogs, "%04d-%02d-%02d %02d:%02d:%02d;INFO;SISTEMA;INICIAR;Nova sessão iniciada\n",
            tempoLocal->tm_year + 1900,
            tempoLocal->tm_mon + 1,
            tempoLocal->tm_mday,
            tempoLocal->tm_hour,
            tempoLocal->tm_min,
            tempoLocal->tm_sec);
    
    fflush(ficheiroLogs);
}

void registarLog(const char *nivel, const char *modulo, const char *acao, const char *detalhe) {
    if (ficheiroLogs == NULL) {
        ficheiroLogs = fopen(FICHEIRO_LOGS, "a");
        if (ficheiroLogs == NULL) {
            return;
        }
    }
    
    /* Obter timestamp atual */
    time_t agora = time(NULL);
    struct tm *tempoLocal = localtime(&agora);
    
    /* Escrever linha de log no formato: YYYY-MM-DD HH:MM:SS;NIVEL;MODULO;ACAO;DETALHE */
    fprintf(ficheiroLogs, "%04d-%02d-%02d %02d:%02d:%02d;%s;%s;%s;%s\n",
            tempoLocal->tm_year + 1900,
            tempoLocal->tm_mon + 1,
            tempoLocal->tm_mday,
            tempoLocal->tm_hour,
            tempoLocal->tm_min,
            tempoLocal->tm_sec,
            nivel,
            modulo,
            acao,
            detalhe);
    

    fflush(ficheiroLogs);
}

void fecharLogs(void) {
    if (ficheiroLogs != NULL) {

        time_t agora = time(NULL);
        struct tm *tempoLocal = localtime(&agora);
        
        fprintf(ficheiroLogs, "%04d-%02d-%02d %02d:%02d:%02d;INFO;SISTEMA;ENCERRAR;Sessão encerrada\n",
                tempoLocal->tm_year + 1900,
                tempoLocal->tm_mon + 1,
                tempoLocal->tm_mday,
                tempoLocal->tm_hour,
                tempoLocal->tm_min,
                tempoLocal->tm_sec);
        fprintf(ficheiroLogs, " SESSÃO ENCERRADA \n\n");
        
        fclose(ficheiroLogs);
        ficheiroLogs = NULL;
    }
}