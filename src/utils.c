#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include "logs.h"

/**
 * @file utils.c
 * @brief Funções auxiliares gerais do sistema.
 *
 * Funcionalidades principais:
 *  - Leitura e validação de input
 *  - Manipulação de datas e horas
 *  - Comparações temporais e cálculos de diferenças
 *  - Gestão de listas dinâmicas
 *  - Conversões de enums para strings
 *  - Menus de escolha de tipos e estados
 *  - Validações de integridade (ex.: fim > início)
 */


/* Funções de input e validação */

int getInt(int min, int max, const char *msg) {
    char buffer[100];
    int valor;
    char extra;

    while (1) {
        printf("%s (entre %d e %d): ", msg, min, max);

        /* Ler linha inteira como string */
        if (!fgets(buffer, sizeof(buffer), stdin)) {
            printf("Erro ao ler input.\n");
            continue;
        }

        /* Tentar converter para inteiro e verificar se há lixo no fim */
        if (sscanf(buffer, "%d %c", &valor, &extra) != 1) {
            printf("Erro: Introduza apenas números.\n");
            continue;
        }


        if (valor < min || valor > max) {
            printf("Erro: Valor fora do intervalo permitido.\n");
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
            if (len > 0 && destino[len - 1] == '\n') {
                destino[len - 1] = '\0';
            }
        }

        if (strlen(destino) == 0) {
            printf("Erro: Campo obrigatório. Por favor introduza um valor válido.\n");
        }

    } while (strlen(destino) == 0);
}


int anoBissexto(int ano) {
    return (ano % 4 == 0 && ano % 100 != 0) || (ano % 400 == 0);
}

