#include "persistencia.h"
#include "logs.h"
#include "utils.h"
#include "modelos.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * @file persistencia.c
 * @brief Implementação das funções de carregamento e gravação de dados.
 *
 * Funcionalidades:
 *  - Guardar todas as entidades do sistema em ficheiros binários
 *  - Carregar os dados no início da sessão
 *  - Persistência individual de: Ocorrências, Bombeiros, Equipamentos, Intervenções
 *  - Garantir consistência entre listas dinâmicas e ficheiros
 *
 * Nota: carregarIntervencoes() deve ser chamada APÓS carregar bombeiros e
 * equipamentos, pois aloca arrays internos que dependem desses dados.
 */


/* ========================================================================= */
/*  CARREGAR / GUARDAR TUDO                                                  */
/* ========================================================================= */

void carregarDados(SistemaGestaoIncendios *sistema) {
    registarLog("INFO", "PERSISTENCIA", "CARREGAR_INICIO", "Início do carregamento de dados");

    carregarOcorrencias(sistema);
    carregarBombeiros(sistema);
    carregarEquipamentos(sistema);
    carregarIntervencoes(sistema);   /* sempre por último */

    registarLog("INFO", "PERSISTENCIA", "CARREGAR_FIM", "Carregamento de dados concluído");
}

void guardarDados(const SistemaGestaoIncendios *sistema) {
    registarLog("INFO", "PERSISTENCIA", "GUARDAR_INICIO", "Início da gravação de dados");

    guardarOcorrencias(sistema);
    guardarBombeiros(sistema);
    guardarEquipamentos(sistema);
    guardarIntervencoes(sistema);

    registarLog("INFO", "PERSISTENCIA", "GUARDAR_FIM", "Gravação de dados concluída");
}


/* ========================================================================= */
/*  OCORRÊNCIAS                                                              */
/* ========================================================================= */

void carregarOcorrencias(SistemaGestaoIncendios *sistema) {
    FILE *f = fopen(FICHEIRO_OCORRENCIAS, "rb");
    if (f == NULL) {
        registarLog("WARNING", "PERSISTENCIA", "CARREGAR_OCORRENCIAS",
                    "Ficheiro não encontrado (primeira execução?)");
        return;
    }

    int count = 0;
    if (fread(&count, sizeof(int), 1, f) != 1) {
        fclose(f);
        registarLog("ERROR", "PERSISTENCIA", "CARREGAR_OCORRENCIAS",
                    "Erro ao ler contador");
        return;
    }
    if (count < 0) {
        fclose(f);
        registarLog("ERROR", "PERSISTENCIA", "CARREGAR_OCORRENCIAS",
                    "Contador inválido (negativo)");
        return;
    }

    ListaOcorrencias *lista = &sistema->ocorrencias;

    if (count > lista->capacidade) {
        Ocorrencia *novo = realloc(lista->dados, count * sizeof(Ocorrencia));
        if (novo == NULL) {
            fclose(f);
            registarLog("ERROR", "PERSISTENCIA", "CARREGAR_OCORRENCIAS",
                        "Falha ao realocar memória");
            return;
        }
        lista->dados     = novo;
        lista->capacidade = count;
    }

    if (count > 0) {
        if (fread(lista->dados, sizeof(Ocorrencia), count, f) != (size_t)count) {
            fclose(f);
            registarLog("ERROR", "PERSISTENCIA", "CARREGAR_OCORRENCIAS",
                        "Erro ao ler registos");
            return;
        }
    }

    lista->tamanho = count;

    /* Recalcular próximo ID */
    int maxId = 0;
    for (int i = 0; i < lista->tamanho; i++) {
        if (lista->dados[i].idOcorrencia > maxId)
            maxId = lista->dados[i].idOcorrencia;
    }
    sistema->proximoIdOcorrencia = maxId + 1;

    fclose(f);
    registarLog("INFO", "PERSISTENCIA", "CARREGAR_OCORRENCIAS",
                "Ocorrências carregadas com sucesso");
}

