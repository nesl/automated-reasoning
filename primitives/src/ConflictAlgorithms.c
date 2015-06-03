/*
 * ConflictAlgorithms.c
 *
 *  Created on: May 22, 2015
 *      Author: salma
 */

#include "ConflictAlgorithms.h"
#include "LiteralWatch.h"
#include "ParseDIMACS.h" //for the add_watching_clause api
#include "global.h"
#include <stdlib.h>



//static int comp (const void * elem1, const void * elem2)
//{
//    int f = *((unsigned long*)elem1);
//    int s = *((unsigned long*)elem2);
//    if (f > s) return  1;
//    if (f < s) return -1;
//    return 0;
//}
static BOOLEAN are_equivalent_clauses(Clause* c1, Clause* c2){
	if(c1->num_literals_in_clause != c2->num_literals_in_clause)
		return 0;

	//TODO sort the two clauses first before checking (in place)
	//qsort (*c1->literals, c1->num_literals_in_clause,  sizeof(unsigned long), comp);
	//qsort (*c2->literals, c2->num_literals_in_clause,  sizeof(unsigned long), comp);

	for(unsigned long i =0; i < c1->num_literals_in_clause; i++ ){
		if(c1->literals[i]->sindex != c2->literals[i]->sindex)
			return 0;
	}

	return 1;
}




static BOOLEAN is_predicate_hold(Lit* lit, SatState* sat_state){
	BOOLEAN predicate_value =0;
	// not a decision TODO: what if the UIP was the decision!?
	//SALMA: I changed this
#ifdef DEBUG
	printf("Is predicate hold for literal: %ld, at decision: %ld  \t",lit->sindex, lit->decision_level );
#endif
	if((lit->decision_level == sat_state->current_decision_level) && (sat_literal_var(lit)->antecedent != NULL)){
	//if((lit->decision_level == sat_state->current_decision_level) && lit->antecedent != NULL)
		predicate_value = 1;
#ifdef DEBUG
	printf("with antecedent = %ld  \t", sat_literal_var(lit)->antecedent->cindex);
#endif
	}

#ifdef DEBUG
	printf("%ld\n",predicate_value);
#endif
	return predicate_value;
}



// get the last falsified literal in the current decision level and not NULL antecedent
static Lit* last_falsified_literal_with_predicate(Clause* clause, SatState* sat_state){
	Lit* last_lit = NULL;
	unsigned long index_last_literal = 0;
	for(unsigned long i =0; i< clause->num_literals_in_clause; i++){
		Lit* cur_falsified_literal = clause->literals[i];
		for(unsigned long j=0; j< sat_state->num_literals_in_decision; j++){
			if(sat_literal_var(cur_falsified_literal)->index == sat_literal_var(sat_state->decisions[j])->index){
				//check predicate here as well
				if(is_predicate_hold(cur_falsified_literal, sat_state)){
					if(j >= index_last_literal)
						index_last_literal = j;
					else
						continue;
				}
			}
		}
	}

	last_lit = sat_state->decisions[index_last_literal];

	return last_lit;
}

//update the clause list of the variable
static void update_variable_and_literal_list(Clause* clause){
	for(unsigned long i = 0; i< clause->num_literals_in_clause; i++){
		add_clause_to_literal(clause->literals[i], clause);

		Var* var = sat_literal_var(clause->literals[i]);
		add_clause_to_variable(var, clause);

		//		if(var->num_of_clauses_of_variables >= var->max_size_list_of_clause_of_variables){
		//
		//			var->max_size_list_of_clause_of_variables =(var->num_of_clauses_of_variables * MALLOC_GROWTH_RATE);
		//			var->list_clause_of_variables = (Clause**) realloc( var->list_clause_of_variables, sizeof(Clause) * (var->max_size_list_of_clause_of_variables));
		//		}
		//
		//		var->list_clause_of_variables[var->num_of_clauses_of_variables++] = clause;
	}
}


