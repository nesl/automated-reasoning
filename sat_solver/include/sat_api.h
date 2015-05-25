#ifndef SATAPI_H_
#define SATAPI_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/******************************************************************************
 * typedefs
 ******************************************************************************/

typedef char BOOLEAN; //signed

typedef unsigned long c2dSize;  //for variables, clauses, and various things
typedef signed long c2dLiteral; //for literals
typedef double c2dWmc;          //for (weighted) model count

typedef struct var Var;
typedef struct literal Lit;
typedef struct clause Clause;
typedef struct sat_state_t SatState;

/******************************************************************************
 * function prototypes 
 ******************************************************************************/

/******************************************************************************
 * Variables
 ******************************************************************************/

Var* sat_index2var(c2dSize index, const SatState* sat_state);
c2dSize sat_var_index(const Var* var);
Var* sat_literal_var(const Lit* lit);
BOOLEAN sat_instantiated_var(const Var* var);
BOOLEAN sat_irrelevant_var(const Var* var);
c2dSize sat_var_count(const SatState* sat_state);
c2dSize sat_var_occurences(const Var* var);
Clause* sat_clause_of_var(c2dSize index, const Var* var);
BOOLEAN sat_marked_var(const Var* var);
void sat_mark_var(Var* var);
void sat_unmark_var(Var* var);

/******************************************************************************
 * Literals 
 ******************************************************************************/

Lit* sat_index2literal(c2dLiteral index, const SatState* sat_state);
c2dLiteral sat_literal_index(const Lit* lit);
Lit* sat_pos_literal(const Var* var);
Lit* sat_neg_literal(const Var* var);
BOOLEAN sat_implied_literal(const Lit* lit);
c2dWmc sat_literal_weight(const Lit* lit);
Clause* sat_decide_literal(Lit* lit, SatState* sat_state);
void sat_undo_decide_literal(SatState* sat_state);

/******************************************************************************
 * Clauses 
 ******************************************************************************/

Clause* sat_index2clause(c2dSize index, const SatState* sat_state);
c2dSize sat_clause_index(const Clause* clause);
Lit** sat_clause_literals(const Clause* clause);
c2dSize sat_clause_size(const Clause* clause);
BOOLEAN sat_subsumed_clause(const Clause* clause);
c2dSize sat_clause_count(const SatState* sat_state);
c2dSize sat_learned_clause_count(const SatState* sat_state);
Clause* sat_assert_clause(Clause* clause, SatState* sat_state);
BOOLEAN sat_marked_clause(const Clause* clause);
void sat_mark_clause(Clause* clause);
void sat_unmark_clause(Clause* clause);

/******************************************************************************
 * SatState
 ******************************************************************************/

SatState* sat_state_new(const char* file_name);
void sat_state_free(SatState* sat_state);
BOOLEAN sat_unit_resolution(SatState* sat_state);
void sat_undo_unit_resolution(SatState* sat_state);
BOOLEAN sat_at_assertion_level(const Clause* clause, const SatState* sat_state);

#endif //SATAPI_H_

/******************************************************************************
 * end
 ******************************************************************************/
