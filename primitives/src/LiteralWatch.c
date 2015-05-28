/*
 * LiteralWatch.c
 *
 *  Created on: May 19, 2015
 *      Author: salma
 */

#include "LiteralWatch.h"
#include "global.h"
#include <assert.h>

// local value for this file
static BOOLEAN INIT_LITERAL_WATCH = 0;


void clear_init_literal_watch(){
	INIT_LITERAL_WATCH = 0;
}


static void intitialize_watching_clauses(SatState* sat_state){

	for(unsigned long i =0; i< sat_state->num_clauses_in_delta; i++){
		Clause watching_clause = sat_state->delta[i];
		Lit* watched_literal1 = watching_clause.L1;
		Lit* watched_literal2 = watching_clause.L2;
		add_watching_clause(&watching_clause, watched_literal1);
		add_watching_clause(&watching_clause, watched_literal2);
	}
	// Set the initialization flag;
	INIT_LITERAL_WATCH = 1;
}

static Lit* get_resolved_lit(Lit* decided_literal, SatState* sat_state){
	Var* corresponding_var = decided_literal->variable;
	Lit* resolved_literal = NULL;
	if(decided_literal->sindex <0){
		resolved_literal = corresponding_var->posLit;
		resolved_literal->LitValue = 0;
		resolved_literal->LitState = 1;
	}
	else if(decided_literal->sindex >0){
		resolved_literal = corresponding_var->negLit;
		resolved_literal->LitValue = 1;
		resolved_literal->LitState = 1;
	}

	resolved_literal->decision_level = decided_literal->decision_level;

	return resolved_literal;
}

static void add_literal_to_list(Lit** list, Lit* lit, unsigned long* capacity, unsigned long* num_elements){

	unsigned long cap = *capacity;
	unsigned long num = *num_elements;

	if(num >= cap){
		// needs to realloc the size
		cap =num * MALLOC_GROWTH_RATE;
		list = (Lit**) realloc(list , sizeof(Lit*) * cap);
	}

		list[num++] = lit;

		*num_elements = num;
		*capacity = cap;

}


//static Clause* construct_unit_clause(Lit* free_lit){
//	Clause* unit_clause = (Clause*) malloc (sizeof(Clause));
//	unit_clause->literals = (Lit**) malloc (sizeof(Lit*));
//	unit_clause->max_size_list_literals = 1;
//	unit_clause->num_literals_in_clause = 1;
//	// I don't care about the rest of the parameters
//	unit_clause->literals[0] = free_lit;
//	return unit_clause;
//}



/******************************************************************************
add watching clause to the list of watching clauses over a literal
******************************************************************************/
void add_watching_clause(Clause* clause, Lit* lit){

	//get clause index.
	unsigned long index = clause->cindex;

	if(lit->num_watched_clauses >= lit->max_size_list_watched_clauses){
			// needs to realloc the size
			lit->max_size_list_watched_clauses =(lit->num_watched_clauses * MALLOC_GROWTH_RATE);
			lit->list_of_watched_clauses = (unsigned long*) realloc( lit->list_of_watched_clauses, sizeof(unsigned long) * lit->max_size_list_watched_clauses);
	}

	lit->list_of_watched_clauses[lit->num_watched_clauses] = index;

	lit->num_watched_clauses ++;
}


