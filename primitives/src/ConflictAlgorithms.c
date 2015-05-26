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

	sat_state->delta[sat_state->num_clauses_in_delta] = *(sat_state->alpha);

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

	sat_state->gamma[sat_state->num_clauses_in_gamma++] = *(sat_state->alpha);

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
static void resolution(Clause* alpha_l, Clause* wl_1, Clause* wl){

//	Var* resolvent_var;
//
//	// At the beginning union alpha_l and wl_1 in wl and then remove unnecessary literals
//
//	for(unsigned long i =0 ; i< alpha_l->num_literals_in_clause; i++){
//		Lit* alit = alpha_l->literals[i];
//		for(unsigned long j = 0; j< wl_1->num_literals_in_clause; j++){
//			Lit* wlit = wl_1->literals[j];
//			//check if they are of opposite sign
//			if(sat_literal_var(alit)->index == sat_literal_var(wlit)->index){
//				if((alit->sindex > 0 && wlit->sindex < 0) || (alit->sindex < 0 && wlit->sindex > 0)){
//
//				}
//			}
//		}
//	}


//	BOOLEAN flag_dont_add_alit;
//
//	Clause*	big;
//	Clause*	small;
//
//	if(alpha_l->num_literals_in_clause >= wl_1->num_literals_in_clause){
//		big = alpha_l;
//		small = wl_1;
//	}
//	else{
//		big = wl_1;
//		small = alpha_l;
//	}
//
//
//	for (unsigned long i =0; i < big->num_literals_in_clause; i++){
//		flag_dont_add_alit = 0;
//		Lit* alit = big->literals[i];
//		for(unsigned long j =0; j<small->num_literals_in_clause; j++){
//			Lit* wlit = small->literals[j];
//
//			//if same variable check if they are the same sign and index then do nothing (alit will be added by the flag later)
//			if(sat_literal_var(alit)->index == sat_literal_var(wlit)->index){
//				if((alit->sindex > 0 && wlit->sindex > 0)   || (alit->sindex < 0 && wlit->sindex < 0)){
//					continue;
//				}
//
//				// if they are of different signs then this literal is removed -- continue
//				if((alit->sindex > 0 && wlit->sindex < 0)   || (alit->sindex < 0 && wlit->sindex > 0)){
//					flag_dont_add_alit = 1;
//				}
//
//			}
//
//		}
//		if(!flag_dont_add_alit){
//			add_literal_to_clause(alpha_l->literals[i], wl);
//		}
//
//
//	}
}


static void initialize_learning_clause(Clause* wl_1, Clause* conflict_clause){

	 wl_1->cindex = conflict_clause->cindex;
	 wl_1->is_subsumed = conflict_clause->is_subsumed;
	 wl_1->mark = conflict_clause->mark;
	 wl_1->L1 = conflict_clause->L1;
	 wl_1->L2 = conflict_clause->L2;
	 wl_1->max_size_list_literals = conflict_clause->max_size_list_literals;
	 wl_1->num_literals_in_clause = conflict_clause->num_literals_in_clause;

	 for(unsigned long i =0; i< conflict_clause->num_literals_in_clause; i++){
		wl_1->literals[i] = conflict_clause->literals[i];
	 }

}


/******************************************************************************
	First UIP algorithm for the non-chronological backtracking using the
	linear time implementation algorithms as described by Marques Silva, J.P. and
	Sakallah, K.A."Dynamic Search-Space Pruning Techniques in Path Sensitization-94"
******************************************************************************/
Clause* CDCL_non_chronological_backtracking_first_UIP(SatState* sat_state){

	// initially the learning clause (wl) and (wl-1)
	Clause* wl = (Clause*) malloc(sizeof(Clause));
	wl->literals = (Lit**) malloc(sizeof(Lit*));
	wl->is_subsumed = 0;
	wl->max_size_list_literals = 1;
	wl->num_literals_in_clause = 0;



	Clause* wl_1 = (Clause*) malloc(sizeof(Clause));
	wl_1->literals = (Lit**) malloc(sizeof(Lit*) * (sat_state->conflict_clause->num_literals_in_clause)); // because at the beginning wl-1 is copied with conflict clause

	//initially wl_1 = conflict clause //deep copy
	initialize_learning_clause(wl_1, sat_state->conflict_clause);

	//fixed point
	BOOLEAN fixed_point_achieved = 0;

	while(fixed_point_achieved == 0){
		Clause* alpha_l = NULL;
		for(unsigned long i=0; i< wl_1->num_literals_in_clause; i++){
			Lit* lit = wl_1->literals[i];
			if(lit->decision_level == sat_state->current_decision_level){
				alpha_l = sat_literal_var(lit)->antecedent;
				break; // break for
			}
		}

		// resolve the conflict clause with the asserted variable antecedent and update the conflict clause (wl)
		// for the next loop
		resolution(alpha_l, wl_1, wl);

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
		else{
			//switch the pointers
			//wl now will be wl_1 and wl values are freed but to keep its space we just point to the space of wl_1 and freed it
			Clause* tmp = wl_1;
			wl_1 = wl;
			wl = tmp;
			wl->max_size_list_literals = 1;
			wl->num_literals_in_clause = 0;
		}
	}

	sat_state->alpha = wl;
	return wl;

}