void guardarOcorrencias(const SistemaGestaoIncendios *sistema) {
    FILE *f = fopen(FICHEIRO_OCORRENCIAS, "wb");
    if (f == NULL) {
        registarLog("ERROR", "PERSISTENCIA", "GUARDAR_OCORRENCIAS",
                    "Erro ao abrir ficheiro para escrita");
        return;
    }

    const ListaOcorrencias *lista = &sistema->ocorrencias;
    int count = lista->tamanho;
    int erroEscrita = 0;

    if (fwrite(&count, sizeof(int), 1, f) != 1) erroEscrita = 1;
    if (!erroEscrita && count > 0)
        if (fwrite(lista->dados, sizeof(Ocorrencia), count, f) != (size_t)count)
            erroEscrita = 1;

    fclose(f);
    if (erroEscrita)
        registarLog("ERROR", "PERSISTENCIA", "GUARDAR_OCORRENCIAS",
                    "Erro de escrita (disco cheio?)");
    else
        registarLog("INFO", "PERSISTENCIA", "GUARDAR_OCORRENCIAS",
                    "Ocorrências guardadas com sucesso");
}


/* ========================================================================= */
/*  BOMBEIROS                                                                */
/* ========================================================================= */

void carregarBombeiros(SistemaGestaoIncendios *sistema) {
    FILE *f = fopen(FICHEIRO_BOMBEIROS, "rb");
    if (f == NULL) {
        registarLog("WARNING", "PERSISTENCIA", "CARREGAR_BOMBEIROS",
                    "Ficheiro não encontrado (primeira execução?)");
        return;
    }

    int count = 0;
    if (fread(&count, sizeof(int), 1, f) != 1) {
        fclose(f);
        registarLog("ERROR", "PERSISTENCIA", "CARREGAR_BOMBEIROS",
                    "Erro ao ler contador");
        return;
    }
    if (count < 0) {
        fclose(f);
        registarLog("ERROR", "PERSISTENCIA", "CARREGAR_BOMBEIROS",
                    "Contador inválido (negativo)");
        return;
    }

    ListaBombeiros *lista = &sistema->bombeiros;

    if (count > lista->capacidade) {
        Bombeiro *novo = realloc(lista->dados, count * sizeof(Bombeiro));
        if (novo == NULL) {
            fclose(f);
            registarLog("ERROR", "PERSISTENCIA", "CARREGAR_BOMBEIROS",
                        "Falha ao realocar memória");
            return;
        }
        lista->dados      = novo;
        lista->capacidade = count;
    }

    if (count > 0) {
        if (fread(lista->dados, sizeof(Bombeiro), count, f) != (size_t)count) {
            fclose(f);
            registarLog("ERROR", "PERSISTENCIA", "CARREGAR_BOMBEIROS",
                        "Erro ao ler registos");
            return;
        }
    }

    lista->tamanho = count;

    int maxId = 0;
    for (int i = 0; i < lista->tamanho; i++) {
        if (lista->dados[i].idBombeiro > maxId)
            maxId = lista->dados[i].idBombeiro;
    }
    sistema->proximoIdBombeiro = maxId + 1;

    fclose(f);
    registarLog("INFO", "PERSISTENCIA", "CARREGAR_BOMBEIROS",
                "Bombeiros carregados com sucesso");
}

void guardarBombeiros(const SistemaGestaoIncendios *sistema) {
    FILE *f = fopen(FICHEIRO_BOMBEIROS, "wb");
    if (f == NULL) {
        registarLog("ERROR", "PERSISTENCIA", "GUARDAR_BOMBEIROS",
                    "Erro ao abrir ficheiro para escrita");
        return;
    }

    const ListaBombeiros *lista = &sistema->bombeiros;
    int count = lista->tamanho;
    int erroEscrita = 0;

    if (fwrite(&count, sizeof(int), 1, f) != 1) erroEscrita = 1;
    if (!erroEscrita && count > 0)
        if (fwrite(lista->dados, sizeof(Bombeiro), count, f) != (size_t)count)
            erroEscrita = 1;

    fclose(f);
    if (erroEscrita)
        registarLog("ERROR", "PERSISTENCIA", "GUARDAR_BOMBEIROS",
                    "Erro de escrita (disco cheio?)");
    else
        registarLog("INFO", "PERSISTENCIA", "GUARDAR_BOMBEIROS",
                    "Bombeiros guardados com sucesso");
}


