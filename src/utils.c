#include "utils.h"
#include "logs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @file utils.c
 * @brief Funções auxiliares gerais do sistema.
 *
 * Funcionalidades:
 *  - Leitura e validação de input (getInt, lerString, lerDataHora)
 *  - Validação de datas e cálculo de diferenças temporais
 *  - Inicialização e libertação de listas dinâmicas
 *  - Expansão dinâmica de listas e arrays internos de intervenções
 *  - Conversões enum → string para todos os módulos
 *  - Menus de escolha de tipos e estados
 */


/* ========================================================================= */
/*  INPUT E VALIDAÇÃO                                                        */
/* ========================================================================= */

int getInt(int min, int max, const char *msg) {
    char buffer[100];
    int  valor;
    char extra;

    while (1) {
        if (msg != NULL && msg[0] != '\0')
            printf("%s (entre %d e %d): ", msg, min, max);

        if (!fgets(buffer, sizeof(buffer), stdin)) {
            printf("Erro ao ler input.\n");
            continue;
        }

        /* Rejeitar se houver texto após o número */
        if (sscanf(buffer, "%d %c", &valor, &extra) != 1) {
            printf("Erro: Introduza apenas um número inteiro.\n");
            continue;
        }

        if (valor < min || valor > max) {
            printf("Erro: Valor fora do intervalo permitido [%d, %d].\n", min, max);
            continue;
        }

        return valor;
    }
}

void lerString(char *destino, int tamanhoMax, const char *msg) {
    do {
        printf("%s: ", msg);

        if (fgets(destino, tamanhoMax, stdin) == NULL) {
            destino[0] = '\0';
        } else {
            size_t len = strlen(destino);
            if (len > 0 && destino[len - 1] == '\n')
                destino[len - 1] = '\0';
        }

        if (strlen(destino) == 0)
            printf("Erro: Campo obrigatório. Por favor introduza um valor válido.\n");

    } while (strlen(destino) == 0);
}


/* ========================================================================= */
/*  VALIDAÇÃO DE DATAS                                                       */
/* ========================================================================= */

int anoBissexto(int ano) {
    return (ano % 4 == 0 && ano % 100 != 0) || (ano % 400 == 0);
}

