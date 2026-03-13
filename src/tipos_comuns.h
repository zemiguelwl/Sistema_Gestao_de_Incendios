#ifndef TIPOS_COMUNS_H
#define TIPOS_COMUNS_H

/**
 * @file tipos_comuns.h
 * @brief Constantes globais e tipos base partilhados por todos os módulos
 *        do Sistema de Gestão de Incêndios (SGI).
 *
 * Este ficheiro deve ser incluído por todos os módulos de entidade.
 * Não contém definições de entidades específicas nem do contexto global.
 *
 * @author José (8220942)
 * @date 2024
 * @version 2.0
 */


/* ========================================================================= */
/*  CONSTANTES GLOBAIS                                                       */
/* ========================================================================= */

#define MAX_LOCAL        100   /**< Tamanho máximo para nomes de localidades */
#define MAX_NOME         100   /**< Tamanho máximo para nomes de pessoas     */
#define MAX_DESIGNACAO   100   /**< Tamanho máximo para designações          */
#define MAX_LINHA        500   /**< Tamanho máximo para leitura de linhas    */

#define CAPACIDADE_INICIAL 10  /**< Capacidade inicial das listas dinâmicas  */

#define DATA_NAO_DEFINIDA -1   /**< Sentinela para datas não definidas       */


/* ========================================================================= */
/*  ESTRUTURA DE DATA E HORA                                                 */
/* ========================================================================= */

/**
 * @brief Representa uma data e hora completas.
 *
 * Utilizada em ocorrências e intervenções para registo temporal de eventos.
 */
typedef struct {
    int dia;     /**< Dia do mês (1–31)   */
    int mes;     /**< Mês do ano  (1–12)  */
    int ano;     /**< Ano (ex.: 2024)     */
    int hora;    /**< Hora (0–23)         */
    int minuto;  /**< Minuto (0–59)       */
} DataHora;


#endif /* TIPOS_COMUNS_H */