static void update_delta_with_alpha(SatState* sat_state){

	if(sat_state->num_clauses_in_delta >= sat_state->max_size_list_delta){
			// needs to realloc the size
			sat_state->max_size_list_delta =(sat_state->num_clauses_in_delta * MALLOC_GROWTH_RATE);
			sat_state->delta = (Clause*) realloc( sat_state->delta, sizeof(Clause) * (sat_state->max_size_list_delta));
	}
	sat_state->delta[sat_state->num_clauses_in_delta] = *(sat_state->alpha); // copy the actual value

	// update the watching literals L1 and L2 be careful that a learnt clause can be a unit clause
	if(sat_state->delta[sat_state->num_clauses_in_delta].num_literals_in_clause > 1){
		sat_state->delta[sat_state->num_clauses_in_delta].L1 = sat_state->delta[sat_state->num_clauses_in_delta].literals[0];
		sat_state->delta[sat_state->num_clauses_in_delta].L2 = sat_state->delta[sat_state->num_clauses_in_delta].literals[1];
		//update the state of the literals with that new watching clause
		add_watching_clause(&sat_state->delta[sat_state->num_clauses_in_delta], sat_state->delta[sat_state->num_clauses_in_delta].L1);
		add_watching_clause(&sat_state->delta[sat_state->num_clauses_in_delta], sat_state->delta[sat_state->num_clauses_in_delta].L2);
	}
	else{ //unit clause
		sat_state->delta[sat_state->num_clauses_in_delta].L1 = sat_state->delta[sat_state->num_clauses_in_delta].literals[0];
		sat_state->delta[sat_state->num_clauses_in_delta].L2 = NULL;
		//update the state of the literals with that new watching clause
		add_watching_clause(&sat_state->delta[sat_state->num_clauses_in_delta], sat_state->delta[sat_state->num_clauses_in_delta].L1);
	}

#ifdef DEBUG
	printf("Update the watched literals of new clause\n");
#endif

	// update the index of the new added clause
	sat_state->delta[sat_state->num_clauses_in_delta].cindex = sat_state->num_clauses_in_delta; //- 1 +1 ; // remember the indices are from 0 to n-1

#ifdef DEBUG
	printf("Index of the new clause: %ld\n",sat_state->delta[sat_state->num_clauses_in_delta].cindex);
#endif

	update_variable_and_literal_list(&sat_state->delta[sat_state->num_clauses_in_delta]);

	if(sat_state->num_clauses_in_gamma >= sat_state->max_size_list_gamma){
		// needs to realloc the size
		sat_state->max_size_list_gamma =(sat_state->num_clauses_in_gamma * MALLOC_GROWTH_RATE);
		sat_state->gamma = (unsigned long*) realloc( sat_state->gamma, sizeof(unsigned long) * (sat_state->max_size_list_gamma));
	}
	sat_state->gamma[sat_state->num_clauses_in_gamma++] = sat_state->delta[sat_state->num_clauses_in_delta].cindex;

	//update number of clauses in delta
	sat_state->num_clauses_in_delta++;

	#ifdef DEBUG
		printf("Number of literals of the new learnt clause %ld is %ld\n", sat_index2clause(sat_state->gamma[sat_state->num_clauses_in_gamma -1], sat_state)->cindex, sat_index2clause(sat_state->gamma[sat_state->num_clauses_in_gamma -1], sat_state)->num_literals_in_clause  );
	#endif

	//TODO: Should I clear alpha here? Yes I should
	sat_state->alpha = NULL;

#ifdef DEBUG
	printf("Cleared alpha\n");
	print_all_clauses(sat_state);
#endif
}


/***** Gamma is not used afterwards this is just for debugging and testing of performance so actually Gamma keeps a copy of its own clauses away from delta*/
void add_clause_to_gamma(SatState* sat_state){
	update_delta_with_alpha(sat_state);
}

static void add_literal_to_clause(Lit* lit, Clause* clause){

//first I have to check if this literal is already in the clause then don't re-add it
	for(unsigned long i =0; i< clause->num_literals_in_clause; i++){
		if(lit->sindex == clause->literals[i]->sindex)
			return;
	}

	if(clause->num_literals_in_clause >= clause->max_size_list_literals){
		// needs to realloc the size
		clause->max_size_list_literals = clause->num_literals_in_clause * MALLOC_GROWTH_RATE;
		clause->literals = (Lit**) realloc( clause->literals, sizeof(Lit*) * clause->max_size_list_literals);
	}

	clause->literals[clause->num_literals_in_clause++] = lit;
}

static BOOLEAN check_element_of_resolvent_list(Var* var, Var** resolvent_list, unsigned long size){
	BOOLEAN skip = 0;

	for(unsigned long i =0; i< size; i++){
		if(var->index == resolvent_list[i]->index){
			skip = 1;
			break;
		}
	}
	return skip;
}

