#ifndef LOGS_H
#define LOGS_H

/**
 * @file logs.h
 * @brief Funções para registo estruturado de ações e erros do sistema.
 *
 */

/**
 * @brief Inicializa o sistema de logs.
 *
 * Deve ser chamado no início da execução da aplicação.
 */
void inicializarLogs(void);

/**
 * @brief Regista uma entrada de log.
 *
 * @param nivel Nível do log (ex.: "INFO", "WARNING", "ERROR").
 * @param modulo Módulo responsável pela operação (ex.: "INTERVENCOES").
 * @param acao Ação executada (ex.: "CRIAR", "ATUALIZAR").
 * @param detalhe Informação adicional relevante.
 */
void registarLog(const char *nivel, const char *modulo,
                 const char *acao, const char *detalhe);

/**
 * @brief Fecha o sistema de logs e liberta recursos associados.
 */
void fecharLogs(void);

#endif /* LOGS_H */
