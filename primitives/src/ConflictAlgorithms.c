/*
 * ConflictAlgorithms.c
 *
 *  Created on: May 22, 2015
 *      Author: salma
 */

#include "ConflictAlgorithms.h"
#include "LiteralWatch.h"
#define MALLOC_GROWTH_RATE 	   	2

static void update_delta_with_gamma(SatState* sat_state){
	if(sat_state->num_clauses_in_delta >= sat_state->max_size_list_delta){
			// needs to realloc the size
			sat_state->max_size_list_delta =(sat_state->num_clauses_in_delta * MALLOC_GROWTH_RATE);
			sat_state->delta = (Clause*) realloc( sat_state->delta, sizeof(Clause*) * (sat_state->max_size_list_delta));
		}

	sat_state->delta[sat_state->num_clauses_in_delta] = sat_state->alpha;

	// update the watching literals L1 and L2
	sat_state->delta[sat_state->num_clauses_in_delta].L1 = sat_state->delta[sat_state->num_clauses_in_delta].literals[0];
	sat_state->delta[sat_state->num_clauses_in_delta].L2 = sat_state->delta[sat_state->num_clauses_in_delta].literals[1];


	//update the state of the literals with that new watching clause
	add_watching_clause(&sat_state->delta[sat_state->num_clauses_in_delta], sat_state->delta[sat_state->num_clauses_in_delta].L1);
	add_watching_clause(&sat_state->delta[sat_state->num_clauses_in_delta], sat_state->delta[sat_state->num_clauses_in_delta].L2);

	//update number of clauses in delta
	sat_state->num_clauses_in_delta++;
}




void add_clause_to_gamma(SatState* sat_state){
	if(sat_state->num_clauses_in_gamma >= sat_state->max_size_list_gamma){
		// needs to realloc the size
		sat_state->max_size_list_gamma =(sat_state->num_clauses_in_gamma * MALLOC_GROWTH_RATE);
		sat_state->gamma = (Clause*) realloc( sat_state->gamma, sizeof(Clause*) * (sat_state->max_size_list_gamma));
	}

	sat_state->gamma[sat_state->num_clauses_in_gamma++] = sat_state->alpha;

	update_delta_with_gamma(sat_state);
}


void CDCL_non_chronological_backtracking(SatState* sat_state){


}
