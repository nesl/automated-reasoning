/*
 * LiteralWatch.c
 *
 *  Created on: May 19, 2015
 *      Author: salma
 */

#include "LiteralWatch.h"


#define MALLOC_GROWTH_RATE 	   	2

/** Global variable for this file **/
BOOLEAN literal_watched_initialized = 0;

static unsigned long add_watching_clause(Clause* clause, Lit* lit, unsigned long MIN_CAPACITY){

	//get clause index.
	unsigned long index = clause->cindex;

	if(lit->num_watched_clauses >= MIN_CAPACITY){
			// needs to realloc the size
			MIN_CAPACITY =(lit->num_watched_clauses * MALLOC_GROWTH_RATE);
			lit->list_of_watched_clauses = (unsigned long*) realloc( lit->list_of_watched_clauses, sizeof(unsigned long) * MIN_CAPACITY);
	}

	lit->list_of_watched_clauses[lit->num_watched_clauses] = index;

	lit->num_watched_clauses ++;

	return MIN_CAPACITY;
}

static void intitialize_watching_clauses(SatState* sat_state){

	unsigned long MIN_CAPACITY1 =1;
	unsigned long MIN_CAPACITY2 =1;
	for(unsigned long i =0; i< sat_state->num_clauses_in_delta; i++){
		Clause watching_clause = sat_state->delta[i];
		Lit* watched_literal1 = watching_clause.L1;
		Lit* watched_literal2 = watching_clause.L2;
		MIN_CAPACITY1 = add_watching_clause(&watching_clause, watched_literal1, MIN_CAPACITY1);
		MIN_CAPACITY2 = add_watching_clause(&watching_clause, watched_literal2, MIN_CAPACITY2);
	}
	// Set the initialization flag;
	literal_watched_initialized = 1;
}





/******************************************************************************
	Two literal watch algorithm for unit resolution
The algorithm taken from the Class Notes for CS264A, UCLA

******************************************************************************/

void two_literal_watch(SatState* sat_state){

	// initialize the list of watched clauses
	if(!literal_watched_initialized)
		intitialize_watching_clauses(sat_state);


	Clause* conflict_clause = NULL;

	// I know here that the decision array is filled with at least one element
	// get the last decision in the decision array and try to propagate from there

	Lit* decided_literal = sat_state->decisions[sat_state->num_literals_in_decision -1];

	//check if the decided literal is resolved literal
	if((decided_literal->sindex <0 && decided_literal->LitValue == 1) || (decided_literal->sindex > 0 && decided_literal->LitValue == 0) ){
		for(unsigned long i = 0; i < decided_literal->num_watched_clauses; i++){
			Clause* wclause = index2clausep( decided_literal->list_of_watched_clauses[i], sat_state);
		}
	}




//	// A state that keeps track of the current decisions and implications
//	Lit** state = (Lit**)malloc(sizeof(Lit*) * sat_state->num_variables_in_state);
//
//	// Pending list
//	// initialize memory of pending literals with one element and then increase the size by double if needed
//	unsigned long MIN_CAPACITY_PENDING = 1;
//	unsigned long num_literals_in_pending = 0;
//	Lit** pending = (Lit**)malloc(sizeof(Lit*) * MIN_CAPACITY_PENDING );






	//add literals to pending
	//realloc
//	if(num_literals_in_pending >= MIN_CAPACITY_PENDING){
//		// needs to realloc the size
//		MIN_CAPACITY_PENDING = num_literals_in_pending * MALLOC_GROWTH_RATE;
//		pending = (Lit**) realloc( pending, sizeof(Lit*) * MIN_CAPACITY_PENDING);
//	}
//

	return ;
}



