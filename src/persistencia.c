#include <stdio.h>
#include <stdlib.h>
#include "persistencia.h"
#include "logs.h"
#include "utils.h"
#include "modelos.h"

/**
 * @file persistencia.c
 * @brief Implementação das funções de carregamento e gravação de dados.
 *
 * Funcionalidades principais:
 *  - Guardar todas as entidades do sistema em ficheiros binários
 *  - Carregar os dados no inicio
 *  - Persistência individual de:
 *      * Ocorrências
 *      * Intervenções
 *      * Bombeiros
 *      * Equipamentos
 *  - Garantir consistência entre listas dinâmicas e ficheiros
 */


/* CARREGAR DADOS */
void carregarDados(SistemaGestaoIncendios *sistema) {
    registarLog("INFO", "PERSISTENCIA", "CARREGAR_INICIO", "Início do carregamento de dados");

    carregarOcorrencias(sistema);
    carregarBombeiros(sistema);
    carregarEquipamentos(sistema);
    carregarIntervencoes(sistema);

    registarLog("INFO", "PERSISTENCIA", "CARREGAR_FIM", "Carregamento de dados concluído");
}


/*  GUARDAR DADOS           */
void guardarDados(const SistemaGestaoIncendios *sistema) {
    registarLog("INFO", "PERSISTENCIA", "GUARDAR_INICIO", "Início da gravação de dados");

    guardarOcorrencias(sistema);
    guardarBombeiros(sistema);
    guardarEquipamentos(sistema);
    guardarIntervencoes(sistema);

    registarLog("INFO", "PERSISTENCIA", "GUARDAR_FIM", "Gravação de dados concluída");
}


/* OCORRÊNCIAS */
void carregarOcorrencias(SistemaGestaoIncendios *sistema) {
    FILE *f = fopen(FICHEIRO_OCORRENCIAS, "rb");
    if (f == NULL) {
        registarLog("WARN", "PERSISTENCIA", "CARREGAR_OCORRENCIAS",
                    "Ficheiro de ocorrências não encontrado (primeira execução?)");
        return;
    }

    int count = 0;
    if (fread(&count, sizeof(int), 1, f) != 1) {
        fclose(f);
        registarLog("ERROR", "PERSISTENCIA", "CARREGAR_OCORRENCIAS",
                    "Erro ao ler contador de ocorrências");
        return;
    }

    if (count < 0) {
        fclose(f);
        registarLog("ERROR", "PERSISTENCIA", "CARREGAR_OCORRENCIAS",
                    "Contador inválido (negativo)");
        return;
    }

    ListaOcorrencias *lista = &sistema->ocorrencias;

    /* Garantir capacidade suficiente */
    if (count > lista->capacidade) {
        Ocorrencia *novo = realloc(lista->dados, count * sizeof(Ocorrencia));
        if (novo == NULL) {
            fclose(f);
            registarLog("ERROR", "PERSISTENCIA", "CARREGAR_OCORRENCIAS",
                        "Falha ao realocar memória para ocorrências");
            return;
        }
        lista->dados = novo;
        lista->capacidade = count;
    }

    if (count > 0) {
        size_t lidos = fread(lista->dados, sizeof(Ocorrencia), count, f);
        if (lidos != (size_t)count) {
            fclose(f);
            registarLog("ERROR", "PERSISTENCIA", "CARREGAR_OCORRENCIAS",
                        "Erro ao ler registos de ocorrências");
            return;
        }
    }

    lista->tamanho = count;

    /* Atualizar próximo ID */
    int maxId = 0;
    for (int i = 0; i < lista->tamanho; i++) {
        if (lista->dados[i].idOcorrencia > maxId) {
            maxId = lista->dados[i].idOcorrencia;
        }
    }
    sistema->proximoIdOcorrencia = maxId + 1;

    fclose(f);
    registarLog("INFO", "PERSISTENCIA", "CARREGAR_OCORRENCIAS", "Ocorrências carregadas com sucesso");
}


