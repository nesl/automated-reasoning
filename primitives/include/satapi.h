#ifndef SATAPI_H_
#define SATAPI_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>




/******************************************************************************
 * typedefs
 ******************************************************************************/

typedef char BOOLEAN;


/******************************************************************************
 * Basic structures
 ******************************************************************************/

/******************************************************************************
 * Variables:
 * --You must represent variables using the following struct
 * --Variable index must start at 1
 * --Index of a variable must be of type "unsigned long"
 ******************************************************************************/

typedef struct {

	unsigned long index;

} Var;


/******************************************************************************
 * Literals:
 * --You must represent literals using the following struct
 * --Positive literals' indices range from 1 to n (n is the number of variables)
 * --Negative literals' indices range from -n to -1 (n is the number of variables)
 * --Index of a literal must be of type "signed long"
 ******************************************************************************/

typedef struct {

	signed long sindex;
	BOOLEAN LitState;
	unsigned long decision_level;
} Lit;


/******************************************************************************
 * Clauses:
 * --You must represent clauses using the following struct
 * --Clause index must start at 1
 * --Each clause must maintain a field to decide whether the clause is subsumed in
 * the current setting (i.e., if any literal of the clause is asserted)
 ******************************************************************************/

typedef struct {

  Lit * literals;
  unsigned long num_literals_in_clause;
  BOOLEAN is_subsumed;

} Clause;


/******************************************************************************
 * SatState:
 * --The following structure will keep track of the data needed to
 * condition/uncondition, perform unit resolution, and so on ...
 ******************************************************************************/

typedef struct {

  Clause* delta;
  Clause* gamma;
  Lit* decisions;
  Lit* implications;
  Clause alpha;

  unsigned long  num_clauses_in_delta;
  unsigned long  num_clauses_in_gamma;
  unsigned long  num_literals_in_decision;
  unsigned long  num_literals_in_implications;
  unsigned long  num_variables_in_state;

  unsigned long current_decision_level;

} SatState;


/******************************************************************************
 * API:
 * --Using the above structures you must implement the following functions
 * --Detailed explanations of the functions can be found in satapi.c
 * --These functions are all you need for the knowledge compiler
 * --Note that most of the functions can be implemented in 1 line
 ******************************************************************************/

//Variables
Var* index2varp(unsigned long,SatState*);

//Literals
Lit* pos_literal(Var*);
Lit* neg_literal(Var*);
BOOLEAN set_literal(Lit*);

//Clauses
Clause* index2clausep(unsigned long,SatState*);
BOOLEAN subsumed_clause(Clause*);

//SatState
SatState* construct_sat_state(char*);
void free_sat_state(SatState*);
BOOLEAN unit_resolution(SatState*);
void undo_unit_resolution(SatState*);
BOOLEAN decide_literal(Lit*,SatState*);
void undo_decide_literal(SatState*);
BOOLEAN add_asserting_clause(SatState*);
BOOLEAN at_assertion_level(SatState*);
BOOLEAN at_start_level(SatState*);
BOOLEAN conflict_exists(SatState*);

#endif // SATAPI_H_

/******************************************************************************
 * end
 ******************************************************************************/
