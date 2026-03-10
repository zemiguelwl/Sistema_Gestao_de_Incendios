#ifndef MENU_H
#define MENU_H

#include "modelos.h"

/**
 * @file menu.h
 * @brief Declarações relacionadas com os menus principais da aplicação.
 *
 * Cada função representa um submenu dedicado à gestão de um tipo de entidade
 * (ocorrências, bombeiros, equipamentos, intervenções, relatórios).
 */

/**
 * @brief Mostra o menu principal do sistema.
 *
 * @param sistema Ponteiro para o sistema de gestão de incêndios.
 */
void mostrarMenuPrincipal(SistemaGestaoIncendios *sistema);

/**
 * @brief Menu de operações relacionadas com Ocorrências.
 *
 * @param sistema Ponteiro para o sistema de gestão de incêndios.
 */
void menuOcorrencias(SistemaGestaoIncendios *sistema);

/**
 * @brief Menu de operações relacionadas com Bombeiros.
 *
 * @param sistema Ponteiro para o sistema de gestão de incêndios.
 */
void menuBombeiros(SistemaGestaoIncendios *sistema);

/**
 * @brief Menu de operações relacionadas com Equipamentos.
 *
 * @param sistema Ponteiro para o sistema de gestão de incêndios.
 */
void menuEquipamentos(SistemaGestaoIncendios *sistema);

/**
 * @brief Menu de operações relacionadas com Intervenções.
 *
 * @param sistema Ponteiro para o sistema de gestão de incêndios.
 */
void menuIntervencoes(SistemaGestaoIncendios *sistema);

/**
 * @brief Menu de relatórios estatísticos e consultas agregadas.
 *
 * @param sistema Ponteiro para o sistema de gestão de incêndios.
 */
void menuRelatorios(SistemaGestaoIncendios *sistema);

#endif /* MENU_H */
