#ifndef BOMBEIROS_H
#define BOMBEIROS_H

/**
 * @file bombeiros.h
 * @brief Gestão da entidade Bombeiro no Sistema de Gestão de Incêndios.
 *
 * Este módulo é responsável por todas as operações CRUD (Create, Read, Update, Delete)
 * relacionadas com bombeiros, incluindo gestão de estados operacionais e especialidades.
 *
 * @author José (8220942)
 * @date 2024
 * @version 2.0
 */

#include "tipos_comuns.h"
#include <stddef.h>


/* ========================================================================= */
/*  ENUMERAÇÕES ESPECÍFICAS DE BOMBEIROS                                    */
/* ========================================================================= */

/**
 * @brief Especialidades operacionais dos bombeiros.
 *
 * Define as áreas de atuação especializada de cada bombeiro,
 * permitindo alocação eficiente segundo o tipo de ocorrência.
 */
typedef enum {
    COMBATE_FLORESTAL,    /**< Especializado em incêndios florestais          */
    RESGATE,              /**< Operações de busca e salvamento                */
    INCENDIO_URBANO       /**< Combate a incêndios urbanos e industriais      */
} EspecialidadeBombeiro;

/**
 * @brief Estado operacional atual de um bombeiro.
 *
 * Controla a disponibilidade do bombeiro para novas intervenções
 * e permite rastreamento da alocação de recursos humanos.
 */
typedef enum {
    DISPONIVEL,                 /**< Livre para ser alocado a uma intervenção */
    EM_INTERVENCAO_BOMBEIRO,    /**< Atualmente em operação no terreno        */
    EM_TREINO                   /**< Em formação ou exercício                 */
} EstadoOperacionalBombeiro;


/* ========================================================================= */
/*  ESTRUTURA DA ENTIDADE BOMBEIRO                                          */
/* ========================================================================= */

/**
 * @brief Entidade que representa um bombeiro no sistema.
 *
 * Cada bombeiro possui identificação única, nome, especialidade
 * e estado operacional. O campo `ativo` implementa soft delete,
 * preservando integridade referencial com intervenções históricas.
 */
typedef struct {
    int idBombeiro;                         /**< Identificador único (auto-increment) */
    char nome[MAX_NOME];                    /**< Nome completo do bombeiro            */
    EspecialidadeBombeiro especialidade;    /**< Área de especialização               */
    EstadoOperacionalBombeiro estado;       /**< Estado operacional atual             */
    int ativo;                              /**< 1=ativo, 0=inativo (soft delete)     */
} Bombeiro;


/* ========================================================================= */
/*  LISTA DINÂMICA DE BOMBEIROS                                             */
/* ========================================================================= */

/**
 * @brief Lista dinâmica que armazena todos os bombeiros do sistema.
 *
 * Cresce automaticamente quando a capacidade é atingida.
 * Utiliza array contíguo em memória para eficiência de acesso.
 */
typedef struct {
    Bombeiro *dados;   /**< Array dinâmico de bombeiros              */
    int tamanho;       /**< Número de bombeiros atualmente guardados */
    int capacidade;    /**< Capacidade máxima atual do array         */
} ListaBombeiros;


/* ========================================================================= */
/*  DECLARAÇÃO FORWARD DO CONTEXTO GLOBAL                                   */
/* ========================================================================= */

/*
 * Declaração antecipada para evitar dependência circular com sgi.h.
 * A definição completa está em sgi.h.
 */
typedef struct SistemaGestaoIncendios SistemaGestaoIncendios;


/* ========================================================================= */
/*  FUNÇÕES DE GESTÃO (CRUD)                                                */
/* ========================================================================= */

/**
 * @brief Cria um novo bombeiro e adiciona-o ao sistema.
 *
 * Solicita interativamente: nome e especialidade.
 * O estado inicial é sempre DISPONIVEL.
 *
 * @param sistema Ponteiro para o contexto global do sistema
 * @return ID do bombeiro criado, ou -1 em caso de erro (falta de memória)
 *
 * @note Expande automaticamente a lista se necessário
 * @note Regista log da operação
 */
int criarBombeiro(SistemaGestaoIncendios *sistema);

/**
 * @brief Lista todos os bombeiros ativos no sistema.
 *
 * Apresenta de forma formatada: ID, nome, especialidade e estado.
 * Ignora bombeiros inativos (ativo == 0).
 *
 * @param sistema Ponteiro para o contexto global (acesso read-only)
 *
 * @note Não regista log (evitar logs excessivos em operações de leitura)
 */
void listarBombeiros(const SistemaGestaoIncendios *sistema);

