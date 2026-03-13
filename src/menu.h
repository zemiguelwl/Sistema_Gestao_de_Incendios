#ifndef MENU_H
#define MENU_H

/**
 * @file menu.h
 * @brief Submenus interativos do Sistema de Gestão de Incêndios.
 *
 * Cada função representa um submenu dedicado a uma área funcional
 * da aplicação. O fluxo de navegação é:
 *
 * @code
 *   mostrarMenuPrincipal()
 *     ├── menuOcorrencias()
 *     ├── menuBombeiros()
 *     ├── menuEquipamentos()
 *     ├── menuIntervencoes()
 *     └── menuRelatorios()
 * @endcode
 *
 * Os submenus executam em ciclo até o utilizador escolher voltar
 * ao menu principal.
 *
 * @author José (8220942)
 * @date 2024
 * @version 2.0
 */

#include "modelos.h"


/* ========================================================================= */
/*  MENU PRINCIPAL                                                           */
/* ========================================================================= */

/**
 * @brief Apresenta o menu principal e encaminha para os submenus.
 *
 * Executa em ciclo até o utilizador optar por sair.
 * Guarda os dados automaticamente antes de encerrar.
 *
 * @param sistema Ponteiro para o contexto global do sistema
 */
void mostrarMenuPrincipal(SistemaGestaoIncendios *sistema);


/* ========================================================================= */
/*  SUBMENUS POR ENTIDADE                                                    */
/* ========================================================================= */

/**
 * @brief Submenu de gestão de ocorrências.
 *
 * Operações disponíveis: criar, listar, atualizar, inativar, reativar.
 *
 * @param sistema Ponteiro para o contexto global do sistema
 */
void menuOcorrencias(SistemaGestaoIncendios *sistema);

/**
 * @brief Submenu de gestão de bombeiros.
 *
 * Operações disponíveis: criar, listar, atualizar, inativar, reativar.
 *
 * @param sistema Ponteiro para o contexto global do sistema
 */
void menuBombeiros(SistemaGestaoIncendios *sistema);

/**
 * @brief Submenu de gestão de equipamentos.
 *
 * Operações disponíveis: criar, listar, atualizar, inativar, reativar.
 *
 * @param sistema Ponteiro para o contexto global do sistema
 */
void menuEquipamentos(SistemaGestaoIncendios *sistema);

/**
 * @brief Submenu de gestão de intervenções.
 *
 * Operações disponíveis: criar, listar, iniciar execução, concluir,
 * adicionar bombeiros, adicionar equipamentos, inativar, reativar.
 *
 * @param sistema Ponteiro para o contexto global do sistema
 */
void menuIntervencoes(SistemaGestaoIncendios *sistema);

/**
 * @brief Submenu de relatórios estatísticos e operacionais.
 *
 * Agrupa relatórios por categoria: ocorrências, intervenções,
 * bombeiros, equipamentos e estratégicos.
 *
 * @param sistema Ponteiro para o contexto global do sistema
 */
void menuRelatorios(SistemaGestaoIncendios *sistema);


#endif /* MENU_H */