int validarData(int dia, int mes, int ano) {
    if (ano < 2000)          return 0;
    if (mes < 1 || mes > 12) return 0;

    int diasPorMes[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (anoBissexto(ano)) diasPorMes[1] = 29;

    if (dia < 1 || dia > diasPorMes[mes - 1]) return 0;
    return 1;
}

DataHora lerDataHora(const char *msg) {
    DataHora dh;

    printf("%s\n", msg);

    do {
        dh.dia    = getInt(1,    31,   "Dia");
        dh.mes    = getInt(1,    12,   "Mês");
        dh.ano    = getInt(2000, 2100, "Ano");

        if (!validarData(dh.dia, dh.mes, dh.ano)) {
            printf("Erro: Data inválida. Tente novamente.\n");
            continue;
        }

        dh.hora   = getInt(0, 23, "Hora");
        dh.minuto = getInt(0, 59, "Minuto");
        break;

    } while (1);

    return dh;
}

int validarDataHoraFimMaiorQueInicio(const DataHora *inicio, const DataHora *fim) {
    if (fim->ano  > inicio->ano)  return 1;
    if (fim->ano  < inicio->ano)  return 0;
    if (fim->mes  > inicio->mes)  return 1;
    if (fim->mes  < inicio->mes)  return 0;
    if (fim->dia  > inicio->dia)  return 1;
    if (fim->dia  < inicio->dia)  return 0;
    if (fim->hora > inicio->hora) return 1;
    if (fim->hora < inicio->hora) return 0;
    return fim->minuto > inicio->minuto;
}


/* ========================================================================= */
/*  CÁLCULO DE DIFERENÇA TEMPORAL                                            */
/* ========================================================================= */

static long converterParaMinutos(DataHora d) {
    int diasPorMes[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    long dias = (long)d.ano * 365;

    /* Somar meses completos */
    for (int i = 1; i < d.mes; i++)
        dias += diasPorMes[i];

    /* Correcção de anos bissextos acumulados */
    int anos = d.ano - 1;
    dias += (anos / 4) - (anos / 100) + (anos / 400);

    /* Ajustar fevereiro do ano corrente se bissexto */
    if (anoBissexto(d.ano) && d.mes > 2)
        dias += 1;

    dias += d.dia;

    return dias * 1440L + d.hora * 60 + d.minuto;
}

int minutosEntre(DataHora d1, DataHora d2) {
    return (int)(converterParaMinutos(d2) - converterParaMinutos(d1));
}


/* ========================================================================= */
/*  INICIALIZAÇÃO DE LISTAS                                                  */
/* ========================================================================= */

void inicializarListaOcorrencias(ListaOcorrencias *lista) {
    lista->dados = malloc(CAPACIDADE_INICIAL * sizeof(Ocorrencia));
    if (lista->dados == NULL) {
        registarLog("ERROR", "UTILS", "INIT_OCORRENCIAS", "Falha ao alocar memória");
        fprintf(stderr, "ERRO FATAL: Não foi possível alocar memória para ocorrências.\n");
        exit(EXIT_FAILURE);
    }
    lista->tamanho   = 0;
    lista->capacidade = CAPACIDADE_INICIAL;
}

void inicializarListaIntervencoes(ListaIntervencoes *lista) {
    lista->dados = malloc(CAPACIDADE_INICIAL * sizeof(Intervencao));
    if (lista->dados == NULL) {
        registarLog("ERROR", "UTILS", "INIT_INTERVENCOES", "Falha ao alocar memória");
        fprintf(stderr, "ERRO FATAL: Não foi possível alocar memória para intervenções.\n");
        exit(EXIT_FAILURE);
    }
    lista->tamanho   = 0;
    lista->capacidade = CAPACIDADE_INICIAL;
}

void inicializarListaBombeiros(ListaBombeiros *lista) {
    lista->dados = malloc(CAPACIDADE_INICIAL * sizeof(Bombeiro));
    if (lista->dados == NULL) {
        registarLog("ERROR", "UTILS", "INIT_BOMBEIROS", "Falha ao alocar memória");
        fprintf(stderr, "ERRO FATAL: Não foi possível alocar memória para bombeiros.\n");
        exit(EXIT_FAILURE);
    }
    lista->tamanho   = 0;
    lista->capacidade = CAPACIDADE_INICIAL;
}

void inicializarListaEquipamentos(ListaEquipamentos *lista) {
    lista->dados = malloc(CAPACIDADE_INICIAL * sizeof(Equipamento));
    if (lista->dados == NULL) {
        registarLog("ERROR", "UTILS", "INIT_EQUIPAMENTOS", "Falha ao alocar memória");
        fprintf(stderr, "ERRO FATAL: Não foi possível alocar memória para equipamentos.\n");
        exit(EXIT_FAILURE);
    }
    lista->tamanho   = 0;
    lista->capacidade = CAPACIDADE_INICIAL;
}


/* ========================================================================= */
/*  LIBERTAÇÃO DE MEMÓRIA                                                    */
/* ========================================================================= */

void libertarListaOcorrencias(ListaOcorrencias *lista) {
    free(lista->dados);
    lista->dados     = NULL;
    lista->tamanho   = 0;
    lista->capacidade = 0;
    registarLog("INFO", "UTILS", "FREE_OCORRENCIAS", "Memória libertada");
}

void libertarListaIntervencoes(ListaIntervencoes *lista) {
    if (lista->dados != NULL) {
        /* Libertar arrays internos de cada intervenção primeiro */
        for (int i = 0; i < lista->tamanho; i++) {
            free(lista->dados[i].idsBombeiros);
            lista->dados[i].idsBombeiros = NULL;
            free(lista->dados[i].idsEquipamentos);
            lista->dados[i].idsEquipamentos = NULL;
        }
        free(lista->dados);
        lista->dados = NULL;
    }
    lista->tamanho   = 0;
    lista->capacidade = 0;
    registarLog("INFO", "UTILS", "FREE_INTERVENCOES",
                "Memória libertada (incluindo arrays internos)");
}

void libertarListaBombeiros(ListaBombeiros *lista) {
    free(lista->dados);
    lista->dados     = NULL;
    lista->tamanho   = 0;
    lista->capacidade = 0;
    registarLog("INFO", "UTILS", "FREE_BOMBEIROS", "Memória libertada");
}

void libertarListaEquipamentos(ListaEquipamentos *lista) {
    free(lista->dados);
    lista->dados     = NULL;
    lista->tamanho   = 0;
    lista->capacidade = 0;
    registarLog("INFO", "UTILS", "FREE_EQUIPAMENTOS", "Memória libertada");
}


/* ========================================================================= */
/*  EXPANSÃO DE LISTAS                                                       */
/* ========================================================================= */

int expandirListaOcorrenciasSeNecessario(ListaOcorrencias *lista) {
    if (lista->tamanho < lista->capacidade) return 1;

    int novaCapacidade = lista->capacidade * 2;
    Ocorrencia *novo   = realloc(lista->dados, novaCapacidade * sizeof(Ocorrencia));
    if (novo == NULL) {
        registarLog("ERROR", "UTILS", "EXPANDIR_LISTA_OCORRENCIAS", "Falha ao expandir");
        return 0;
    }
    lista->dados      = novo;
    lista->capacidade = novaCapacidade;
    registarLog("INFO", "UTILS", "EXPANDIR_LISTA_OCORRENCIAS", "Lista expandida");
    return 1;
}

int expandirListaBombeirosSeNecessario(ListaBombeiros *lista) {
    if (lista->tamanho < lista->capacidade) return 1;

    int novaCapacidade = lista->capacidade * 2;
    Bombeiro *novo     = realloc(lista->dados, novaCapacidade * sizeof(Bombeiro));
    if (novo == NULL) {
        registarLog("ERROR", "UTILS", "EXPANDIR_LISTA_BOMBEIROS", "Falha ao expandir");
        return 0;
    }
    lista->dados      = novo;
    lista->capacidade = novaCapacidade;
    registarLog("INFO", "UTILS", "EXPANDIR_LISTA_BOMBEIROS", "Lista expandida");
    return 1;
}

int expandirListaEquipamentosSeNecessario(ListaEquipamentos *lista) {
    if (lista->tamanho < lista->capacidade) return 1;

    int novaCapacidade = lista->capacidade * 2;
    Equipamento *novo  = realloc(lista->dados, novaCapacidade * sizeof(Equipamento));
    if (novo == NULL) {
        registarLog("ERROR", "UTILS", "EXPANDIR_LISTA_EQUIPAMENTOS", "Falha ao expandir");
        return 0;
    }
    lista->dados      = novo;
    lista->capacidade = novaCapacidade;
    registarLog("INFO", "UTILS", "EXPANDIR_LISTA_EQUIPAMENTOS", "Lista expandida");
    return 1;
}

int expandirListaIntervencoesSeNecessario(ListaIntervencoes *lista) {
    if (lista->tamanho < lista->capacidade) return 1;

    int novaCapacidade = lista->capacidade * 2;
    Intervencao *novo  = realloc(lista->dados, novaCapacidade * sizeof(Intervencao));
    if (novo == NULL) {
        registarLog("ERROR", "UTILS", "EXPANDIR_LISTA_INTERVENCOES", "Falha ao expandir");
        return 0;
    }
    lista->dados      = novo;
    lista->capacidade = novaCapacidade;
    registarLog("INFO", "UTILS", "EXPANDIR_LISTA_INTERVENCOES", "Lista expandida");
    return 1;
}

int expandirArrayBombeirosIntervencao(Intervencao *intervencao) {
    if (intervencao->numBombeiros < intervencao->capacidadeBombeiros) return 1;

    int novaCapacidade = intervencao->capacidadeBombeiros * 2;
    int *novo          = realloc(intervencao->idsBombeiros,
                                  novaCapacidade * sizeof(int));
    if (novo == NULL) {
        registarLog("ERROR", "UTILS", "EXPANDIR_ARRAY_BOMBEIROS", "Falha ao expandir");
        return 0;
    }
    intervencao->idsBombeiros        = novo;
    intervencao->capacidadeBombeiros = novaCapacidade;
    return 1;
}

int expandirArrayEquipamentosIntervencao(Intervencao *intervencao) {
    if (intervencao->numEquipamentos < intervencao->capacidadeEquipamentos) return 1;

    int novaCapacidade = intervencao->capacidadeEquipamentos * 2;
    int *novo          = realloc(intervencao->idsEquipamentos,
                                  novaCapacidade * sizeof(int));
    if (novo == NULL) {
        registarLog("ERROR", "UTILS", "EXPANDIR_ARRAY_EQUIPAMENTOS", "Falha ao expandir");
        return 0;
    }
    intervencao->idsEquipamentos        = novo;
    intervencao->capacidadeEquipamentos = novaCapacidade;
    return 1;
}


/* ========================================================================= */
/*  MENUS DE ESCOLHA — OCORRÊNCIAS                                          */
/* ========================================================================= */

TipoOcorrencia escolherTipoOcorrencia(void) {
    printf("\n    TIPO DE OCORRÊNCIA    \n");
    printf("1. Florestal\n");
    printf("2. Urbano\n");
    printf("3. Industrial\n");

    switch (getInt(1, 3, "Escolha o tipo")) {
        case 1: return FLORESTAL;
        case 2: return URBANO;
        case 3: return INDUSTRIAL;
        default: return URBANO;
    }
}

PrioridadeOcorrencia escolherPrioridadeOcorrencia(void) {
    printf("\n    PRIORIDADE    \n");
    printf("1. Baixa\n");
    printf("2. Normal\n");
    printf("3. Alta\n");

    switch (getInt(1, 3, "Escolha a prioridade")) {
        case 1: return BAIXA;
        case 2: return NORMAL;
        case 3: return ALTA;
        default: return NORMAL;
    }
}

EstadoOcorrencia escolherEstadoOcorrencia(void) {
    printf("\n    ESTADO DA OCORRÊNCIA    \n");
    printf("1. Reportada\n");
    printf("2. Em Intervenção\n");
    printf("3. Concluída\n");

    switch (getInt(1, 3, "Escolha o estado")) {
        case 1: return REPORTADA;
        case 2: return EM_INTERVENCAO;
        case 3: return CONCLUIDA;
        default: return REPORTADA;
    }
}


/* ========================================================================= */
/*  MENUS DE ESCOLHA — BOMBEIROS                                            */
/* ========================================================================= */

EspecialidadeBombeiro escolherEspecialidadeBombeiro(void) {
    printf("\n    ESPECIALIDADE DO BOMBEIRO    \n");
    printf("1. Combate Florestal\n");
    printf("2. Resgate\n");
    printf("3. Incêndio Urbano\n");

    switch (getInt(1, 3, "Escolha a especialidade")) {
        case 1: return COMBATE_FLORESTAL;
        case 2: return RESGATE;
        case 3: return INCENDIO_URBANO;
        default: return INCENDIO_URBANO;
    }
}

EstadoOperacionalBombeiro escolherEstadoBombeiro(void) {
    printf("\n    ESTADO OPERACIONAL    \n");
    printf("1. Disponível\n");
    printf("2. Em Intervenção\n");
    printf("3. Em Treino\n");

    switch (getInt(1, 3, "Defina o estado")) {
        case 1: return DISPONIVEL;
        case 2: return EM_INTERVENCAO_BOMBEIRO;
        case 3: return EM_TREINO;
        default: return DISPONIVEL;
    }
}


/* ========================================================================= */
/*  MENUS DE ESCOLHA — EQUIPAMENTOS                                         */
/* ========================================================================= */

TipoEquipamento escolherTipoEquipamento(void) {
    printf("\n    TIPO DE EQUIPAMENTO    \n");
    printf("1. Veículo\n");
    printf("2. Mangueira\n");
    printf("3. Respirador\n");

    switch (getInt(1, 3, "Escolha o tipo")) {
        case 1: return VEICULO;
        case 2: return MANGUEIRA;
        case 3: return RESPIRADOR;
        default: return VEICULO;
    }
}

EstadoEquipamento escolherEstadoEquipamento(void) {
    printf("\n    ESTADO DO EQUIPAMENTO    \n");
    printf("1. Disponível\n");
    printf("2. Em Uso\n");
    printf("3. Em Manutenção\n");

    switch (getInt(1, 3, "Escolha o estado")) {
        case 1: return EQ_DISPONIVEL;
        case 2: return EQ_EM_USO;
        case 3: return EQ_EM_MANUTENCAO;
        default: return EQ_DISPONIVEL;
    }
}


/* ========================================================================= */
/*  CONVERSÕES ENUM → STRING                                                */
/* ========================================================================= */

const char *tipoOcorrenciaParaString(TipoOcorrencia tipo) {
    switch (tipo) {
        case FLORESTAL:  return "Florestal";
        case URBANO:     return "Urbano";
        case INDUSTRIAL: return "Industrial";
        default:         return "Desconhecido";
    }
}

const char *prioridadeParaString(PrioridadeOcorrencia prioridade) {
    switch (prioridade) {
        case BAIXA:  return "Baixa";
        case NORMAL: return "Normal";
        case ALTA:   return "Alta";
        default:     return "Desconhecida";
    }
}

/* Declarada em ocorrencias.h — implementada aqui como alias de prioridadeParaString */
const char *prioridadeOcorrenciaParaString(PrioridadeOcorrencia prioridade) {
    return prioridadeParaString(prioridade);
}

const char *estadoOcorrenciaParaString(EstadoOcorrencia estado) {
    switch (estado) {
        case REPORTADA:      return "Reportada";
        case EM_INTERVENCAO: return "Em Intervenção";
        case CONCLUIDA:      return "Concluída";
        default:             return "Desconhecido";
    }
}

const char *especialidadeBombeiroParaString(EspecialidadeBombeiro especialidade) {
    switch (especialidade) {
        case COMBATE_FLORESTAL: return "Combate Florestal";
        case RESGATE:           return "Resgate";
        case INCENDIO_URBANO:   return "Incêndio Urbano";  /* corrigido */
        default:                return "Desconhecida";
    }
}

const char *estadoBombeiroParaString(EstadoOperacionalBombeiro estado) {
    switch (estado) {
        case DISPONIVEL:             return "Disponível";
        case EM_INTERVENCAO_BOMBEIRO: return "Em Intervenção";
        case EM_TREINO:              return "Em Treino";
        default:                     return "Desconhecido";
    }
}

const char *tipoEquipamentoParaString(TipoEquipamento tipo) {
    switch (tipo) {
        case VEICULO:    return "Veículo";
        case MANGUEIRA:  return "Mangueira";
        case RESPIRADOR: return "Respirador";
        default:         return "Desconhecido";
    }
}

const char *estadoEquipamentoParaString(EstadoEquipamento estado) {
    switch (estado) {
        case EQ_DISPONIVEL:    return "Disponível";
        case EQ_EM_USO:        return "Em Uso";
        case EQ_EM_MANUTENCAO: return "Em Manutenção";
        default:               return "Desconhecido";
    }
}

const char *estadoIntervencaoParaString(EstadoIntervencao estado) {
    switch (estado) {
        case EM_PLANEAMENTO: return "Em Planeamento";
        case EM_EXECUCAO:    return "Em Execução";
        case INT_CONCLUIDA:  return "Concluída";
        default:             return "Desconhecido";
    }
}