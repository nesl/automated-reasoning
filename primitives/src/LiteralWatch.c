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
	// The decide literal can be positive or negative.
	// if u decide a negative literal then its positive version is resolved.
	// then I need the other literal version!!

	//Hence from the literal absolute index I can get the variable and then access the other literal which will be the resolved literal
	// get the tail of the decisions list
	Lit* decided_literal = sat_state->decisions[sat_state->num_literals_in_decision -1];

	Var* corresponding_var = index2varp(decided_literal->sindex, sat_state);
	Lit* resolved_literal = NULL;
	if(decided_literal->sindex <0){
		resolved_literal = corresponding_var->posLit;
		resolved_literal->LitValue = 0;
		resolved_literal->LitState = 1;
	}
	else if(decided_literal->sindex >0){
		resolved_literal = corresponding_var->negLit;
		resolved_literal->LitValue = 1;
		resolved_literal->LitValue = 1;
	}

	//If no watching clauses on the resolved literal then do nothing and record the decision
	if(resolved_literal->num_watched_clauses == 0){
		// The implications list already has malloc with number of variables.
		sat_state->implications[sat_state->num_literals_in_implications++] = decided_literal;
		//wait for a new decision
		return;
	}
	else{
		//Get the watched clauses for the resolved literal
		for(unsigned long i = 0; i < resolved_literal->num_watched_clauses; i++){
				Clause* wclause = index2clausep( resolved_literal->list_of_watched_clauses[i], sat_state);
				// find another literal to watch that is free
				for(unsigned long j = 0; j < wclause->num_literals_in_clause; j++){
					if(is_free_literal(wclause->literals[j])){}
						// remove this clause from the list of watched clauses on the resolved literal and add it to the new free literal

				}

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



