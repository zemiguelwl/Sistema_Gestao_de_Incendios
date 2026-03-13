#ifndef MODELOS_H
#define MODELOS_H

/**
 * @file modelos.h
 * @brief Header de compatibilidade — inclui o contexto global completo do SGI.
 *
 * Este ficheiro existe para manter compatibilidade com todos os módulos
 * existentes que fazem `#include "modelos.h"`. Inclui sgi.h, que por sua vez
 * inclui todos os módulos de entidade.
 *
 * Em código novo, prefira incluir diretamente o módulo necessário:
 * @code
 *   #include "sgi.h"           // contexto global + todas as entidades
 *   #include "bombeiros.h"     // apenas tipos e funções de bombeiros
 *   #include "equipamentos.h"  // apenas tipos e funções de equipamentos
 *   #include "ocorrencias.h"   // apenas tipos e funções de ocorrências
 *   #include "intervencoes.h"  // apenas tipos e funções de intervenções
 *   #include "tipos_comuns.h"  // apenas constantes e DataHora
 * @endcode
 *
 * @author José (8220942)
 * @date 2024
 * @version 2.0
 */

#include "sgi.h"

#endif /* MODELOS_H */