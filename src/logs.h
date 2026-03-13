#ifndef LOGS_H
#define LOGS_H

/**
 * @file logs.h
 * @brief Sistema de registo estruturado de ações e erros do SGI.
 *
 * Os logs são escritos em modo append no ficheiro `logs.txt`, preservando
 * sessões anteriores. Cada entrada segue o formato CSV:
 *
 *   YYYY-MM-DD HH:MM:SS;NIVEL;MODULO;ACAO;DETALHE
 *
 * Exemplo:
 *   2025-12-01 14:32:05;INFO;INTERVENCOES;CRIAR;ID=5;Ocorrencia=2
 *   2025-12-01 14:35:11;ERROR;UTILS;EXPANDIR_LISTA_BOMBEIROS;Falha ao expandir
 *   2025-12-01 14:40:00;WARNING;INTERVENCOES;INATIVAR;Tentativa bloqueada
 *
 * Níveis utilizados: INFO | WARNING | ERROR
 *
 * @note localtime() não é thread-safe. Para aplicações multi-thread,
 *       substituir por localtime_r() (POSIX) ou localtime_s() (Windows).
 *
 * @note Algumas operações de leitura (ex.: listar) têm log intencionalmente
 *       comentado para evitar ruído excessivo. Podem ser reativados para
 *       fins de debugging.
 */


/**
 * @brief Inicializa o sistema de logs e abre o ficheiro de destino.
 *
 * Abre `logs.txt` em modo append (preserva sessões anteriores).
 * Regista um cabeçalho de sessão com timestamp de arranque.
 * Se o ficheiro não puder ser aberto, emite aviso para stderr
 * e continua — o sistema funciona sem logs.
 *
 * Deve ser chamada uma única vez no início da execução, antes de
 * qualquer chamada a registarLog().
 *
 * @see fecharLogs()
 */
void inicializarLogs(void);

/**
 * @brief Regista uma entrada de log com timestamp automático.
 *
 * Escreve uma linha no formato:
 *   YYYY-MM-DD HH:MM:SS;NIVEL;MODULO;ACAO;DETALHE
 *
 * Se o ficheiro não estiver aberto (ex.: inicializarLogs() falhou),
 * tenta reabri-lo silenciosamente antes de escrever.
 * Chama fflush() após cada escrita para garantir persistência imediata.
 *
 * @param nivel   Severidade da entrada: "INFO", "WARNING" ou "ERROR"
 * @param modulo  Módulo de origem: ex. "INTERVENCOES", "UTILS", "PERSISTENCIA"
 * @param acao    Operação executada: ex. "CRIAR", "INATIVAR", "FREE_BOMBEIROS"
 * @param detalhe Contexto adicional: ex. "ID=5;Ocorrencia=2" ou mensagem de erro
 *
 * @note Todos os parâmetros devem ser strings não-NULL.
 */
void registarLog(const char *nivel, const char *modulo,
                 const char *acao,  const char *detalhe);

/**
 * @brief Regista o encerramento da sessão e fecha o ficheiro de logs.
 *
 * Escreve um rodapé de sessão com timestamp de encerramento,
 * faz flush e fecha o ficheiro. Define o ponteiro interno para NULL.
 * Chamadas subsequentes a registarLog() após fecharLogs() reabrirão
 * o ficheiro automaticamente.
 *
 * Deve ser chamada uma única vez antes de terminar a aplicação,
 * após a última chamada a registarLog().
 *
 * @see inicializarLogs()
 */
void fecharLogs(void);


#endif /* LOGS_H */