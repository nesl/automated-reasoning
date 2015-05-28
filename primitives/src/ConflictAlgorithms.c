/*
 * ConflictAlgorithms.c
 *
 *  Created on: May 22, 2015
 *      Author: salma
 */

#include "ConflictAlgorithms.h"
#include "LiteralWatch.h"
#include "global.h"



//update the clause list of the variable
static void update_variable_list(Clause* clause){
	for(unsigned long i = 0; i< clause->num_literals_in_clause; i++){
		Var* var = sat_literal_var(clause->literals[i]);
		if(var->num_of_clauses_of_variables >= var->max_size_list_of_clause_of_variables){

			var->max_size_list_of_clause_of_variables =(var->num_of_clauses_of_variables * MALLOC_GROWTH_RATE);
			var->list_clause_of_variables = (Clause**) realloc( var->list_clause_of_variables, sizeof(Clause) * (var->max_size_list_of_clause_of_variables));
		}

		var->list_clause_of_variables[var->num_of_clauses_of_variables++] = clause;

	}
}

static void update_delta_with_alpha(SatState* sat_state){

#ifdef DEBUG
	printf("Add gamma to delta: number of clauses in delta: %ld\n",sat_state->num_clauses_in_delta);
#endif
	if(sat_state->num_clauses_in_delta >= sat_state->max_size_list_delta){
			// needs to realloc the size
			sat_state->max_size_list_delta =(sat_state->num_clauses_in_delta * MALLOC_GROWTH_RATE);
			sat_state->delta = (Clause*) realloc( sat_state->delta, sizeof(Clause) * (sat_state->max_size_list_delta));
	}

	sat_state->delta[sat_state->num_clauses_in_delta] = *(sat_state->alpha); // copy the actual value

	//TODO: Should I clear alpha here? Yes I should
	sat_state->alpha = NULL;

#ifdef DEBUG
	printf("Cleared alpha\n");
#endif

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

	//update the clause list of the variable
	update_variable_list(&sat_state->delta[sat_state->num_clauses_in_delta]);

	//update number of clauses in delta
	sat_state->num_clauses_in_delta++;


#ifdef DEBUG
	printf("Number of literals of the new learnt clause %ld is %ld\n", sat_state->delta[sat_state->num_clauses_in_delta].cindex, sat_state->delta[sat_state->num_clauses_in_delta].num_literals_in_clause);
#endif

}


