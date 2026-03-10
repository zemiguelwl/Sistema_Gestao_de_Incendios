#ifndef UTILS_H
#define UTILS_H

#include "modelos.h"

/**
 * @file utils.h
 * @brief Funções auxiliares: input, validações, listas dinâmicas, datas,
 *        conversões para string e menus de escolha.
 */


/**
 * @brief Lê um inteiro do stdin, garantindo que está dentro de um intervalo.
 *
 * Repete a leitura até o utilizador fornecer um valor válido.
 *
 * @param min Valor mínimo permitido (inclusivo).
 * @param max Valor máximo permitido (inclusivo).
 * @param msg Mensagem a apresentar ao utilizador.
 * @return Valor inteiro lido e validado.
 */
int getInt(int min, int max, const char *msg);

/**
 * @brief Lê uma string do stdin com tamanho máximo definido.
 *
 * Remove o '\n' final, se existir.
 *
 * @param destino Buffer de destino onde será armazenada a string.
 * @param tamanhoMax Tamanho máximo do buffer.
 * @param msg Mensagem a apresentar ao utilizador.
 */
void lerString(char *destino, int tamanhoMax, const char *msg);

/**
 * @brief Lê uma data e hora completas do utilizador.
 *
 * Garante que a data é válida (dia/mês/ano).
 *
 * @param msg Mensagem a apresentar antes da leitura.
 * @return Estrutura DataHora preenchida.
 */
DataHora lerDataHora(const char *msg);


/**
 * @brief Valida se uma data (dia, mês, ano) é válida.
 *
 * @param dia Dia do mês.
 * @param mes Mês (1-12).
 * @param ano Ano (>= 2000).
 * @return 1 se a data for válida, 0 caso contrário.
 */
int validarData(int dia, int mes, int ano);

/**
 * @brief Verifica se um determinado ano é bissexto.
 *
 * @param ano Ano a testar.
 * @return 1 se for bissexto, 0 caso contrário.
 */
int anoBissexto(int ano);



/**
 * @brief Inicializa a lista de ocorrências.
 *
 *
 * @param lista Ponteiro para a lista a inicializar.
 */
void inicializarListaOcorrencias(ListaOcorrencias *lista);

/**
 * @brief Inicializa a lista de intervenções.
 *
 * @param lista Ponteiro para a lista a inicializar.
 */
void inicializarListaIntervencoes(ListaIntervencoes *lista);

/**
 * @brief Inicializa a lista de bombeiros.
 *
 * @param lista Ponteiro para a lista a inicializar.
 */
void inicializarListaBombeiros(ListaBombeiros *lista);

/**
 * @brief Inicializa a lista de equipamentos.
 *
 * @param lista Ponteiro para a lista a inicializar.
 */
void inicializarListaEquipamentos(ListaEquipamentos *lista);

/**
 * @brief Liberta a memória associada à lista de ocorrências.
 *
 * @param lista Ponteiro para a lista a libertar.
 */
void libertarListaOcorrencias(ListaOcorrencias *lista);

/**
 * @brief Liberta a memória associada à lista de intervenções,
 * incluindo os arrays internos de bombeiros/equipamentos.
 *
 * @param lista Ponteiro para a lista a libertar.
 */
void libertarListaIntervencoes(ListaIntervencoes *lista);

/**
 * @brief Liberta a memória associada à lista de bombeiros.
 *
 * @param lista Ponteiro para a lista a libertar.
 */
void libertarListaBombeiros(ListaBombeiros *lista);

/**
 * @brief Liberta a memória associada à lista de equipamentos.
 *
 * @param lista Ponteiro para a lista a libertar.
 */
void libertarListaEquipamentos(ListaEquipamentos *lista);

/**
 * @brief Garante espaço suficiente na lista de ocorrências,
 *        expandindo a capacidade se necessário.
 *
 * @param lista Ponteiro para a lista.
 * @return 1 em caso de sucesso, 0 se a realocação falhar.
 */
int expandirListaOcorrenciasSeNecessario(ListaOcorrencias *lista);

/**
 * @brief Expande a lista de bombeiros se a capacidade atual estiver esgotada.
 *
 * @param lista Ponteiro para a lista.
 * @return 1 em caso de sucesso, 0 em caso de erro.
 */
int expandirListaBombeirosSeNecessario(ListaBombeiros *lista);

