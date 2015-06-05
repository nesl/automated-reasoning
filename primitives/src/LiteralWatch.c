/*
 * LiteralWatch.c
 *
 *  Created on: May 19, 2015
 *      Author: salma
 */

#include "LiteralWatch.h"
#include "global.h"
#include <assert.h>
#include <stdlib.h>

// local value for this file
//static BOOLEAN INIT_LITERAL_WATCH = 0;


//void clear_init_literal_watch(){
//	INIT_LITERAL_WATCH = 0;
//}
//

//static void intitialize_watching_clauses(SatState* sat_state){
//
//	for(unsigned long i =0; i< sat_state->num_clauses_in_delta; i++){
//		Clause watching_clause = sat_state->delta[i];
//		Lit* watched_literal1 = watching_clause.L1;
//		Lit* watched_literal2 = watching_clause.L2;
//		add_watching_clause(&watching_clause, watched_literal1);
//		add_watching_clause(&watching_clause, watched_literal2);
//	}
//	// Set the initialization flag;
//	INIT_LITERAL_WATCH = 1;
//}


static BOOLEAN evaluate_delta(SatState* sat_state, Lit** pending_list, unsigned long num_pending_lit){
	BOOLEAN fails =0;
#ifdef DEBUG
	printf("Evaluate delta loop: %ld at pending lit %ld \n",num_pending_lit, pending_list[num_pending_lit-1]->sindex );
#endif
	// just for the sake of this function but will be reset if contradiction happens
	for(unsigned long i =0; i < num_pending_lit; i++){
		Lit* pending_lit = pending_list[i];
		pending_lit->decision_level = sat_state->current_decision_level;
		if(pending_lit->sindex <0){
			pending_lit->LitValue = 0;
			pending_lit->LitState = 1;

			Lit* opposite_Lit = sat_literal_var(pending_lit)->posLit;
			opposite_Lit->LitValue = 0;
			opposite_Lit->LitState = 1;
			opposite_Lit->decision_level = pending_lit->decision_level;
		}
		else if(pending_lit->sindex >0){
			pending_lit->LitValue = 1;
			pending_lit->LitState = 1;

			Lit* opposite_Lit = sat_literal_var(pending_lit)->negLit;
			opposite_Lit->LitValue = 1;
			opposite_Lit->LitState = 1;
			opposite_Lit->decision_level = pending_lit->decision_level;
		}
	}


	for(unsigned long i=0; i<sat_state->num_clauses_in_delta; i++){
		Clause* clause = &sat_state->delta[i];
		//check clause
		BOOLEAN contradiction_found = 1;
		for(unsigned long k = 0; k<clause->num_literals_in_clause;k++){
			//check if the literal is asserted or not yet set
			if(sat_is_asserted_literal(clause->literals[k]) || (!sat_implied_literal(clause->literals[k]) )){
				contradiction_found = 0;
				break; //don't check the other literals of the clause
			}
		} // for all literals in a clause

		if(contradiction_found == 1){
			fails = 1;
			sat_state->conflict_clause = clause;
#ifdef DEBUG
			printf("In Evaluate Delta: \t");
			print_clause(clause);
#endif
//			for(unsigned long j = 0; j < num_pending_lit; j++){
//				Lit* pending_lit = pending_list[j];
//				pending_lit->LitValue = 'u';
//				pending_lit->LitState = 0;
//				//SALMA added this
//				pending_lit->decision_level = 0;
//				if(pending_lit->sindex <0){
//					Lit* opposite_Lit = sat_literal_var(pending_lit)->posLit;
//					opposite_Lit->LitValue = 'u';
//					opposite_Lit->LitState = 0;
//					//SALMA added this
//					opposite_Lit->decision_level = 0;
//				}else{
//					Lit* opposite_Lit = sat_literal_var(pending_lit)->negLit;
//					opposite_Lit->LitValue = 'u';
//					opposite_Lit->LitState = 0;
//					//SALMA added this
//					opposite_Lit->decision_level = 0;
//				}
//
//			}
			break;
		}

	}//end of for loop on clauses in delta
#ifdef DEBUG
	printf("Contradiction_found = %d\n",fails);
#endif

	return fails;
}



