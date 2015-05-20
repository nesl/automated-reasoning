/*
 * satapi.h
 *
 *  Created on:  Apr 30, 2015
 *      Author: salma
 */

#ifndef SATAPI_H_
#define SATAPI_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>



/******************************************************************************
 * typedefs
 ******************************************************************************/

typedef char BOOLEAN; //1 is true; 0 is false; d is don't care or free


/******************************************************************************
 * Basic structures
 ******************************************************************************/



/******************************************************************************
 * Literals:
 * --You must represent literals using the following struct
 * --Positive literals' indices range from 1 to n (n is the number of variables)
 * --Negative literals' indices range from -n to -1 (n is the number of variables)
 * --Index of a literal must be of type "signed long"
 ******************************************************************************/

typedef struct {

	signed long sindex;
	BOOLEAN LitState;			  // whether it is set or not

	unsigned long decision_level; // if it is decided or implied what is the level of the literal in this setting

	BOOLEAN LitValue;			// whether it has value true, false, or free(not set)

	/** for the two literal watch data structure */
	unsigned long num_watched_clauses;
	unsigned long* list_of_watched_clauses;  // List of clause indices that contain this literal as a watched literal

} Lit;


/******************************************************************************
 * Variables:
 * --You must represent variables using the following struct
 * --Variable index must start at 1
 * --Index of a variable must be of type "unsigned long"
 ******************************************************************************/

typedef struct {
//TODO: I switched the place of Var with Lit to avoid forward declaration --> Check if actually we need this
	unsigned long index;
	Lit* posLit; // Keep track of the variable positive literal
	Lit* negLit; // keep track of the variable negative literal
} Var;

/******************************************************************************
 * Clauses:
 * --You must represent clauses using the following struct
 * --Clause index must start at 1
 * --Each clause must maintain a field to decide whether the clause is subsumed in
 * the current setting (i.e., if any literal of the clause is asserted)
 ******************************************************************************/

typedef struct {

  Lit ** literals;
  unsigned long cindex;
  unsigned long num_literals_in_clause;
  BOOLEAN is_subsumed;

  /** for the two literal watch data structure */
  Lit* L1; //first literal
  Lit* L2; //second literal

} Clause;


/******************************************************************************
 * SatState:
 * --The following structure will keep track of the data needed to
 * condition/uncondition, perform unit resolution, and so on ...
 ******************************************************************************/

typedef struct {

  Clause* delta;
  Clause* gamma;
  Lit** decisions;
  Lit** implications;
  Clause alpha;

  // Variables in the problem space and their positive and negative literals.
  // keep them here now because I don't know where else to allocate them
  Var* variables; //Array of variables

  unsigned long  num_clauses_in_delta;  // m
  unsigned long  num_clauses_in_gamma;
  unsigned long  num_literals_in_decision;
  unsigned long  num_literals_in_implications;
  unsigned long  num_variables_in_state; //n

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

BOOLEAN is_free_literal(Lit*);

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