/*  GUARDAR OCORRENCIAS            */
void guardarOcorrencias(const SistemaGestaoIncendios *sistema) {
    FILE *f = fopen(FICHEIRO_OCORRENCIAS, "wb");
    if (f == NULL) {
        registarLog("ERROR", "PERSISTENCIA", "GUARDAR_OCORRENCIAS",
                    "Erro ao abrir ficheiro de ocorrências para escrita");
        return;
    }

    const ListaOcorrencias *lista = &sistema->ocorrencias;
    int count = lista->tamanho;

    /* Escrever contador */
    fwrite(&count, sizeof(int), 1, f);

    /* Escrever array de structs */
    if (count > 0) {
        fwrite(lista->dados, sizeof(Ocorrencia), count, f);
    }

    fclose(f);
    registarLog("INFO", "PERSISTENCIA", "GUARDAR_OCORRENCIAS", "Ocorrências guardadas com sucesso");
}



/*  BOMBEIROS  */
void carregarBombeiros(SistemaGestaoIncendios *sistema) {
    FILE *f = fopen(FICHEIRO_BOMBEIROS, "rb");
    if (f == NULL) {
        registarLog("WARN", "PERSISTENCIA", "CARREGAR_BOMBEIROS",
                    "Ficheiro de bombeiros não encontrado (primeira execução?)");
        return;
    }

    int count = 0;
    if (fread(&count, sizeof(int), 1, f) != 1) {
        fclose(f);
        registarLog("ERROR", "PERSISTENCIA", "CARREGAR_BOMBEIROS",
                    "Erro ao ler contador de bombeiros");
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
                        "Falha ao realocar memória para bombeiros");
            return;
        }
        lista->dados = novo;
        lista->capacidade = count;
    }

    if (count > 0) {
        size_t lidos = fread(lista->dados, sizeof(Bombeiro), count, f);
        if (lidos != (size_t)count) {
            fclose(f);
            registarLog("ERROR", "PERSISTENCIA", "CARREGAR_BOMBEIROS",
                        "Erro ao ler registos de bombeiros");
            return;
        }
    }

    lista->tamanho = count;

    int maxId = 0;
    for (int i = 0; i < lista->tamanho; i++) {
        if (lista->dados[i].idBombeiro > maxId) {
            maxId = lista->dados[i].idBombeiro;
        }
    }
    sistema->proximoIdBombeiro = maxId + 1;

    fclose(f);
    registarLog("INFO", "PERSISTENCIA", "CARREGAR_BOMBEIROS", "Bombeiros carregados com sucesso");
}


/*  GUARDAR BOMBEIROS            */
void guardarBombeiros(const SistemaGestaoIncendios *sistema) {
    FILE *f = fopen(FICHEIRO_BOMBEIROS, "wb");
    if (f == NULL) {
        registarLog("ERROR", "PERSISTENCIA", "GUARDAR_BOMBEIROS",
                    "Erro ao abrir ficheiro de bombeiros para escrita");
        return;
    }

    const ListaBombeiros *lista = &sistema->bombeiros;
    int count = lista->tamanho;

    fwrite(&count, sizeof(int), 1, f);

    if (count > 0) {
        fwrite(lista->dados, sizeof(Bombeiro), count, f);
    }

    fclose(f);
    registarLog("INFO", "PERSISTENCIA", "GUARDAR_BOMBEIROS", "Bombeiros guardados com sucesso");
}