/**
 * @brief Atualiza os dados de um bombeiro existente.
 *
 * Permite modificar: nome, especialidade e estado operacional.
 * Lista os bombeiros ativos antes de pedir o ID.
 *
 * @param sistema Ponteiro para o contexto global do sistema
 *
 * @warning Se o bombeiro estiver em intervenção ativa, não pode alterar
 *          o estado para DISPONIVEL ou EM_TREINO
 * @note Apresenta preview das alterações e requer confirmação
 */
void atualizarBombeiro(SistemaGestaoIncendios *sistema);

/**
 * @brief Inativa logicamente um bombeiro (soft delete).
 *
 * O bombeiro deixa de aparecer em listagens e não pode ser alocado
 * a novas intervenções. Tenta eliminação física se não houver relações.
 *
 * @param sistema Ponteiro para o contexto global do sistema
 *
 * @warning Não permite inativar bombeiros em intervenções EM_EXECUCAO
 *
 * @see tentarEliminarBombeiro()
 */
void inativarBombeiro(SistemaGestaoIncendios *sistema);

/**
 * @brief Reativa um bombeiro previamente inativado.
 *
 * Torna o bombeiro novamente visível e disponível para alocação.
 * O estado é automaticamente reposto para DISPONIVEL.
 *
 * @param sistema Ponteiro para o contexto global do sistema
 */
void reativarBombeiro(SistemaGestaoIncendios *sistema);


/* ========================================================================= */
/*  FUNÇÕES DE PESQUISA                                                     */
/* ========================================================================= */

/**
 * @brief Procura um bombeiro pelo seu ID único.
 *
 * @param sistema Ponteiro para o contexto global (acesso read-only)
 * @param id ID do bombeiro a procurar
 * @return Ponteiro para o bombeiro encontrado, ou NULL se não existir
 *
 * @note Retorna o bombeiro mesmo que esteja inativo (ativo == 0)
 * @note Complexidade: O(n) — procura linear
 */
Bombeiro *procurarBombeiroPorId(const SistemaGestaoIncendios *sistema, int id);

/**
 * @brief Tenta eliminar fisicamente um bombeiro da memória.
 *
 * A eliminação só ocorre se TODAS as condições forem satisfeitas:
 * - Bombeiro está inativo (ativo == 0)
 * - Não está associado a nenhuma intervenção (mesmo concluídas)
 *
 * @param sistema    Ponteiro para o contexto global do sistema
 * @param idBombeiro ID do bombeiro a tentar eliminar
 *
 * @note Chamada automaticamente por inativarBombeiro()
 * @note Utiliza "swap and pop" — eficiente mas não preserva a ordem
 */
void tentarEliminarBombeiro(SistemaGestaoIncendios *sistema, int idBombeiro);


/* ========================================================================= */
/*  FUNÇÕES AUXILIARES (CONVERSÕES E MENUS INTERATIVOS)                    */
/* ========================================================================= */

/**
 * @brief Converte uma especialidade para string legível pelo utilizador.
 *
 * @param esp Especialidade a converter
 * @return String descritiva da especialidade; "Desconhecida" se inválida
 */
const char *especialidadeBombeiroParaString(EspecialidadeBombeiro esp);

/**
 * @brief Converte um estado operacional para string legível pelo utilizador.
 *
 * @param estado Estado a converter
 * @return String descritiva do estado; "Desconhecido" se inválido
 */
const char *estadoBombeiroParaString(EstadoOperacionalBombeiro estado);

/**
 * @brief Apresenta menu interativo e solicita escolha de especialidade.
 *
 * Exibe numeração e descrição de todas as especialidades disponíveis,
 * valida a entrada e repete até obter valor válido.
 *
 * @return Especialidade escolhida pelo utilizador
 */
EspecialidadeBombeiro escolherEspecialidadeBombeiro(void);

/**
 * @brief Apresenta menu interativo e solicita escolha de estado operacional.
 *
 * Exibe numeração e descrição de todos os estados disponíveis,
 * valida a entrada e repete até obter valor válido.
 *
 * @return Estado operacional escolhido pelo utilizador
 */
EstadoOperacionalBombeiro escolherEstadoBombeiro(void);

/**
 * @brief Expande a lista de bombeiros se a capacidade estiver esgotada.
 *
 * Duplica a capacidade quando tamanho == capacidade.
 *
 * @param lista Ponteiro para a lista a expandir
 * @return 1 se bem-sucedido (ou expansão desnecessária), 0 se falha de memória
 */
int expandirListaBombeirosSeNecessario(ListaBombeiros *lista);


#endif /* BOMBEIROS_H */