int validarData(int dia, int mes, int ano) {
    /* Validar ano */
    if (ano < 2000) {
        return 0;
    }

    /* Validar mês */
    if (mes < 1 || mes > 12) {
        return 0;
    }

    /* Validar dia conforme o mês */
    int diasPorMes[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    /* Ajustar fevereiro se ano bissexto */
    if (anoBissexto(ano)) {
        diasPorMes[1] = 29;
    }

    if (dia < 1 || dia > diasPorMes[mes - 1]) {
        return 0;
    }

    return 1;
}

DataHora lerDataHora(const char *msg) {
    DataHora dh;
    int valido = 0;

    printf("%s\n", msg);

    do {
        dh.dia = getInt(1, 31, "Dia");
        dh.mes = getInt(1, 12, "Mês");
        dh.ano = getInt(2000, 2100, "Ano");

        if (!validarData(dh.dia, dh.mes, dh.ano)) {
            printf("Erro: Data inválida. Tente novamente.\n");
            continue;
        }

        dh.hora = getInt(0, 23, "Hora");
        dh.minuto = getInt(0, 59, "Minuto");

        valido = 1;

    } while (!valido);

    return dh;
}

/* Funções de inicialização de listas  */

void inicializarListaOcorrencias(ListaOcorrencias *lista) {
    lista->dados = malloc(CAPACIDADE_INICIAL * sizeof(Ocorrencia));
    if (lista->dados == NULL) {
        registarLog("ERROR", "UTILS", "INIT_OCORRENCIAS", "Falha ao alocar memória");
        fprintf(stderr, "ERRO FATAL: Não foi possível alocar memória para ocorrências.\n");
        exit(EXIT_FAILURE);
    }
    lista->tamanho = 0;
    lista->capacidade = CAPACIDADE_INICIAL;
    //registarLog("INFO", "UTILS", "INIT_OCORRENCIAS", "Lista inicializada com sucesso");
    // Log comentado para evitar excesso de logs
}

void inicializarListaIntervencoes(ListaIntervencoes *lista) {
    lista->dados = malloc(CAPACIDADE_INICIAL * sizeof(Intervencao));
    if (lista->dados == NULL) {
        registarLog("ERROR", "UTILS", "INIT_INTERVENCOES", "Falha ao alocar memória");
        fprintf(stderr, "ERRO FATAL: Não foi possível alocar memória para intervenções.\n");
        exit(EXIT_FAILURE);
    }
    lista->tamanho = 0;
    lista->capacidade = CAPACIDADE_INICIAL;
    //registarLog("INFO", "UTILS", "INIT_INTERVENCOES", "Lista inicializada com sucesso");
    // Log comentado para evitar excesso de logs
}

void inicializarListaBombeiros(ListaBombeiros *lista) {
    lista->dados = malloc(CAPACIDADE_INICIAL * sizeof(Bombeiro));
    if (lista->dados == NULL) {
        registarLog("ERROR", "UTILS", "INIT_BOMBEIROS", "Falha ao alocar memória");
        fprintf(stderr, "ERRO FATAL: Não foi possível alocar memória para bombeiros.\n");
        exit(EXIT_FAILURE);
    }
    lista->tamanho = 0;
    lista->capacidade = CAPACIDADE_INICIAL;
    //registarLog("INFO", "UTILS", "INIT_BOMBEIROS", "Lista inicializada com sucesso");
    // Log comentado para evitar excesso de logs
}

void inicializarListaEquipamentos(ListaEquipamentos *lista) {
    lista->dados = malloc(CAPACIDADE_INICIAL * sizeof(Equipamento));
    if (lista->dados == NULL) {
        registarLog("ERROR", "UTILS", "INIT_EQUIPAMENTOS", "Falha ao alocar memória");
        fprintf(stderr, "ERRO FATAL: Não foi possível alocar memória para equipamentos.\n");
        exit(EXIT_FAILURE);
    }
    lista->tamanho = 0;
    lista->capacidade = CAPACIDADE_INICIAL;
    //registarLog("INFO", "UTILS", "INIT_EQUIPAMENTOS", "Lista inicializada com sucesso");
    // Log comentado para evitar excesso de logs
}

/* Funções de libertação de memória */

void libertarListaOcorrencias(ListaOcorrencias *lista) {
    if (lista->dados != NULL) {
        free(lista->dados);
        lista->dados = NULL;
    }
    lista->tamanho = 0;
    lista->capacidade = 0;
    registarLog("INFO", "UTILS", "FREE_OCORRENCIAS", "Memória libertada");
}

void libertarListaIntervencoes(ListaIntervencoes *lista) {
    if (lista->dados != NULL) {
        /* IMPORTANTE - libertar arrays internos de cada intervenção primeiro */
        for (int i = 0; i < lista->tamanho; i++) {
            if (lista->dados[i].idsBombeiros != NULL) {
                free(lista->dados[i].idsBombeiros);
                lista->dados[i].idsBombeiros = NULL;
            }
            if (lista->dados[i].idsEquipamentos != NULL) {
                free(lista->dados[i].idsEquipamentos);
                lista->dados[i].idsEquipamentos = NULL;
            }
        }
        free(lista->dados);
        lista->dados = NULL;
    }
    lista->tamanho = 0;
    lista->capacidade = 0;
    registarLog("INFO", "UTILS", "FREE_INTERVENCOES", "Memória libertada (incluindo arrays internos)");
}

void libertarListaBombeiros(ListaBombeiros *lista) {
    if (lista->dados != NULL) {
        free(lista->dados);
        lista->dados = NULL;
    }
    lista->tamanho = 0;
    lista->capacidade = 0;
    registarLog("INFO", "UTILS", "FREE_BOMBEIROS", "Memória libertada");
}

void libertarListaEquipamentos(ListaEquipamentos *lista) {
    if (lista->dados != NULL) {
        free(lista->dados);
        lista->dados = NULL;
    }
    lista->tamanho = 0;
    lista->capacidade = 0;
    registarLog("INFO", "UTILS", "FREE_EQUIPAMENTOS", "Memória libertada");
}

int expandirListaOcorrenciasSeNecessario(ListaOcorrencias *lista) {
    /* Verificar se precisa expandir */
    if (lista->tamanho < lista->capacidade) {
        return 1; /* Ainda há espaço */
    }

    /* Calcular nova capacidade */
    int novaCapacidade = lista->capacidade * 2;

    /* Tentar realocar memória */
    Ocorrencia *novoArray = realloc(lista->dados, novaCapacidade * sizeof(Ocorrencia));

    if (novoArray == NULL) {
        registarLog("ERROR", "UTILS", "EXPANDIR_LISTA", "Falha ao expandir lista de ocorrências");
        return 0;
    }

    lista->dados = novoArray;
    lista->capacidade = novaCapacidade;

    registarLog("INFO", "UTILS", "EXPANDIR_LISTA", "Lista expandida com sucesso");
    return 1;
}

TipoOcorrencia escolherTipoOcorrencia(void) {
    printf("\n    TIPO DE OCORRÊNCIA    \n");
    printf("1. Florestal\n");
    printf("2. Urbano\n");
    printf("3. Industrial\n");

    int escolha = getInt(1, 3, "Escolha o tipo");

    switch (escolha) {
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

    int escolha = getInt(1, 3, "Escolha a prioridade");

    switch (escolha) {
        case 1: return BAIXA;
        case 2: return NORMAL;
        case 3: return ALTA;
        default: return NORMAL;
    }
}

EspecialidadeBombeiro escolherEspecialidadeBombeiro(void) {
        printf("\n     ESPECIALIDADE DO BOMBEIRO    \n");
        printf("1. Combate florestal\n");
        printf("2. Resgate\n");
        printf("3. Incêndio\n");

        int escolha = getInt(1, 3, "Escolha a especialidade");

        switch (escolha) {
            case 1: return COMBATE_FLORESTAL;
            case 2: return RESGATE;
            case 3: return INCENDIO;
            default: return INCENDIO;
        }
}

EstadoOperacionalBombeiro escolherEstadoBombeiro(void) {
        printf("\n    ESTADO    \n");
        printf("1. Disponível\n");
        printf("2. Em intervenção\n");
        printf("3. Em treino\n");

        int escolha = getInt(1, 3, "Defina o seu estado: ");

        switch (escolha) {
            case 1: return DISPONIVEL;
            case 2: return EM_INTERVENCAO_BOMBEIRO;
            case 3: return EM_TREINO;
            default: return DISPONIVEL;
        }
}

/* Expandir Lista de Bombeiros */

int expandirListaBombeirosSeNecessario(ListaBombeiros *lista) {
    if (lista->tamanho < lista->capacidade) {
        return 1;
    }

    int novaCapacidade = lista->capacidade * 2;
    Bombeiro *novoArray = realloc(lista->dados, novaCapacidade * sizeof(Bombeiro));

    if (novoArray == NULL) {
        registarLog("ERROR", "UTILS", "EXPANDIR_LISTA_BOMBEIROS", "Falha ao expandir");
        return 0;
    }

    lista->dados = novoArray;
    lista->capacidade = novaCapacidade;

    registarLog("INFO", "UTILS", "EXPANDIR_LISTA_BOMBEIROS", "Lista expandida");
    return 1;
}


const char* especialidadeBombeiroParaString(EspecialidadeBombeiro especialidade) {
    switch (especialidade) {
        case COMBATE_FLORESTAL:     return "Combate Florestal";
        case RESGATE:               return "Resgate";
        case INCENDIO:              return "Incêndio";
        default:                    return "Desconhecida";
    }
}

const char* estadoBombeiroParaString(EstadoOperacionalBombeiro estado) {
    switch (estado) {
        case DISPONIVEL:                 return "Disponível";
        case EM_INTERVENCAO_BOMBEIRO:    return "Em Intervenção";
        case EM_TREINO:                  return "Em Treino";
        default:                         return "Desconhecido";
    }
}

/* Expandir Lista de Equipamentos */

int expandirListaEquipamentosSeNecessario(ListaEquipamentos *lista) {
    if (lista->tamanho < lista->capacidade) {
        return 1;
    }

    int novaCapacidade = lista->capacidade * 2;
    Equipamento *novoArray = realloc(lista->dados, novaCapacidade * sizeof(Equipamento));

    if (novoArray == NULL) {
        registarLog("ERROR", "UTILS", "EXPANDIR_LISTA_EQUIPAMENTOS", "Falha ao expandir");
        return 0;
    }

    lista->dados = novoArray;
    lista->capacidade = novaCapacidade;

    registarLog("INFO", "UTILS", "EXPANDIR_LISTA_EQUIPAMENTOS", "Lista expandida");
    return 1;
}

/* Escolher Tipo de Equipamento */

TipoEquipamento escolherTipoEquipamento(void) {
    printf("\n    TIPO DE EQUIPAMENTO    \n");
    printf("1. Veículo\n");
    printf("2. Mangueira\n");
    printf("3. Respirador\n");

    int escolha = getInt(1, 3, "Escolha o tipo");

    switch (escolha) {
        case 1: return VEICULO;
        case 2: return MANGUEIRA;
        case 3: return RESPIRADOR;
        default: return VEICULO;
    }
}


const char* tipoEquipamentoParaString(TipoEquipamento tipo) {
    switch (tipo) {
        case VEICULO:            return "Veículo";
        case MANGUEIRA:          return "Mangueira";
        case RESPIRADOR:         return "Respirador";
        default:                 return "Desconhecido";
    }
}

const char* estadoEquipamentoParaString(EstadoEquipamento estado) {
    switch (estado) {
        case EQ_DISPONIVEL:     return "Disponível";
        case EQ_EM_USO:         return "Em Uso";
        case EQ_EM_MANUTENCAO:  return "Em Manutenção";
        default:                return "Desconhecido";
    }
}

EstadoEquipamento escolherEstadoEquipamento(void) {
    printf("\n    ESTADO DO EQUIPAMENTO      \n");
    printf("1. Disponível\n");
    printf("2. Em uso\n");
    printf("3. Em manutenção\n");

    int escolha = getInt(1, 3, "Escolha o estado");

    switch (escolha) {
        case 1: return EQ_DISPONIVEL;
        case 2: return EQ_EM_USO;
        case 3: return EQ_EM_MANUTENCAO;
        default: return EQ_DISPONIVEL;
    }
}

int expandirArrayBombeirosIntervencao(Intervencao *intervencao) {
    if (intervencao->numBombeiros < intervencao->capacidadeBombeiros) {
        return 1;
    }

    int novaCapacidade = intervencao->capacidadeBombeiros * 2;
    int *novoArray = realloc(intervencao->idsBombeiros, novaCapacidade * sizeof(int));

    if (novoArray == NULL) {
        registarLog("ERROR", "UTILS", "EXPANDIR_ARRAY_BOMBEIROS", "Falha ao expandir");
        return 0;
    }

    intervencao->idsBombeiros = novoArray;
    intervencao->capacidadeBombeiros = novaCapacidade;
    return 1;
}

int expandirArrayEquipamentosIntervencao(Intervencao *intervencao) {
    if (intervencao->numEquipamentos < intervencao->capacidadeEquipamentos) {
        return 1;
    }

    int novaCapacidade = intervencao->capacidadeEquipamentos * 2;
    int *novoArray = realloc(intervencao->idsEquipamentos, novaCapacidade * sizeof(int));

    if (novoArray == NULL) {
        registarLog("ERROR", "UTILS", "EXPANDIR_ARRAY_EQUIPAMENTOS", "Falha ao expandir");
        return 0;
    }

    intervencao->idsEquipamentos = novoArray;
    intervencao->capacidadeEquipamentos = novaCapacidade;
    return 1;
}

int expandirListaIntervencoesSeNecessario(ListaIntervencoes *lista) {
    if (lista->tamanho < lista->capacidade) {
        return 1;
    }

    int novaCapacidade = lista->capacidade * 2;
    Intervencao *novoArray = realloc(lista->dados, novaCapacidade * sizeof(Intervencao));

    if (novoArray == NULL) {
        registarLog("ERROR", "UTILS", "EXPANDIR_LISTA_INTERVENCOES", "Falha ao expandir");
        return 0;
    }

    lista->dados = novoArray;
    lista->capacidade = novaCapacidade;

    registarLog("INFO", "UTILS", "EXPANDIR_LISTA_INTERVENCOES", "Lista expandida");
    return 1;
}

const char* estadoIntervencaoParaString(EstadoIntervencao estado) {
    switch (estado) {
        case EM_PLANEAMENTO: return "Em Planeamento";
        case EM_EXECUCAO:    return "Em Execução";
        case INT_CONCLUIDA:  return "Concluída";
        default:             return "Desconhecido";
    }
}

const char* estadoOcorrenciaParaString(EstadoOcorrencia estado) {
    switch (estado) {
        case REPORTADA: return "Reportada";
        case EM_INTERVENCAO: return "Em Intervenção";
        case CONCLUIDA: return "Concluída";
        default: return "Desconhecido";
    }
}

int validarDataHoraFimMaiorQueInicio(const DataHora *inicio, const DataHora *fim) {
    /* Comparar anos */
    if (fim->ano > inicio->ano) return 1;
    if (fim->ano < inicio->ano) return 0;

    /* Mesmo ano, comparar meses */
    if (fim->mes > inicio->mes) return 1;
    if (fim->mes < inicio->mes) return 0;

    /* Mesmo mês, comparar dias */
    if (fim->dia > inicio->dia) return 1;
    if (fim->dia < inicio->dia) return 0;

    /* Mesmo dia, comparar horas */
    if (fim->hora > inicio->hora) return 1;
    if (fim->hora < inicio->hora) return 0;

    /* Mesma hora, comparar minutos */
    if (fim->minuto > inicio->minuto) return 1;

    return 0; /* fim <= inicio */
}


static long converterParaMinutos(DataHora d) {
    int diasPorMes[] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
    long dias = 0;

    dias = d.ano * 365;

    /* Adicionar meses completos */
    for (int i = 1; i < d.mes; i++) {
        dias += diasPorMes[i];
    }

    /* Ajustar anos bissextos */
    int anos = d.ano - 1;
    dias += (anos / 4) - (anos / 100) + (anos / 400);

    /* Ajustar fevereiro se ano for bissexto */
    int ano = d.ano;
    int bissexto = (ano % 4 == 0 && ano % 100 != 0) || (ano % 400 == 0);

    if (bissexto && d.mes > 2) {
        dias += 1;
    }

    /* Adicionar dias restantes */
    dias += d.dia;

    /* Converter para minutos */
    long minutos = dias * 1440 + d.hora * 60 + d.minuto;

    return minutos;
}

int minutosEntre(DataHora d1, DataHora d2) {
    long m1 = converterParaMinutos(d1);
    long m2 = converterParaMinutos(d2);
    return (int)(m2 - m1);
}


const char* tipoOcorrenciaParaString(TipoOcorrencia tipo) {
    switch (tipo) {
        case FLORESTAL: return "Florestal";
        case URBANO: return "Urbano";
        case INDUSTRIAL: return "Industrial";
        default: return "Desconhecido";
    }
}

const char* prioridadeParaString(PrioridadeOcorrencia prioridade) {
    switch (prioridade) {
        case BAIXA: return "Baixa";
        case NORMAL: return "Normal";
        case ALTA: return "Alta";
        default: return "Desconhecida";
    }
}
