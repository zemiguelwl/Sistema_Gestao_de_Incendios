#ifndef UTILS_H
#define UTILS_H

/**
 * @file utils.h
 * @brief Funções utilitárias transversais ao Sistema de Gestão de Incêndios.
 *
 * Este módulo centraliza operações de uso geral:
 *  - Leitura e validação de input do utilizador
 *  - Inicialização e libertação das listas dinâmicas
 *  - Cálculos e validações de datas
 *  - Conversões enum → string (implementadas aqui, declaradas nos módulos)
 *  - Menus interativos de escolha (implementados aqui, declarados nos módulos)
 *
 * @note As funções de conversão para string e os menus interativos estão
 *       declarados nos módulos de entidade respetivos (bombeiros.h,
 *       equipamentos.h, etc.) mas implementados neste ficheiro para
 *       centralizar as implementações e evitar duplicação de código.
 *
 * @author José (8220942)
 * @date 2024
 * @version 2.0
 */

#include "modelos.h"


/* ========================================================================= */
/*  INPUT E VALIDAÇÃO                                                        */
/* ========================================================================= */

/**
 * @brief Lê um inteiro do stdin garantindo que está dentro de um intervalo.
 *
 * Repete a leitura até o utilizador fornecer um valor válido.
 * Descarta qualquer input inválido restante no buffer.
 *
 * @param min Valor mínimo permitido (inclusivo)
 * @param max Valor máximo permitido (inclusivo)
 * @param msg Mensagem a apresentar ao utilizador
 * @return Valor inteiro lido e validado
 */
int getInt(int min, int max, const char *msg);

/**
 * @brief Lê uma string do stdin com tamanho máximo definido.
 *
 * Remove o '\n' final se existir. Garante terminação nula.
 *
 * @param destino    Buffer de destino onde a string será armazenada
 * @param tamanhoMax Tamanho máximo do buffer (incluindo '\0')
 * @param msg        Mensagem a apresentar ao utilizador
 */
void lerString(char *destino, int tamanhoMax, const char *msg);

/**
 * @brief Lê uma data e hora completas do utilizador com validação.
 *
 * Solicita dia, mês, ano, hora e minuto separadamente.
 * Repete a leitura de campos inválidos.
 *
 * @param msg Mensagem a apresentar antes da leitura
 * @return Estrutura DataHora preenchida e validada
 */
DataHora lerDataHora(const char *msg);


/* ========================================================================= */
/*  VALIDAÇÃO DE DATAS                                                       */
/* ========================================================================= */

/**
 * @brief Valida se uma combinação de dia, mês e ano é calendáricamente correta.
 *
 * @param dia Dia do mês
 * @param mes Mês (1–12)
 * @param ano Ano (>= 2000)
 * @return 1 se a data for válida, 0 caso contrário
 */
int validarData(int dia, int mes, int ano);

/**
 * @brief Verifica se um ano é bissexto.
 *
 * @param ano Ano a testar
 * @return 1 se for bissexto, 0 caso contrário
 */
int anoBissexto(int ano);

/**
 * @brief Calcula a diferença em minutos entre duas datas.
 *
 * Converte ambas as datas para minutos absolutos para garantir
 * precisão em cálculos que atravessam meses e anos.
 *
 * @param d1 Data/hora inicial
 * @param d2 Data/hora final
 * @return Diferença em minutos (d2 - d1); negativo se d2 < d1
 */
int minutosEntre(DataHora d1, DataHora d2);

/**
 * @brief Verifica se uma data/hora de fim é estritamente posterior à de início.
 *
 * @param inicio Ponteiro para a data/hora inicial
 * @param fim    Ponteiro para a data/hora final
 * @return 1 se fim > inicio, 0 caso contrário
 */
int validarDataHoraFimMaiorQueInicio(const DataHora *inicio, const DataHora *fim);


/* ========================================================================= */
/*  INICIALIZAÇÃO DE LISTAS DINÂMICAS                                       */
/* ========================================================================= */

/**
 * @brief Inicializa a lista de ocorrências com capacidade inicial.
 *
 * @param lista Ponteiro para a lista a inicializar
 */
void inicializarListaOcorrencias(ListaOcorrencias *lista);

/**
 * @brief Inicializa a lista de intervenções com capacidade inicial.
 *
 * @param lista Ponteiro para a lista a inicializar
 */
void inicializarListaIntervencoes(ListaIntervencoes *lista);

/**
 * @brief Inicializa a lista de bombeiros com capacidade inicial.
 *
 * @param lista Ponteiro para a lista a inicializar
 */
void inicializarListaBombeiros(ListaBombeiros *lista);

/**
 * @brief Inicializa a lista de equipamentos com capacidade inicial.
 *
 * @param lista Ponteiro para a lista a inicializar
 */
void inicializarListaEquipamentos(ListaEquipamentos *lista);


/* ========================================================================= */
/*  LIBERTAÇÃO DE LISTAS DINÂMICAS                                          */
/* ========================================================================= */

/**
 * @brief Liberta a memória alocada pela lista de ocorrências.
 *
 * @param lista Ponteiro para a lista a libertar
 */
void libertarListaOcorrencias(ListaOcorrencias *lista);

/**
 * @brief Liberta a memória alocada pela lista de intervenções.
 *
 * Liberta também os arrays internos de cada intervenção
 * (idsBombeiros e idsEquipamentos).
 *
 * @param lista Ponteiro para a lista a libertar
 *
 * @warning Deve ser chamada antes de libertar o sistema para evitar
 *          fugas de memória nos arrays internos das intervenções
 */
void libertarListaIntervencoes(ListaIntervencoes *lista);

/**
 * @brief Liberta a memória alocada pela lista de bombeiros.
 *
 * @param lista Ponteiro para a lista a libertar
 */
void libertarListaBombeiros(ListaBombeiros *lista);

/**
 * @brief Liberta a memória alocada pela lista de equipamentos.
 *
 * @param lista Ponteiro para a lista a libertar
 */
void libertarListaEquipamentos(ListaEquipamentos *lista);


/* ========================================================================= */
/*  CONVERSÃO ENUM → STRING (alias para uso cross-módulo)                  */
/* ========================================================================= */

/**
 * @brief Alias de prioridadeOcorrenciaParaString() para uso em outros módulos.
 *
 * Declarada aqui para que módulos como intervencoes.c possam usar
 * prioridadeParaString() sem incluir ocorrencias.h diretamente.
 *
 * @param prio Prioridade a converter
 * @return String descritiva da prioridade
 */
const char *prioridadeParaString(PrioridadeOcorrencia prio);


#endif /* UTILS_H */