// Resolution between two clauses
static void resolution(Clause* alpha_l, Clause* wl_1, Clause* wl){
	//alpha_l = {~A, ~X, ~Y }
	//wl_1 = {~A, Y, ~Z}
	// I should expect wl = {~A, ~X, ~Z}

	// algorithm
	//get the resolvent first then add all the literals of alpha_l and wl_1 to wl unless it is equal to the resolvent
#ifdef DEBUG
	printf("BEGIN Clause Resolution between clauses: \n");
	print_clause(alpha_l);
	print_clause(wl_1);

	//BOOLEAN are_equ = are_equivalent_clauses(alpha_l, wl_1);
	//assert(are_equ != 1);
#endif


	Var** resolvent_var_list = (Var**)malloc(sizeof(Var*));
	unsigned long num_of_var_in_list = 0;
	unsigned long max_size_of_var_in_list = 1;

	for(unsigned long i =0 ; i< alpha_l->num_literals_in_clause; i++){
		Lit* alit = alpha_l->literals[i];
		for(unsigned long j = 0; j< wl_1->num_literals_in_clause; j++){
			Lit* wlit = wl_1->literals[j];
			//check if they are of opposite sign --> i.e. a resolvent
			if(sat_literal_var(alit)->index == sat_literal_var(wlit)->index){
				if((alit->sindex > 0 && wlit->sindex < 0) || (alit->sindex < 0 && wlit->sindex > 0)){
					Var* added_var = sat_literal_var(alit); // or wlit it doesn't matter they are the same
#ifdef DEBUG
					printf("resolvent variable added: %ld\n", added_var->index);
#endif
					//add added_var to the list
					if(num_of_var_in_list >= max_size_of_var_in_list){
						max_size_of_var_in_list = num_of_var_in_list * MALLOC_GROWTH_RATE;
						resolvent_var_list = (Var**)realloc(resolvent_var_list, sizeof(Var*) * max_size_of_var_in_list );
					}
					resolvent_var_list[num_of_var_in_list++] = added_var;
				}
			}
		}
	}

#ifdef DEBUG
	printf("Resolvent variable list is ready with %ld variables in it\n Debugging the variable list:\n", num_of_var_in_list);
	for(unsigned long i =0; i < num_of_var_in_list;i++){
		printf("%ld\t", resolvent_var_list[i]->index);
	}
	printf("\n");
#endif
	// at the end of the loop I should have all the resolvent variable ready
	// go element by element in alpha_l and wl_1 and check if this element is one of the resolvent
	//Check alpha_l first
	for(unsigned long i =0 ; i< alpha_l->num_literals_in_clause; i++){
		Lit* alit = alpha_l->literals[i];

		BOOLEAN skip = check_element_of_resolvent_list(sat_literal_var(alit), resolvent_var_list, num_of_var_in_list);
		if(!skip){
			add_literal_to_clause(alit, wl);
		}
	}
	//check wl_1 as well
	for(unsigned long i =0 ; i< wl_1->num_literals_in_clause; i++){
		Lit* wlit = wl_1->literals[i];

		BOOLEAN skip = check_element_of_resolvent_list(sat_literal_var(wlit), resolvent_var_list, num_of_var_in_list);
		if(!skip){
			add_literal_to_clause(wlit, wl);
		}
	}

	// at the end deallocate the Var**
	// TODO: do I need deep clean here?
	FREE(resolvent_var_list);

}

static void initialize_learning_clause(Clause* wl_1, Clause* conflict_clause){

	 wl_1->cindex = conflict_clause->cindex;
	 wl_1->is_subsumed = conflict_clause->is_subsumed;
	 wl_1->mark = conflict_clause->mark;
	 wl_1->max_size_list_literals = conflict_clause->max_size_list_literals;
	 wl_1->num_literals_in_clause = conflict_clause->num_literals_in_clause;

	 if(conflict_clause->L1 != NULL)
		 wl_1->L1 = conflict_clause->L1;
	 else wl_1->L1 = NULL;

	 if(conflict_clause->L2 != NULL)
		 wl_1->L2 = conflict_clause->L2;
	 else wl_1->L2 = NULL;


	 for(unsigned long i =0; i< conflict_clause->num_literals_in_clause; i++){
		wl_1->literals[i] = conflict_clause->literals[i];
	 }

}


//static void unit_resolution(Clause* clause, Clause* unit_clause, Clause* newclause){
//	// get the literal from the unit clause
//	Lit* lit = unit_clause->literals[0];
//
//	for(unsigned long i =0; i< clause->max_size_list_literals; i++){
//		if (clause->literals[i]->sindex == lit->num_watched_clauses)
//			continue;
//		else
//			add_literal_to_clause(lit, newclause);
//	}
//}


