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
	//TODO: Do I really need to do this. I just need to update the watching clause lists for the literals. Keep this for now but check later
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

	// update the index of the new added clause
	sat_state->delta[sat_state->num_clauses_in_delta].cindex = sat_state->num_clauses_in_delta + 1;

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

static void add_literal_to_clause(Lit* lit, Clause* clause){

	if(clause->num_literals_in_clause >= clause->max_size_list_literals){
		// needs to realloc the size
		clause->max_size_list_literals = clause->num_literals_in_clause * MALLOC_GROWTH_RATE;
		clause->literals = (Lit**) realloc( clause->literals, sizeof(Lit*) * clause->max_size_list_literals);
	}

	clause->literals[clause->num_literals_in_clause++] = lit;
}


// Resolution between two clauses
static void resolution(Clause* w1, Clause* w2, Clause* wreturn){
	BOOLEAN flag_dont_add_i;
	for (unsigned long i =0; i<w1->num_literals_in_clause; i++){
		flag_dont_add_i = 0;
		for(unsigned long j =0; j<w2->num_literals_in_clause; j++){
			//check the variable that is pos_lit in w1 and neg_lit in the w2 or vice versa --> then this is the variable you resolve on
			if((abs(w1->literals[i]->sindex) == w2->literals[j]->sindex)   ||  (w1->literals[i]->sindex == abs(w2->literals[j]->sindex)) ){
				flag_dont_add_i = 1;
				continue;
			}
			else
				add_literal_to_clause(w2->literals[j], wreturn);
		}
		if(!flag_dont_add_i)
			add_literal_to_clause(w1->literals[i], wreturn);

	}
}

/******************************************************************************
	First UIP algorithm for the non-chronological backtracking using the
	linear time implementation algorithms as described by Marques Silva, J.P. and
	Sakallah, K.A."Dynamic Search-Space Pruning Techniques in Path Sensitization-94"
******************************************************************************/
void CDCL_non_chronological_backtracking_first_UIP(SatState* sat_state){

	// initially the learning clause (wl) is the conflict clause
	Clause* wl = sat_state->conflict_clause;

	//fixed point
	BOOLEAN fixed_point_achieved = 0;

	while(fixed_point_achieved == 0){
		Clause* alpha_l = NULL;
		for(unsigned long i=0; i< wl->num_literals_in_clause; i++){
			Lit* lit = wl->literals[i];
			if(lit->decision_level == sat_state->current_decision_level){
				alpha_l = lit->antecedent;
				break; // break for
			}
		}
		resolution(alpha_l, wl, wl);

		//check the breaking condition of fixed point
		unsigned long count_asserting_literals = 0;
		for(unsigned long i =0; i < wl->num_literals_in_clause; i++){
			Lit* lit = wl->literals[i];
			if(lit->decision_level == sat_state->current_decision_level){
				count_asserting_literals++;
			}
		}
		if(count_asserting_literals == 1) // is indeed an asserting clause // only one literal at the current decision level
			fixed_point_achieved = 1;
	}

	sat_state->alpha = *wl;
}