/* EQUIPAMENTOS  */
void carregarEquipamentos(SistemaGestaoIncendios *sistema) {
    FILE *f = fopen(FICHEIRO_EQUIPAMENTOS, "rb");
    if (f == NULL) {
        registarLog("WARN", "PERSISTENCIA", "CARREGAR_EQUIPAMENTOS",
                    "Ficheiro de equipamentos não encontrado (primeira execução?)");
        return;
    }

    int count = 0;
    if (fread(&count, sizeof(int), 1, f) != 1) {
        fclose(f);
        registarLog("ERROR", "PERSISTENCIA", "CARREGAR_EQUIPAMENTOS",
                    "Erro ao ler contador de equipamentos");
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
                        "Falha ao realocar memória para equipamentos");
            return;
        }
        lista->dados = novo;
        lista->capacidade = count;
    }

    if (count > 0) {
        size_t lidos = fread(lista->dados, sizeof(Equipamento), count, f);
        if (lidos != (size_t)count) {
            fclose(f);
            registarLog("ERROR", "PERSISTENCIA", "CARREGAR_EQUIPAMENTOS",
                        "Erro ao ler registos de equipamentos");
            return;
        }
    }

    lista->tamanho = count;

    int maxId = 0;
    for (int i = 0; i < lista->tamanho; i++) {
        if (lista->dados[i].idEquipamento > maxId) {
            maxId = lista->dados[i].idEquipamento;
        }
    }
    sistema->proximoIdEquipamento = maxId + 1;

    fclose(f);
    registarLog("INFO", "PERSISTENCIA", "CARREGAR_EQUIPAMENTOS", "Equipamentos carregados com sucesso");
}


/*  GUARDAR EQUIPAMENTOS            */
void guardarEquipamentos(const SistemaGestaoIncendios *sistema) {
    FILE *f = fopen(FICHEIRO_EQUIPAMENTOS, "wb");
    if (f == NULL) {
        registarLog("ERROR", "PERSISTENCIA", "GUARDAR_EQUIPAMENTOS",
                    "Erro ao abrir ficheiro de equipamentos para escrita");
        return;
    }

    const ListaEquipamentos *lista = &sistema->equipamentos;
    int count = lista->tamanho;

    fwrite(&count, sizeof(int), 1, f);

    if (count > 0) {
        fwrite(lista->dados, sizeof(Equipamento), count, f);
    }

    fclose(f);
    registarLog("INFO", "PERSISTENCIA", "GUARDAR_EQUIPAMENTOS", "Equipamentos guardados com sucesso");
}



/* CARREGAR INTERVENÇÕES  */
void carregarIntervencoes(SistemaGestaoIncendios *sistema) {
    FILE *f = fopen(FICHEIRO_INTERVENCOES, "rb");
    if (f == NULL) {
        registarLog("WARN", "PERSISTENCIA", "CARREGAR_INTERVENCOES",
                    "Ficheiro de intervenções não encontrado (primeira execução?)");
        return;
    }

    int count = 0;
    if (fread(&count, sizeof(int), 1, f) != 1) {
        fclose(f);
        registarLog("ERROR", "PERSISTENCIA", "CARREGAR_INTERVENCOES",
                    "Erro ao ler contador de intervenções");
        return;
    }

    if (count < 0) {
        fclose(f);
        registarLog("ERROR", "PERSISTENCIA", "CARREGAR_INTERVENCOES",
                    "Contador inválido (negativo)");
        return;
    }

    ListaIntervencoes *lista = &sistema->intervencoes;

    /* Realocar lista se necessário */
    if (count > lista->capacidade) {
        Intervencao *novo = realloc(lista->dados, count * sizeof(Intervencao));
        if (novo == NULL) {
            fclose(f);
            registarLog("ERROR", "PERSISTENCIA", "CARREGAR_INTERVENCOES",
                        "Falha ao realocar memória para intervenções");
            return;
        }
        lista->dados = novo;
        lista->capacidade = count;
    }

    lista->tamanho = count;

    int maxId = 0;

    for (int i = 0; i < count; i++) {
        Intervencao *intv = &lista->dados[i];

        /* Inicializar ponteiros para evitar lixo */
        intv->idsBombeiros = NULL;
        intv->idsEquipamentos = NULL;
        intv->capacidadeBombeiros = 0;
        intv->capacidadeEquipamentos = 0;


        fread(&intv->idIntervencao, sizeof(int), 1, f);
        fread(&intv->idOcorrencia,  sizeof(int), 1, f);

        fread(&intv->inicio, sizeof(DataHora), 1, f);
        fread(&intv->fim,    sizeof(DataHora), 1, f);

        fread(&intv->fimDefinido,      sizeof(int), 1, f);
        fread(&intv->estado,           sizeof(EstadoIntervencao), 1, f);
        fread(&intv->ativo,            sizeof(int), 1, f);

        /* BOMBEIROS */
        fread(&intv->numBombeiros, sizeof(int), 1, f);
        if (intv->numBombeiros > 0) {
            intv->capacidadeBombeiros = intv->numBombeiros;
            intv->idsBombeiros = malloc(intv->numBombeiros * sizeof(int));
            if (intv->idsBombeiros == NULL) {
                registarLog("ERROR", "PERSISTENCIA", "CARREGAR_INTERVENCOES",
                            "Falha ao alocar memória para ids Bombeiros");
                fclose(f);
                return;
            }
            fread(intv->idsBombeiros, sizeof(int), intv->numBombeiros, f);
        } else {
            intv->idsBombeiros = NULL;
            intv->capacidadeBombeiros = 0;
        }

        /* EQUIPAMENTOS */
        fread(&intv->numEquipamentos, sizeof(int), 1, f);
        if (intv->numEquipamentos > 0) {
            intv->capacidadeEquipamentos = intv->numEquipamentos;
            intv->idsEquipamentos = malloc(intv->numEquipamentos * sizeof(int));
            if (intv->idsEquipamentos == NULL) {
                registarLog("ERROR", "PERSISTENCIA", "CARREGAR_INTERVENCOES",
                            "Falha ao alocar memória para ids Equipamentos");
                fclose(f);
                return;
            }
            fread(intv->idsEquipamentos, sizeof(int), intv->numEquipamentos, f);
        } else {
            intv->idsEquipamentos = NULL;
            intv->capacidadeEquipamentos = 0;
        }

        if (intv->idIntervencao > maxId) {
            maxId = intv->idIntervencao;
        }
    }

    sistema->proximoIdIntervencao = maxId + 1;

    fclose(f);
    registarLog("INFO", "PERSISTENCIA", "CARREGAR_INTERVENCOES", "Intervenções carregadas com sucesso");
}