static BOOLEAN is_termination_condition_hold(Clause* wl, SatState* sat_state, BOOLEAN use_UIP){
	BOOLEAN reached = 0;

	if(use_UIP){
		unsigned long count_asserting_literals = 0;
		for(unsigned long i =0; i < wl->num_literals_in_clause; i++){
			Lit* lit = wl->literals[i];
			if(lit->decision_level == sat_state->current_decision_level){
				count_asserting_literals++;
#ifdef DEBUG
				printf("Checking the termination condition\t");
				printf("lit decision level: %ld, current decision level: %ld\n",lit->decision_level,sat_state->current_decision_level);
#endif
			}
		}

#ifdef DEBUG
		printf("count asserting literals: %ld\n",count_asserting_literals);
#endif
		if(count_asserting_literals == 1) // is indeed an asserting clause // only one literal at the current decision level
			reached = 1;

		return reached;
	}
	else{
		for(unsigned long i =0; i< wl->num_literals_in_clause; i++){
			Lit* lit = wl->literals[i];
			if(is_predicate_hold(lit, sat_state))
				return 0;
			else
				continue;
		}
		//reached end of loop and no return then predicate does not hold for all literals then reached = 1
		return 1;
	}
}

static void learn_trivial_clause(Clause* wl, SatState* sat_state){

	//TODO: check the stopping condition. I stopped at the before last in decision because u usually have double opposite assignment
	for(unsigned long i =0; i < sat_state->num_literals_in_decision; i++){
		Lit* lit = sat_state->decisions[i];
		Lit* addedlit = NULL;
		if(lit->sindex > 0)
			addedlit = (sat_literal_var(lit))->negLit;
		else if (lit->sindex < 0)
			addedlit = (sat_literal_var(lit))->posLit;


		add_literal_to_clause(addedlit, wl);
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
	wl->cindex = sat_state->num_clauses_in_delta;// - 1 + 1; // the learnt clause will have the next index in delta (remember it is from 0 to n-1)
	wl->L1 = NULL;
	wl->L2 = NULL;

	Clause* wl_1 = (Clause*) malloc(sizeof(Clause));
	wl_1->literals = (Lit**) malloc(sizeof(Lit*) * (sat_state->conflict_clause->num_literals_in_clause)); // because at the beginning wl-1 is copied with conflict clause

#ifdef DEBUG
	printf("Initialize wl_1 with %ld\n",sat_state->conflict_clause->cindex );
#endif
	//initially wl_1 = conflict clause //deep copy
	initialize_learning_clause(wl_1, sat_state->conflict_clause);
#ifdef DEBUG
	printf("finish initializing wl_1 with %ld\n",sat_state->conflict_clause->cindex );
#endif
	//fixed point
	BOOLEAN fixed_point_achieved = 0;

	while(fixed_point_achieved == 0){
		Clause* alpha_l = NULL;
//		for(unsigned long i=0; i< wl_1->num_literals_in_clause; i++){
//			Lit* lit = wl_1->literals[i];
//#ifdef DEBUG
//	printf("examine first literal in wl_1 %ld\n",lit->sindex );
//#endif
//	//TODO: Put here a code to call function last_falsified_literal however, the last falsified literal is not necessarily be at the last decision level!!! because of undo decide literal
//	// TODO: how to solve it!!!
//			if(is_predicate_hold(lit, sat_state)){
//				alpha_l = sat_literal_var(lit)->antecedent; // alpha_l will evaluate to a unit clause in the current setting
//				//SALMA: I changed this
//				//alpha_l = lit->antecedent;
//				//break; // break for
//				//maybe continue
//
//				//SALMA: here maybe check whether this lit is falsified last in this clause or not. if yes then break else continue
//
//				continue;
//			}
//		}

		Lit* lit = last_falsified_literal_with_predicate(wl_1,sat_state);
		if(lit != NULL)
			alpha_l = sat_literal_var(lit)->antecedent;

// added a part in the algorithm to compromise for the the non backtracking to the assertion level
		if(alpha_l != NULL){
			// resolve the conflict clause with the asserted variable antecedent and update the conflict clause (wl)
			// for the next loop
#ifdef DEBUG
			printf("alpha_l != NULL\n");
#endif
			resolution(alpha_l, wl_1, wl);
		}
		else{
#ifdef DEBUG
			printf("alpha == NULL");
#endif
			learn_trivial_clause(wl, sat_state);
		}


#ifdef DEBUG
		printf("Resolution of two clauses led to a new clause\n");
		printf("The clause consists of: ");
		for(unsigned long i =0; i< wl->num_literals_in_clause; i++){
			printf("%ld\t",wl->literals[i]->sindex);
		}
		printf("\n");
#endif

	//	check fixed point
		if(are_equivalent_clauses(wl, wl_1)){
			fixed_point_achieved =1;
		}
		else
		if(is_termination_condition_hold(wl, sat_state, USE_UIP_LEARNT_CLAUSE) == 1) // is indeed an asserting clause // only one literal at the current decision level
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
	} //end of while

	sat_state->alpha = wl;

	//TODO: I have to clean wl and wl-1 somewhere

	return wl;
}