static BOOLEAN evaluate_current_assignment(SatState* sat_state, Lit** pending_list, unsigned long num_pending_lit){
#ifdef DEBUG
	printf("Evaluate the current assignment\n");
#endif
	BOOLEAN fails = 0;

	// just for the sake of this function but will be reset if contradiction happens
	for(unsigned long i =0; i<num_pending_lit; i++){
		Lit* pending_lit = pending_list[i];
		if(pending_lit->sindex <0){
			pending_lit->LitValue = 0;
			pending_lit->LitState = 1;
		}
		else if(pending_lit->sindex >0){
			pending_lit->LitValue = 1;
			pending_lit->LitState = 1;
		}
	}

	Lit** decisions = sat_state->decisions;
	//now I have the current decision and the pending list
	// instead of merging the two list in one list just pass by each one
	for(unsigned long i =0; i<sat_state->num_literals_in_decision; i++){
#ifdef DEBUG
		printf("In the decision literals loop: current literal %ld \n", decisions[i]->sindex);
#endif
		Var* corresponding_var = sat_literal_var(decisions[i]);
		unsigned long* decision_clause_list = corresponding_var->list_clause_of_variables;


		for(unsigned long j=0; j<corresponding_var->num_of_clauses_of_variables; j++){

			Clause* clause = sat_index2clause(decision_clause_list[j], sat_state);

			//check clause
			for(unsigned long k = 0; k<clause->num_literals_in_clause;k++){
				//check if the literal is asserted or not yet set
				if(sat_is_asserted_literal(clause->literals[k]) || (!sat_implied_literal(clause->literals[k]) )){
					break; //don't check the other literals of the clause
				}
				else if(k== clause->num_literals_in_clause-1){ // I reached the end of the loop with no break (contradicted clause)
					fails = 1;
					sat_state->conflict_clause = clause;
					// reset the pending list
					for(unsigned long i =0; i<num_pending_lit; i++){
						Lit* pending_lit = pending_list[i];
						pending_lit->LitValue = 'u';
						pending_lit->LitState = 0;
					}
					return fails;
				}
			} // for all literals in a clause
		} // for all clauses for this variable
	}// for all variables in decision list


	//repeat the same for pending list
	for(unsigned long i =0; i< num_pending_lit; i++){
#ifdef DEBUG
		printf("In the pending literals loop: current literal %ld \n", pending_list[i]->sindex);
#endif
		Var* corresponding_var = sat_literal_var(pending_list[i]);
		unsigned long* pending_clause_list = corresponding_var->list_clause_of_variables;

		for(unsigned long j=0; j<corresponding_var->num_of_clauses_of_variables; j++){

			Clause* clause = sat_index2clause(pending_clause_list[j], sat_state);

			//check clause
			for(unsigned long k = 0; k<clause->num_literals_in_clause;k++){
				//check if the literal is asserted or not yet set
				if(sat_is_asserted_literal(clause->literals[k]) || (!sat_implied_literal(clause->literals[k]) )){
					break; //don't check the other literals of the clause
				}
				else if(k== clause->num_literals_in_clause-1){ // I reached the end of the loop with no break (contradicted clause)
					fails = 1;
					sat_state->conflict_clause = clause;
					// reset the pending list
					for(unsigned long i =0; i<num_pending_lit; i++){
						Lit* pending_lit = pending_list[i];
						pending_lit->LitValue = 'u';
						pending_lit->LitState = 0;
					}
					return fails;

				}
			} // for all literals in a clause
		} // for all clauses for this variable
	}// for all variables in pending list


//TODO: check do i have to unset all the literals in the pending list
#ifdef DEBUG
	if(fails == 1){
		printf("Contradiction happens in evaluating clauses in the pending list\n");
	}
#endif

	return fails;
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

static void add_literal_to_list(Lit*** list, Lit* lit, unsigned long* capacity, unsigned long* num_elements){

	unsigned long cap = *capacity;
	unsigned long num = *num_elements;

	if(num >= cap){
		// needs to realloc the size
		cap =num * MALLOC_GROWTH_RATE;
		*list = (Lit**) realloc(*list , sizeof(Lit*) * cap);
	}

		(*list)[num++] = lit;

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
	Two literal watch algorithm for unit resolution
The algorithm taken from the Class Notes for CS264A, UCLA

******************************************************************************/
BOOLEAN two_literal_watch(SatState* sat_state, Lit*** literals_list, unsigned long * num_elements, unsigned long * capacity){

	// Once I entered here I must have elements in the decision array
	assert(sat_state->num_literals_in_decision > 0);

	// initialize the list of watched clauses already initialized in ParseDIMACS
//	if(!INIT_LITERAL_WATCH)
//		intitialize_watching_clauses(sat_state);

	BOOLEAN contradiction_flag = 0;

	//TODO: Due to recursion of pending list we may need to consider more than one decided literal then we need a list that captures all literals of last decision

	//loop on all decided literals

	// Lit** literals_in_decision = literals_list;
	// unsigned long max_size_decision_list = capacity;
	// unsigned long num_decision_lit = num_elements;


	// Create pending literal list
	Lit** pending_list = (Lit**)malloc(sizeof(Lit*));
	unsigned long max_size_pending_list = 1;
	unsigned long num_pending_lit = 0;

	for(unsigned long i =0; i< *num_elements; i++){
#ifdef DEBUG
		printf("--------------------------------------\n");
		printf("Decision list now in two literal watch: ");
		for(unsigned long j =0; j<  *num_elements;j++){
			printf("%ld\t", (*literals_list)[j]->sindex );
		}
		printf("\n");
#endif
		//Lit* decided_literal = sat_state->decisions[sat_state->num_literals_in_decision -1];
		Lit* decided_literal = (*literals_list)[i];
		Lit* resolved_literal = get_resolved_lit(decided_literal, sat_state);

#ifdef DEBUG
		printf("Resolved Literal: %ld\n", resolved_literal->sindex);
#endif

		//TODO: Update clauses state based on the decided literal
		sat_update_clauses_state(decided_literal , sat_state);

		//If no watching clauses on the resolved literal then do nothing and record the decision
		if(resolved_literal->num_watched_clauses == 0){
			// The decided literal is already in the decision list so no need to record it again
#ifdef DEBUG
			printf("Number of watching clause on literal %ld is %ld\n",resolved_literal->sindex,resolved_literal->num_watched_clauses );
#endif
			//wait for a new decision
			continue;
		}
		else{
			//Get the watched clauses for the resolved literal
			for(unsigned long k = 0; k < resolved_literal->num_watched_clauses && contradiction_flag == 0; k++){
					//check dirty flag
					if(resolved_literal->list_of_dirty_watched_clauses[k] == 0) continue;

					Clause* wclause = sat_index2clause( resolved_literal->list_of_watched_clauses[k], sat_state);
					//TODO: Check if this actually works to skip the subsumed clauses
					//TODO: Enhance: we can only get the watched clauses that are not subsumed to speed it up and to avoid checking the unit clause
					if(wclause->is_subsumed)
						continue;

					unsigned long num_free_literals = 0;

					// I have to check the other watched literal
					Lit* the_other_watched_literal = NULL;
					if(resolved_literal->sindex == wclause->L1->sindex)
						the_other_watched_literal = wclause->L2;
					else if(resolved_literal->sindex == wclause->L2->sindex)
						the_other_watched_literal = wclause->L1;


					for(unsigned long l = 0; l < wclause->num_literals_in_clause; l++){
						//check for not resolved literal = free or asserted
						if( !sat_is_resolved_literal(wclause->literals[l])  && wclause->literals[l]->sindex != the_other_watched_literal->sindex  ) {
							num_free_literals++;
						}
					}

					if (num_free_literals == 0){ //3cases

						//contradiction --> everything is resolved
						//subsumed clause --> do nothing
						//implication --> free literal
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
						else if (the_other_watched_literal->LitState == 0){
							// implication

							if(sat_literal_var(the_other_watched_literal)->antecedent == NULL){

								sat_literal_var(the_other_watched_literal)->antecedent = wclause;  // remember the decision list is updated with the pending list so that's ok
#ifdef DEBUG
								printf("free literal %ld\n",the_other_watched_literal->sindex);
								//print_clause(wclause);
								printf("Antecedent: \n");
								print_clause(sat_literal_var(the_other_watched_literal)->antecedent);
#endif
								add_literal_to_list(&pending_list,  the_other_watched_literal , &max_size_pending_list, &num_pending_lit);
#ifdef DEBUG
								printf("Add the free literal %ld to the pending list with antecedent %ld\n",the_other_watched_literal->sindex, (sat_literal_var(the_other_watched_literal)->antecedent)->cindex );
#endif
								continue; // go to the next clause
							}
							else { //this means I implied the opposite (contradiction) or the same literal again(redundancy) in another clause
								//TODO: first check if pending list contains the same literal, if yes then just continue to the next clause else contradiction
								BOOLEAN lit_is_already_in_pending = 0;
								for(unsigned long pendelem =0; pendelem<num_pending_lit; pendelem ++){
									if (pending_list[pendelem]->sindex == the_other_watched_literal->sindex)
										lit_is_already_in_pending =1;
								}
								if(lit_is_already_in_pending == 1)
									continue; //go to the next clause
								else{
									contradiction_flag= 1;
#ifdef DEBUG
									printf("free literal %ld\n",the_other_watched_literal->sindex);
									printf("-------------Contradiction(opposite implication) happens with clause: %ld\n",wclause->cindex);
#endif
									sat_state->conflict_clause = wclause;
									break; //break from loop of watched clauses over this decided literal
								}
							}
						} //end of the implication case
					} // end of the three cases

					else if (num_free_literals > 0) { // find another literal to watch that is free
						for(unsigned long z = 0; z < wclause->num_literals_in_clause; z++){
							if((!sat_is_resolved_literal(wclause->literals[z])) && wclause->literals[z] != wclause->L1 && wclause->literals[z] != wclause->L2){
								Lit* new_watched_lit = wclause->literals[z];
								//remove_watching_clause(k, resolved_literal);
								resolved_literal->list_of_dirty_watched_clauses[k] = 0;
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
					} //end of else number of free literals > 0

			} //end of for for watched clauses over this decided literal

			//SALMA removed and the contradiction !=1
			if(num_pending_lit >0){
			// Pass over the pending list and add the literals to the decision while maintaining the level
				for(unsigned long i =0; i < num_pending_lit; i++){
					//evaluate the pending list to make sure we didn't miss a chance of contraction // if the contradiction flag is raised then the conflict clause is already set in this function
					//BOOLEAN fails = evaluate_delta(sat_state, pending_list, i+1); //up till this pending literal


					// pending literal was an already watched clause so the associated list of watching clauses is already handled
					// fix values of pending literal before putting in decision
						Lit* pending_lit = pending_list[i];

						//TODO: the decision level should be the same as the max level of the antecedent not the current level.
						//pending_lit->decision_level =  sat_state->current_decision_level; // this is so wrong ... this will affect the UIP learning at contradiction because of stopping condition
						unsigned long pending_lit_decision =0;
						Var* pending_var = sat_literal_var(pending_lit);
						Clause* pending_antecedent = pending_var->antecedent;

						for(unsigned long indxant =0; indxant < pending_antecedent->num_literals_in_clause; indxant++ ){
							Var* current_var = sat_literal_var(pending_antecedent->literals[indxant]);
							if(current_var->index == pending_var->index)
								continue; //skip the var
							else if(pending_antecedent->literals[indxant]->decision_level > pending_lit_decision)
								pending_lit_decision = pending_antecedent->literals[indxant]->decision_level;
						}

						pending_lit->decision_level = pending_lit_decision;


						//TODO: this part is already done in evaluate delta
						if(pending_lit->sindex <0){
							pending_lit->LitValue = 0;
							pending_lit->LitState = 1;
							Lit* opposite_lit = sat_literal_var(pending_lit)->posLit;
							opposite_lit->LitValue = 0;
							opposite_lit->LitState = 1;
							opposite_lit->decision_level = pending_lit->decision_level;
						}
						else if(pending_lit->sindex >0){
							pending_lit->LitValue = 1;
							pending_lit->LitState = 1;
							Lit* opposite_lit = sat_literal_var(pending_lit)->negLit;
							opposite_lit->LitValue = 1;
							opposite_lit->LitState = 1;
							opposite_lit->decision_level = pending_lit->decision_level;
						}

	//					for(unsigned long i = 0; i< sat_literal_var(pending_lit)->num_of_clauses_of_variables; i++){
	//						Clause* clause = sat_literal_var(pending_lit)->list_clause_of_variables[i];
	//
	//					}



						//update the decision list
						sat_state->decisions[sat_state->num_literals_in_decision++] = pending_lit;

						//update list of literals in last decision because this is the main loop

						if (*num_elements >= *capacity)
						{
							*capacity *= MALLOC_GROWTH_RATE;
							*literals_list = realloc(*literals_list, sizeof(Lit * ) * (*capacity));
						}

						(*literals_list)[(*num_elements)++] = pending_lit;

//					if (fails == 1){
//						contradiction_flag = 1;
//						break;
//					}
				}
//				//free pending list for the next round
//				if(pending_list != NULL && num_pending_lit != 0)
//					FREE(pending_list);

			} // pending_list > 1


		}//Get the watched clauses for the resolved literal

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