/* ========================================================================= */
/*  EQUIPAMENTOS                                                             */
/* ========================================================================= */

void carregarEquipamentos(SistemaGestaoIncendios *sistema) {
    FILE *f = fopen(FICHEIRO_EQUIPAMENTOS, "rb");
    if (f == NULL) {
        registarLog("WARNING", "PERSISTENCIA", "CARREGAR_EQUIPAMENTOS",
                    "Ficheiro não encontrado (primeira execução?)");
        return;
    }

    int count = 0;
    if (fread(&count, sizeof(int), 1, f) != 1) {
        fclose(f);
        registarLog("ERROR", "PERSISTENCIA", "CARREGAR_EQUIPAMENTOS",
                    "Erro ao ler contador");
        return;
    }
    if (count < 0) {
        fclose(f);
        registarLog("ERROR", "PERSISTENCIA", "CARREGAR_EQUIPAMENTOS",
                    "Contador inválido (negativo)");
        return;
    }

    ListaEquipamentos *lista = &sistema->equipamentos;

    if (count > lista->capacidade) {
        Equipamento *novo = realloc(lista->dados, count * sizeof(Equipamento));
        if (novo == NULL) {
            fclose(f);
            registarLog("ERROR", "PERSISTENCIA", "CARREGAR_EQUIPAMENTOS",
                        "Falha ao realocar memória");
            return;
        }
        lista->dados      = novo;
        lista->capacidade = count;
    }

    if (count > 0) {
        if (fread(lista->dados, sizeof(Equipamento), count, f) != (size_t)count) {
            fclose(f);
            registarLog("ERROR", "PERSISTENCIA", "CARREGAR_EQUIPAMENTOS",
                        "Erro ao ler registos");
            return;
        }
    }

    lista->tamanho = count;

    int maxId = 0;
    for (int i = 0; i < lista->tamanho; i++) {
        if (lista->dados[i].idEquipamento > maxId)
            maxId = lista->dados[i].idEquipamento;
    }
    sistema->proximoIdEquipamento = maxId + 1;

    fclose(f);
    registarLog("INFO", "PERSISTENCIA", "CARREGAR_EQUIPAMENTOS",
                "Equipamentos carregados com sucesso");
}

void guardarEquipamentos(const SistemaGestaoIncendios *sistema) {
    FILE *f = fopen(FICHEIRO_EQUIPAMENTOS, "wb");
    if (f == NULL) {
        registarLog("ERROR", "PERSISTENCIA", "GUARDAR_EQUIPAMENTOS",
                    "Erro ao abrir ficheiro para escrita");
        return;
    }

    const ListaEquipamentos *lista = &sistema->equipamentos;
    int count = lista->tamanho;
    int erroEscrita = 0;

    if (fwrite(&count, sizeof(int), 1, f) != 1) erroEscrita = 1;
    if (!erroEscrita && count > 0)
        if (fwrite(lista->dados, sizeof(Equipamento), count, f) != (size_t)count)
            erroEscrita = 1;

    fclose(f);
    if (erroEscrita)
        registarLog("ERROR", "PERSISTENCIA", "GUARDAR_EQUIPAMENTOS",
                    "Erro de escrita (disco cheio?)");
    else
        registarLog("INFO", "PERSISTENCIA", "GUARDAR_EQUIPAMENTOS",
                    "Equipamentos guardados com sucesso");
}


/* ========================================================================= */
/*  INTERVENÇÕES                                                             */
/* ========================================================================= */