/**
 * @brief Expande a lista de equipamentos se necessário.
 *
 * @param lista Ponteiro para a lista.
 * @return 1 em caso de sucesso, 0 em caso de erro.
 */
int expandirListaEquipamentosSeNecessario(ListaEquipamentos *lista);

/**
 * @brief Expande a lista de intervenções se necessário.
 *
 * @param lista Ponteiro para a lista.
 * @return 1 em caso de sucesso, 0 em caso de erro.
 */
int expandirListaIntervencoesSeNecessario(ListaIntervencoes *lista);

/**
 * @brief Expande o array de IDs de bombeiros associado a uma intervenção.
 *
 * @param intervencao Ponteiro para a intervenção.
 * @return 1 em caso de sucesso, 0 em caso de erro.
 */
int expandirArrayBombeirosIntervencao(Intervencao *intervencao);

/**
 * @brief Expande o array de IDs de equipamentos associado a uma intervenção.
 *
 * @param intervencao Ponteiro para a intervenção.
 * @return 1 em caso de sucesso, 0 em caso de erro.
 */
int expandirArrayEquipamentosIntervencao(Intervencao *intervencao);



/**
 * @brief Calcula a diferença em minutos entre duas datas, com maior precisão.
 *
 * Converte ambas as datas para um número absoluto de minutos desde ano 0.
 *
 * @param d1 Primeira data/hora.
 * @param d2 Segunda data/hora.
 * @return Diferença em minutos (d2 - d1).
 */
int minutosEntre(DataHora d1, DataHora d2);

/**
 * @brief Verifica se uma data/hora de fim é posterior à de início.
 *
 * @param inicio Ponteiro para a data/hora inicial.
 * @param fim Ponteiro para a data/hora final.
 * @return 1 se fim > inicio, 0 caso contrário.
 */
int validarDataHoraFimMaiorQueInicio(const DataHora *inicio, const DataHora *fim);



/**
 * @brief Converte um EstadoOcorrencia numa string legível.
 */
const char* estadoOcorrenciaParaString(EstadoOcorrencia estado);

/**
 * @brief Converte um EstadoIntervencao numa string legível.
 */
const char* estadoIntervencaoParaString(EstadoIntervencao estado);

/**
 * @brief Converte uma EspecialidadeBombeiro em string legível.
 */
const char* especialidadeBombeiroParaString(EspecialidadeBombeiro esp);

/**
 * @brief Converte um EstadoOperacionalBombeiro em string legível.
 */
const char* estadoBombeiroParaString(EstadoOperacionalBombeiro est);

/**
 * @brief Converte um TipoEquipamento em string legível.
 */
const char* tipoEquipamentoParaString(TipoEquipamento tipo);

/**
 * @brief Converte um EstadoEquipamento em string legível.
 */
const char* estadoEquipamentoParaString(EstadoEquipamento estado);

/**
 * @brief Converte um TipoOcorrencia em string legível.
 */
const char* tipoOcorrenciaParaString(TipoOcorrencia tipo);

/**
 * @brief Converte uma PrioridadeOcorrencia em string legível.
 */
const char* prioridadeParaString(PrioridadeOcorrencia prio);



/**
 * @brief Mostra um menu e permite escolher um tipo de ocorrência.
 *
 * @return Tipo de ocorrência escolhido.
 */
TipoOcorrencia escolherTipoOcorrencia(void);

/**
 * @brief Mostra um menu e permite escolher uma prioridade de ocorrência.
 *
 * @return Prioridade escolhida.
 */
PrioridadeOcorrencia escolherPrioridadeOcorrencia(void);

/**
 * @brief Mostra um menu e permite escolher a especialidade de um bombeiro.
 *
 * @return Especialidade escolhida.
 */
EspecialidadeBombeiro escolherEspecialidadeBombeiro(void);

/**
 * @brief Mostra um menu e permite escolher o estado operacional de um bombeiro.
 *
 * @return Estado escolhido.
 */
EstadoOperacionalBombeiro escolherEstadoBombeiro(void);

/**
 * @brief Mostra um menu e permite escolher o tipo de equipamento.
 *
 * @return Tipo escolhido.
 */
TipoEquipamento escolherTipoEquipamento(void);

/**
 * @brief Mostra um menu e permite escolher o estado de um equipamento.
 *
 * @return Estado escolhido.
 */
EstadoEquipamento escolherEstadoEquipamento(void);

#endif /* UTILS_H */
