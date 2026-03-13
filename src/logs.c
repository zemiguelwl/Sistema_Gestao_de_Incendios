#include "logs.h"
#include <stdio.h>
#include <time.h>

/**
 * @file logs.c
 * @brief Implementação do sistema de registo de atividade (logs).
 *
 * Os logs são escritos em formato CSV no ficheiro FICHEIRO_LOGS,
 * com uma linha por entrada no formato:
 *   YYYY-MM-DD HH:MM:SS;NIVEL;MODULO;ACAO;DETALHE
 *
 * O ficheiro é aberto em modo append para preservar sessões anteriores.
 *
 * @note localtime() não é thread-safe. Para aplicações multi-thread,
 *       substituir por localtime_r() (POSIX) ou localtime_s() (Windows).
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

    fprintf(ficheiroLogs, "\n=== SESSÃO INICIADA ===\n");
    fprintf(ficheiroLogs, "%04d-%02d-%02d %02d:%02d:%02d;INFO;SISTEMA;INICIAR;Nova sessão iniciada\n",
            tempoLocal->tm_year + 1900,
            tempoLocal->tm_mon  + 1,
            tempoLocal->tm_mday,
            tempoLocal->tm_hour,
            tempoLocal->tm_min,
            tempoLocal->tm_sec);

    fflush(ficheiroLogs);
}


void registarLog(const char *nivel, const char *modulo,
                 const char *acao, const char *detalhe) {

    /* Abrir o ficheiro se ainda não estiver aberto */
    if (ficheiroLogs == NULL) {
        ficheiroLogs = fopen(FICHEIRO_LOGS, "a");
        if (ficheiroLogs == NULL) return;
    }

    time_t agora = time(NULL);
    struct tm *tempoLocal = localtime(&agora);

    /* Formato: YYYY-MM-DD HH:MM:SS;NIVEL;MODULO;ACAO;DETALHE */
    fprintf(ficheiroLogs, "%04d-%02d-%02d %02d:%02d:%02d;%s;%s;%s;%s\n",
            tempoLocal->tm_year + 1900,
            tempoLocal->tm_mon  + 1,
            tempoLocal->tm_mday,
            tempoLocal->tm_hour,
            tempoLocal->tm_min,
            tempoLocal->tm_sec,
            nivel, modulo, acao, detalhe);

    fflush(ficheiroLogs);
}


void fecharLogs(void) {
    if (ficheiroLogs == NULL) return;

    time_t agora = time(NULL);
    struct tm *tempoLocal = localtime(&agora);

    fprintf(ficheiroLogs, "%04d-%02d-%02d %02d:%02d:%02d;INFO;SISTEMA;ENCERRAR;Sessão encerrada\n",
            tempoLocal->tm_year + 1900,
            tempoLocal->tm_mon  + 1,
            tempoLocal->tm_mday,
            tempoLocal->tm_hour,
            tempoLocal->tm_min,
            tempoLocal->tm_sec);
    fprintf(ficheiroLogs, "=== SESSÃO ENCERRADA ===\n\n");

    fclose(ficheiroLogs);
    ficheiroLogs = NULL;
}