void carregarIntervencoes(SistemaGestaoIncendios *sistema) {
    FILE *f = fopen(FICHEIRO_INTERVENCOES, "rb");
    if (f == NULL) {
        registarLog("WARNING", "PERSISTENCIA", "CARREGAR_INTERVENCOES",
                    "Ficheiro não encontrado (primeira execução?)");
        return;
    }

    int count = 0;
    if (fread(&count, sizeof(int), 1, f) != 1) {
        fclose(f);
        registarLog("ERROR", "PERSISTENCIA", "CARREGAR_INTERVENCOES",
                    "Erro ao ler contador");
        return;
    }
    if (count < 0) {
        fclose(f);
        registarLog("ERROR", "PERSISTENCIA", "CARREGAR_INTERVENCOES",
                    "Contador inválido (negativo)");
        return;
    }

    ListaIntervencoes *lista = &sistema->intervencoes;

    if (count > lista->capacidade) {
        Intervencao *novo = realloc(lista->dados, count * sizeof(Intervencao));
        if (novo == NULL) {
            fclose(f);
            registarLog("ERROR", "PERSISTENCIA", "CARREGAR_INTERVENCOES",
                        "Falha ao realocar memória");
            return;
        }
        lista->dados      = novo;
        lista->capacidade = count;
    }

    lista->tamanho = count;

    /*
     * Inicializar TODOS os ponteiros dinâmicos a NULL antes do loop.
     * Se a leitura falhar a meio, libertarListaIntervencoes() iterará até
     * lista->tamanho (= count) e chamará free() sobre entradas não lidas.
     * Sem esta inicialização prévia, essas entradas conteriam lixo de memória
     * (realloc não zera), resultando em free() com ponteiro inválido (UB).
     */
    for (int j = 0; j < count; j++) {
        lista->dados[j].idsBombeiros         = NULL;
        lista->dados[j].idsEquipamentos      = NULL;
        lista->dados[j].capacidadeBombeiros   = 0;
        lista->dados[j].capacidadeEquipamentos = 0;
    }

    int maxId = 0;

    for (int i = 0; i < count; i++) {
        Intervencao *intv = &lista->dados[i];

        if (fread(&intv->idIntervencao, sizeof(int),      1, f) != 1 ||
            fread(&intv->idOcorrencia,  sizeof(int),      1, f) != 1 ||
            fread(&intv->inicio,        sizeof(DataHora), 1, f) != 1 ||
            fread(&intv->fim,           sizeof(DataHora), 1, f) != 1 ||
            fread(&intv->fimDefinido,   sizeof(int),      1, f) != 1 ||
            fread(&intv->estado,        sizeof(EstadoIntervencao), 1, f) != 1 ||
            fread(&intv->ativo,         sizeof(int),      1, f) != 1)
        {
            fclose(f);
            registarLog("ERROR", "PERSISTENCIA", "CARREGAR_INTERVENCOES",
                        "Erro ao ler campos fixos de intervenção");
            return;
        }

        /* Bombeiros */
        if (fread(&intv->numBombeiros, sizeof(int), 1, f) != 1) {
            fclose(f);
            registarLog("ERROR", "PERSISTENCIA", "CARREGAR_INTERVENCOES",
                        "Erro ao ler numBombeiros");
            return;
        }
        if (intv->numBombeiros > 0) {
            intv->idsBombeiros = malloc(intv->numBombeiros * sizeof(int));
            if (intv->idsBombeiros == NULL) {
                fclose(f);
                registarLog("ERROR", "PERSISTENCIA", "CARREGAR_INTERVENCOES",
                            "Falha ao alocar memória para idsBombeiros");
                return;
            }
            intv->capacidadeBombeiros = intv->numBombeiros;
            if (fread(intv->idsBombeiros, sizeof(int), intv->numBombeiros, f)
                    != (size_t)intv->numBombeiros) {
                fclose(f);
                registarLog("ERROR", "PERSISTENCIA", "CARREGAR_INTERVENCOES",
                            "Erro ao ler idsBombeiros");
                return;
            }
        }

        /* Equipamentos */
        if (fread(&intv->numEquipamentos, sizeof(int), 1, f) != 1) {
            fclose(f);
            registarLog("ERROR", "PERSISTENCIA", "CARREGAR_INTERVENCOES",
                        "Erro ao ler numEquipamentos");
            return;
        }
        if (intv->numEquipamentos > 0) {
            intv->idsEquipamentos = malloc(intv->numEquipamentos * sizeof(int));
            if (intv->idsEquipamentos == NULL) {
                fclose(f);
                registarLog("ERROR", "PERSISTENCIA", "CARREGAR_INTERVENCOES",
                            "Falha ao alocar memória para idsEquipamentos");
                return;
            }
            intv->capacidadeEquipamentos = intv->numEquipamentos;
            if (fread(intv->idsEquipamentos, sizeof(int), intv->numEquipamentos, f)
                    != (size_t)intv->numEquipamentos) {
                fclose(f);
                registarLog("ERROR", "PERSISTENCIA", "CARREGAR_INTERVENCOES",
                            "Erro ao ler idsEquipamentos");
                return;
            }
        }

        if (intv->idIntervencao > maxId)
            maxId = intv->idIntervencao;
    }

    sistema->proximoIdIntervencao = maxId + 1;

    fclose(f);
    registarLog("INFO", "PERSISTENCIA", "CARREGAR_INTERVENCOES",
                "Intervenções carregadas com sucesso");
}