/******************************************************************************
	Two literal watch algorithm for unit resolution
The algorithm taken from the Class Notes for CS264A, UCLA

******************************************************************************/
BOOLEAN two_literal_watch(SatState* sat_state, Lit** literals_list, unsigned long num_elements, unsigned long capacity){

	// Once I entered here I must have elements in the decision array
	assert(sat_state->num_literals_in_decision > 0);

	// initialize the list of watched clauses
	if(!INIT_LITERAL_WATCH)
		intitialize_watching_clauses(sat_state);

	BOOLEAN contradiction_flag = 0;

	//TODO: Due to recursion of pending list we may need to consider more than one decided literal then we need a list that captures all literals of last decision

	//loop on all decided literals
	Lit** literals_in_decision = literals_list;
	unsigned long max_size_decision_list = capacity;
	unsigned long num_decision_lit = num_elements;


	// Create pending literal list
	Lit** pending_list = (Lit**)malloc(sizeof(Lit*));
	unsigned long max_size_pending_list = 1;
	unsigned long num_pending_lit = 0;

	for(unsigned long i =0; i<num_decision_lit; i++){
		//Lit* decided_literal = sat_state->decisions[sat_state->num_literals_in_decision -1];
		Lit* decided_literal = literals_in_decision[i];
		Lit* resolved_literal = get_resolved_lit(decided_literal, sat_state);


		//TODO: uncomment Update clauses state based on the decided literal
		//sat_update_clauses_state(decided_literal);
		//TODO: Enhance: we can only get the watched clauses that are not subsumed to speed it up and to avoid checking the unit clause



		//If no watching clauses on the resolved literal then do nothing and record the decision
		if(resolved_literal->num_watched_clauses == 0){
			// The decided literal is already in the decision list so no need to record it again
			//wait for a new decision
			continue;
		}
		else{
			//Get the watched clauses for the resolved literal
			for(unsigned long i = 0; i < resolved_literal->num_watched_clauses; i++){

					Clause* wclause = sat_index2clause( resolved_literal->list_of_watched_clauses[i], sat_state);

					//TODO: Check if this actually works to skip the subsumed clauses
					//if(wclause->is_subsumed) continue;

					Lit* free_lit = NULL;
					// if unit (only one free)
					unsigned long num_free_literals = 0;
					for(unsigned long j = 0; j < wclause->num_literals_in_clause; j++){
						if(sat_implied_literal(wclause->literals[j]) ==  0){
							num_free_literals++;
							free_lit = wclause->literals[j];
						}
					}

					if(num_free_literals == 1){
						//I have a unit clause take an implication and update the antecedent
						//TODO The antecedent is a unit clause!

						// maybe here put a check!!
						//if the antecedent is already assigned then
						//don't add it again and most probably you will have a conflict in the next round
						if(sat_literal_var(free_lit)->antecedent == NULL){
							// construct a unit clause for the antecedent
							//Clause* unit_clause = construct_unit_clause(free_lit);
							sat_literal_var(free_lit)->antecedent = wclause;  // remember the decision list is updated with the pending list so that's ok

#ifdef DEBUG
							printf("free literal %ld\n",free_lit->sindex);
							//print_clause(wclause);
							printf("Antecedent: ");
							print_clause(sat_literal_var(free_lit)->antecedent);
#endif
						}
						// the last free literal is the only free literal in this case
						add_literal_to_list(pending_list,  free_lit , &max_size_pending_list, &num_pending_lit);
						continue; // go to the next clause
					}

					if (num_free_literals == 0){
						//contradiction --> everything is resolved
						//subsumed clause --> do nothing

						// I have to check the other watched literal!? only if it is not a unit clause
						Lit* the_other_watched_literal = NULL;

						if(wclause->num_literals_in_clause > 2){
							if(resolved_literal->sindex == wclause->L1->sindex)
								the_other_watched_literal = wclause->L2;
							else if(resolved_literal->sindex == wclause->L2->sindex)
								the_other_watched_literal = wclause->L1;

							if(sat_is_resolved_literal(the_other_watched_literal)){
								// all literal of the clause are resolved --> contradiction
								contradiction_flag = 1;
								printf("-------------Contradiction happens with clause: %ld\n",wclause->cindex);
								sat_state->conflict_clause = wclause;

								break; //break from loop of watched clauses over this decided literal
							}
							else if(sat_is_asserted_literal(the_other_watched_literal)){
								// we do nothing since this clause is subsumed
								wclause->is_subsumed = 1;
								contradiction_flag = 0; // just checking  if I have to reassign the value in order to avoid the compilation optimization
								continue;
							}
						}
						else{ // unit clause
							Lit* unit_lit = wclause->literals[0];
							if(sat_is_asserted_literal(unit_lit)){
								wclause->is_subsumed = 1;
								contradiction_flag = 0;
								continue;
							}
							else if(sat_is_resolved_literal(unit_lit)){ // this should never happen unless unsat // you can't have unit clause that is not asserted
								contradiction_flag = 1;
								printf("-------------Contradiction happens with clause: %ld\n",wclause->cindex);
								sat_state->conflict_clause = wclause;
								break;
							}
						}

					}

					// find another literal to watch that is free
					for(unsigned long j = 0; j < wclause->num_literals_in_clause; j++){
						if((!sat_implied_literal(wclause->literals[j])) && wclause->literals[j] != wclause->L1 && wclause->literals[j] != wclause->L2){
							Lit* new_watched_lit = wclause->literals[j];
							//add the watching clause to the free literal
							add_watching_clause(wclause, new_watched_lit);

							// remove the resolved literal from the watch and update the clause watch list
							if(resolved_literal->sindex == wclause->L1->sindex)
								wclause->L1 = new_watched_lit;
							else if (resolved_literal->sindex == wclause->L2->sindex)
								wclause->L2 = new_watched_lit;
							break;
						}
					}

			} //end of for for watched clauses over this decided literal

			if(num_pending_lit > 0){
			// Pass over the pending list and add the literals to the decision while maintaining the level
				for(unsigned long i =0; i < num_pending_lit; i++){
					// pending literal was an already watched clause so the associated list of watching clauses is already handled
					// fix values of pending literal before putting in decision
					Lit* pending_lit = pending_list[i];

					if(pending_lit->sindex <0){
						pending_lit->LitValue = 0;
						pending_lit->LitState = 1;
					}
					else if(pending_lit->sindex >0){
						pending_lit->LitValue = 1;
						pending_lit->LitState = 1;
					}

					pending_lit->decision_level = sat_state->current_decision_level;
					//update the decision list
					sat_state->decisions[sat_state->num_literals_in_decision++] = pending_lit;

					//update list of literals in last decision because this is the main loop
					literals_in_decision[num_decision_lit++] = pending_lit;
				}

//				//free pending list for the next round
//				if(pending_list != NULL && num_pending_lit != 0)
//					FREE(pending_list);
			}
		}

		//free pending list for the next round
		//--> pending list is related to each decided literal. so before taking a new decision clear the pending list
		//double free or corruption (out):
		if(pending_list != NULL && num_pending_lit != 0){
			//FREE(pending_list);
			// TODO: free pending list
			num_pending_lit = 0;
		}


		if(contradiction_flag == 1) // no need to go for another literal in the decided literal list
			break;

	}//end of for for decide literal



	//TODO: This is so bad ... need to figure out why this sentence is executed even if I have a return when the contradiction happens
	//TODO: needs to find a way to clear the literals in last_decision
//	if(literals_in_last_decision != NULL && num_last_decision_lit !=0)
//		FREE(literals_in_last_decision);

	if(contradiction_flag == 1)
		return 0;
	else
		return 1;

}