/***** Gamma is not used afterwards this is just for debugging and testing of performance so actually Gamma keeps a copy of its own clauses away from delta*/
void add_clause_to_gamma(SatState* sat_state){
	if(sat_state->num_clauses_in_gamma >= sat_state->max_size_list_gamma){
		// needs to realloc the size
		sat_state->max_size_list_gamma =(sat_state->num_clauses_in_gamma * MALLOC_GROWTH_RATE);
		sat_state->gamma = (Clause*) realloc( sat_state->gamma, sizeof(Clause) * (sat_state->max_size_list_gamma));
	}
	sat_state->gamma[sat_state->num_clauses_in_gamma++] = *(sat_state->alpha);


// update the watching literals L1 and L2 be careful that a learnt clause can be a unit clause
	if(sat_state->gamma[sat_state->num_clauses_in_gamma].num_literals_in_clause > 1){
		sat_state->gamma[sat_state->num_clauses_in_gamma].L1 = sat_state->gamma[sat_state->num_clauses_in_gamma].literals[0];
		sat_state->gamma[sat_state->num_clauses_in_gamma].L2 = sat_state->gamma[sat_state->num_clauses_in_gamma].literals[1];
		//update the state of the literals with that new watching clause
		add_watching_clause(&sat_state->gamma[sat_state->num_clauses_in_gamma], sat_state->gamma[sat_state->num_clauses_in_gamma].L1);
		add_watching_clause(&sat_state->gamma[sat_state->num_clauses_in_gamma], sat_state->gamma[sat_state->num_clauses_in_gamma].L2);
	}
	else{ //unit clause
		sat_state->gamma[sat_state->num_clauses_in_gamma].L1 = sat_state->gamma[sat_state->num_clauses_in_gamma].literals[0];
		sat_state->gamma[sat_state->num_clauses_in_gamma].L2 = NULL;
		//update the state of the literals with that new watching clause
		add_watching_clause(&sat_state->gamma[sat_state->num_clauses_in_gamma], sat_state->gamma[sat_state->num_clauses_in_gamma].L1);
	}

#ifdef DEBUG
	printf("Update the watched literals of new clause\n");
#endif

	// update the index of the new added clause
	sat_state->gamma[sat_state->num_clauses_in_gamma].cindex = sat_state->num_clauses_in_gamma; //- 1 +1 ; // remember the indices are from 0 to n-1

#ifdef DEBUG
	printf("Index of the new clause: %ld\n",sat_state->gamma[sat_state->num_clauses_in_gamma].cindex);
#endif

	//update the clause list of the variable
	update_variable_list(&sat_state->gamma[sat_state->num_clauses_in_gamma]);

	//update number of clauses in delta
	sat_state->num_clauses_in_gamma++;


	#ifdef DEBUG
		printf("Number of literals of the new learnt clause %ld is %ld\n", sat_state->gamma[sat_state->num_clauses_in_gamma].cindex, sat_state->gamma[sat_state->num_clauses_in_gamma].num_literals_in_clause);
	#endif


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

	//TODO: actually it can be more than one resolvent it can be a list
#ifdef DEBUG
	printf("BEGIN Clause Resolution between clauses: \n");
	print_clause(alpha_l);
	print_clause(wl_1);
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
//	 wl_1->L1 = conflict_clause->L1;
//	 wl_1->L2 = conflict_clause->L2; // this is not correct because the learning clause can be unit clause
	 wl_1->max_size_list_literals = conflict_clause->max_size_list_literals;
	 wl_1->num_literals_in_clause = conflict_clause->num_literals_in_clause;
	 wl_1->L1 = NULL; //TODO: not fixed for unit learnt clause!
	 wl_1->L2 = NULL;

	 for(unsigned long i =0; i< conflict_clause->num_literals_in_clause; i++){
		wl_1->literals[i] = conflict_clause->literals[i];
	 }

}

static BOOLEAN is_predicate_hold(Lit* lit, SatState* sat_state){
	BOOLEAN predicate_value =0;
	// not a decision TODO: what if the UIP was the decision!?
	if((lit->decision_level == sat_state->current_decision_level) && (sat_literal_var(lit)->antecedent != NULL))
		predicate_value = 1;


	return predicate_value;
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


static BOOLEAN is_fixed_point_reached(Clause* wl, SatState* sat_state, BOOLEAN use_UIP){
	BOOLEAN reached = 0;

	if(use_UIP){
		unsigned long count_asserting_literals = 0;
		for(unsigned long i =0; i < wl->num_literals_in_clause; i++){
			Lit* lit = wl->literals[i];
			if(lit->decision_level == sat_state->current_decision_level){
				count_asserting_literals++;
			}
		}
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
			if(is_predicate_hold(lit, sat_state)){
				alpha_l = sat_literal_var(lit)->antecedent; // alpha_l will always be a unit clause
				//break; // break for
				//maybe continue
				continue;
			}
		}
		// resolve the conflict clause with the asserted variable antecedent and update the conflict clause (wl)
		// for the next loop
		resolution(alpha_l, wl_1, wl);

#ifdef DEBUG
		printf("Resolution of two clauses led to a new clause\n");
		printf("The clause consists of: \n");
		print_clause(wl);
#endif
		if(is_fixed_point_reached(wl, sat_state, USE_UIP_LEARNT_CLAUSE) == 1) // is indeed an asserting clause // only one literal at the current decision level
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