void guardarIntervencoes(const SistemaGestaoIncendios *sistema) {
    FILE *f = fopen(FICHEIRO_INTERVENCOES, "wb");
    if (f == NULL) {
        registarLog("ERROR", "PERSISTENCIA", "GUARDAR_INTERVENCOES",
                    "Erro ao abrir ficheiro para escrita");
        return;
    }

    const ListaIntervencoes *lista = &sistema->intervencoes;
    int count = lista->tamanho;
    int erroEscrita = 0;

    if (fwrite(&count, sizeof(int), 1, f) != 1) erroEscrita = 1;

    for (int i = 0; i < count && !erroEscrita; i++) {
        const Intervencao *intv = &lista->dados[i];

        if (fwrite(&intv->idIntervencao, sizeof(int),             1, f) != 1 ||
            fwrite(&intv->idOcorrencia,  sizeof(int),             1, f) != 1 ||
            fwrite(&intv->inicio,        sizeof(DataHora),        1, f) != 1 ||
            fwrite(&intv->fim,           sizeof(DataHora),        1, f) != 1 ||
            fwrite(&intv->fimDefinido,   sizeof(int),             1, f) != 1 ||
            fwrite(&intv->estado,        sizeof(EstadoIntervencao), 1, f) != 1 ||
            fwrite(&intv->ativo,         sizeof(int),             1, f) != 1)
        {
            erroEscrita = 1;
            break;
        }

        if (fwrite(&intv->numBombeiros, sizeof(int), 1, f) != 1) {
            erroEscrita = 1; break;
        }
        if (intv->numBombeiros > 0 && intv->idsBombeiros != NULL)
            if (fwrite(intv->idsBombeiros, sizeof(int), intv->numBombeiros, f)
                    != (size_t)intv->numBombeiros) {
                erroEscrita = 1; break;
            }

        if (fwrite(&intv->numEquipamentos, sizeof(int), 1, f) != 1) {
            erroEscrita = 1; break;
        }
        if (intv->numEquipamentos > 0 && intv->idsEquipamentos != NULL)
            if (fwrite(intv->idsEquipamentos, sizeof(int), intv->numEquipamentos, f)
                    != (size_t)intv->numEquipamentos) {
                erroEscrita = 1; break;
            }
    }

    fclose(f);
    if (erroEscrita)
        registarLog("ERROR", "PERSISTENCIA", "GUARDAR_INTERVENCOES",
                    "Erro de escrita (disco cheio?)");
    else
        registarLog("INFO", "PERSISTENCIA", "GUARDAR_INTERVENCOES",
                    "Intervenções guardadas com sucesso");
}