/*  GUARDAR INTERVENCOES            */
void guardarIntervencoes(const SistemaGestaoIncendios *sistema) {
    FILE *f = fopen(FICHEIRO_INTERVENCOES, "wb");
    if (f == NULL) {
        registarLog("ERROR", "PERSISTENCIA", "GUARDAR_INTERVENCOES",
                    "Erro ao abrir ficheiro de intervenções para escrita");
        return;
    }

    const ListaIntervencoes *lista = &sistema->intervencoes;
    int count = lista->tamanho;

    /* Escrever contador */
    fwrite(&count, sizeof(int), 1, f);

    for (int i = 0; i < count; i++) {
        const Intervencao *intv = &lista->dados[i];

        fwrite(&intv->idIntervencao, sizeof(int), 1, f);
        fwrite(&intv->idOcorrencia,  sizeof(int), 1, f);

        fwrite(&intv->inicio, sizeof(DataHora), 1, f);
        fwrite(&intv->fim,    sizeof(DataHora), 1, f);

        fwrite(&intv->fimDefinido,      sizeof(int), 1, f);
        fwrite(&intv->estado,           sizeof(EstadoIntervencao), 1, f);
        fwrite(&intv->ativo,            sizeof(int), 1, f);

        /* BOMBEIROS */
        fwrite(&intv->numBombeiros, sizeof(int), 1, f);
        if (intv->numBombeiros > 0 && intv->idsBombeiros != NULL) {
            fwrite(intv->idsBombeiros, sizeof(int), intv->numBombeiros, f);
        }

        /* EQUIPAMENTOS */
        fwrite(&intv->numEquipamentos, sizeof(int), 1, f);
        if (intv->numEquipamentos > 0 && intv->idsEquipamentos != NULL) {
            fwrite(intv->idsEquipamentos, sizeof(int), intv->numEquipamentos, f);
        }
    }

    fclose(f);
    registarLog("INFO", "PERSISTENCIA", "GUARDAR_INTERVENCOES", "Intervenções guardadas com sucesso");
}
