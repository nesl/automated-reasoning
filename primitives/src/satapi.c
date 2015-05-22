/*
 * satapi.c
 *
 *  Created on:  Apr 30, 2015
 *      Author: salma
 */


#include "satapi.h"
#include <fcntl.h>
#include <assert.h>
#include "ParseDIMACS.h"
#include "LiteralWatch.h"
#include "VSIDS.h"


/* GLOBALS */
BOOLEAN FLAG_CASE1_UNIT_RESOLUTION = 1;
BOOLEAN FLAG_CASE2_UNIT_RESOLUTION = 0;
BOOLEAN FLAG_CASE3_UNIT_RESOLUTION = 0;


/******************************************************************************
 * We explain here the functions you need to implement
 *
 * Rules:
 * --You cannot change any parts of the function signatures
 * --You can/should define auxiliary functions to help implementation
 * --You can implement the functions in different files if you wish
 * --That is, you do not need to put everything in a single file
 * --You should carefully read the descriptions and must follow each requirement
 ******************************************************************************/


/******************************************************************************
 * Given a variable index i, you should return the corresponding variable
 * structure (notice you must return a pointer to the structure)
 *
 * Note variable indices range from 1 to n where n is the number of variables
 ******************************************************************************/
Var* index2varp(unsigned long i, SatState* sat_state) {

  return &sat_state->variables[i];
}


/******************************************************************************
 * Given a variable var, you should return
 * --its positive literal (pos_literal) 
 * --its negative literal (neg_literal) 
 *
 *
 * Given a literal lit, set_literal(lit) should return 
 * --1 if lit is set in the current setting
 * --0 if lit is free 
 *
 * Note a literal is set either by a decision or implication
 * Do not forget to update the status of literals when you run unit resolution
 ******************************************************************************/
Lit* pos_literal(Var* var) {
	return var->posLit;
}

Lit* neg_literal(Var* var) {
	return var->negLit;
}

BOOLEAN is_asserted_literal(Lit* lit){
	if(lit->sindex<0){
		switch(lit->LitValue){
		case 0:
			assert(lit->LitState == 1); // asserted --> ie negative literal and has value false then it will be evaluated to true
			return 1;
			break;
		case 1:
			assert(lit->LitState == 1);
			return 0;
			break;
		default:
			assert(lit->LitState == 0);
			return 0;
			break;
		}
	}
	else if(lit->sindex > 0){
		switch(lit->LitValue){
		case 1:
			assert(lit->LitState == 1);
			return 1;
			break;
		case 0:
			assert(lit->LitState == 1);
			return 0;
			break;
		default:
			assert(lit->LitState == 0);
			return 0;
			break;
		}
	}

	return 0; // this should never happen

}

BOOLEAN is_resolved_literal(Lit* lit){
	if(lit->sindex<0){
		switch(lit->LitValue){
		case 1:
			assert(lit->LitState == 1); // resolved --> ie negative literal and has value true then it will be evaluated to false
			return 1;
			break;
		case 0:
			assert(lit->LitState == 1);
			return 0;
			break;
		default:
			assert(lit->LitState == 0);
			return 0;
			break;
		}
	}
	else if(lit->sindex > 0){
		switch(lit->LitValue){
		case 0:
			assert(lit->LitState == 1);
			return 1;
			break;
		case 1:
			assert(lit->LitState == 1);
			return 0;
			break;
		default:
			assert(lit->LitState == 0);
			return 0;
			break;
		}
	}

	return 0; // this should never happen
}

BOOLEAN is_free_literal(Lit* lit){
	if(lit->LitState == 0)
		return 1;
	else
		return 0;
}


BOOLEAN set_literal(Lit* lit) {

//	//TODO: Check the sanity of this
//	if ( lit->sindex < 0 ) //negative literal
//	 switch(lit->LitValue){
//	 case 0:
//		 lit->LitState = 1; // asserted --> ie negative literal and has value false then it will be evaluated to true
//		 break;
//	 case 1:
//		 lit->LitState = 1 ;// resolved --> ie negative literal and has value true then it will be evaluated to false
//		 break;
//	 case 'd':
//		 lit->LitState = 0; // free --> neither asserted nor resolved
//		 break;
//	 }
//
//	else if(lit->sindex > 0)// positive literal
//	 switch(lit->LitValue){
//	 case 1:
//		 lit->LitState = 1; // asserted --> ie positive literal and has value true then it will be evaluated to true
//		 break;
//	 case 0:
//		 lit->LitState = 1 ;// resolved --> ie positive literal and has value false then it will be evaluated to false
//		 break;
//	 case 'd':
//		 lit->LitState = 0; // free --> neither asserted nor resolved
//		 break;
//	 }
//
	if(is_asserted_literal(lit) || is_resolved_literal(lit))
		return 1;
	else
		return 0;

}

/******************************************************************************
 * Given a clause index i, you should return the corresponding clause 
 * structure (notice you must return a pointer to the structure)
 *
 * Note clause indices range from 1 to m where m is the number of clauses 
 ******************************************************************************/
Clause* index2clausep(unsigned long i, SatState* sat_state) {

	assert(i <= sat_state->num_clauses_in_delta);

	return ((sat_state->delta)+i);
}
 

/******************************************************************************
 * Given a clause, you should return 
 * --1 if the clause is subsumed in the current setting
 * --0 otherwise
 *
 * Note a clause is subsumed if at least one of its literals is implied
 * Do not forget to update the status of clauses when you run unit resolution
 ******************************************************************************/
BOOLEAN subsumed_clause(Clause* clause) {

	// if any of its literals have the LitState =1
	for (unsigned long i = 0; i< clause->num_literals_in_clause; i++){
		if(clause->literals[i]->LitState == 1){
			clause->is_subsumed = 1;
			break;
		}
	}
 
  return clause->is_subsumed;
}


/******************************************************************************
 * A SatState should keep track of pretty much everything you will need to
 * condition/uncondition, perform unit resolution, and do clause learning
 *
 * Given a string cnf_fname, which is a file name of the input CNF, you should
 * construct a SatState
 *
 * This construction will depend on how you define a SatState 
 * Still, you should at least do the following:
 * --read a CNF (in DIMACS format) from the file
 * --initialize variables (n of them)
 * --initialize literals  (2n of them)
 *
 * Once a SatState is constructed, all of the functions that work on a SatState
 * should be ready to use
 *
 * You should also write a function that frees the memory allocated by a
 * SatState (free_sat_state)
 ******************************************************************************/
SatState* construct_sat_state(char* cnf_fname) {

	//initialization
  SatState* sat_state = (SatState *) malloc (sizeof (SatState));

  //sat_state->decisions = (Lit **) malloc(sizeof (Lit*));
  sat_state->gamma = (Clause *) malloc(sizeof(Clause));

  sat_state->current_decision_level = 1; // this is by description
  sat_state->num_clauses_in_delta = 0;
  sat_state->num_clauses_in_gamma = 0;
  sat_state->num_literals_in_decision = 0;
  sat_state->num_literals_in_implications = 0;
  sat_state->num_variables_in_state = 0;


  FILE* cnf_file = fopen(cnf_fname, "r");
  
  if (cnf_file == 0){
	  perror("Cannot open the CNF file");
	  exit(-1);
  }
  else{
	  // call the parser
	  parseDIMACS(cnf_file, sat_state);
  }

  fclose(cnf_file);


	 initialize_vsids_counters(sat_state);

  return sat_state;
}

void free_sat_state(SatState* sat_state) {
//deep cleaning of variables and implications
#ifdef DEBUG
	printf("number of variables in sat_state %ld\n",sat_state->num_variables_in_state );
#endif
	for(unsigned long i = 0; i<sat_state->num_variables_in_state; i++){
		free(sat_state->variables[i].negLit->list_of_watched_clauses);
		free(sat_state->variables[i].posLit->list_of_watched_clauses);
	//	free(sat_state->implications[i]->list_of_watched_clauses); // this causes segmentation fault!
	}
	free(sat_state->variables);
	free(sat_state->implications); // this does not!


//deep cleaning of decisions
	for(unsigned long i = 0; i<sat_state->num_literals_in_decision; i++){
		free(sat_state->decisions[i]->list_of_watched_clauses);
	}
	free(sat_state->decisions);


//deep cleaning of delta
	for(unsigned long i =0; i<sat_state->num_clauses_in_delta; i++){
		free(sat_state->delta[i].L1->list_of_watched_clauses);
		free(sat_state->delta[i].L2->list_of_watched_clauses);
		free(sat_state->delta[i].L1);
		free(sat_state->delta[i].L2);
		for(unsigned long j=0; j<sat_state->delta[i].num_literals_in_clause; j++){
			free(sat_state->delta[i].literals[j]->list_of_watched_clauses);
			free(sat_state->delta[i].literals[j]);
		}
	}
	free(sat_state->delta);


//deep cleaning of gamma
	for(unsigned long i =0; i<sat_state->num_clauses_in_gamma; i++){
		free(sat_state->gamma[i].L1->list_of_watched_clauses);
		free(sat_state->gamma[i].L2->list_of_watched_clauses);
		free(sat_state->gamma[i].L1);
		free(sat_state->delta[i].L2);
		for(unsigned long j=0; j<sat_state->gamma[i].num_literals_in_clause; j++){
			free(sat_state->gamma[i].literals[j]->list_of_watched_clauses);
			free(sat_state->gamma[i].literals[j]);
		}
	}
	free(sat_state->gamma);

	free(sat_state);
}


/******************************************************************************
 * Given a SatState, which should contain data related to the current setting
 * (i.e., decided literals, asserted literals, subsumed clauses, decision
 * level, etc.), this function should perform unit resolution at the current
 * decision level 
 *
 * It returns 1 if succeeds, 0 otherwise (after constructing an asserting
 * clause)
 *
 * There are three possible places where you should perform unit resolution: 
 * (1) after deciding on a new literal (i.e., decide_literal(SatState*)) 
 * (2) after adding an asserting clause (i.e., add_asserting_clause(SatState*)) 
 * (3) neither the above, which would imply literals appearing in unit clauses
 *
 * (3) would typically happen only once and before the other two cases 
 * It may be useful to distinguish between the above three cases
 * 
 * Note if the current decision level is L, then the literals implied by unit
 * resolution must have decision level L
 *
 * This implies that there must be a start level S, which will be the level
 * where the decision sequence would be empty
 *
 * We require you to choose S as 1, then literals implied by (3) would have 1 as
 * their decision level (this level will also be the assertion level of unit
 * clauses)
 *
 * Yet, the first decided literal must have 2 as its decision level
 ******************************************************************************/
BOOLEAN unit_resolution_case_1(SatState* sat_state){
	if(two_literal_watch(sat_state) == 0) {
		//contradiction
		printf("Contradiction");
	}

	return 0;
}

BOOLEAN unit_resolution_case_2(SatState* sat_state){

	return 0;
}

BOOLEAN unit_resolution_case_3(SatState* sat_state){
	// The first time it is called which is case 3 is before any decision or adding assertion clause.
	// This means search in the clauses for unit literals, if found, add it in a list and decide literal on it and run unit resolution.
	for(unsigned long i=0; i<sat_state->num_clauses_in_delta; i++){
		Clause* cur_clause = &(sat_state->delta[i]);
		if (cur_clause->num_literals_in_clause == 1){
			// a unit clause
			Lit* unit_lit = cur_clause->literals[0];
			//update the literal parameters (decide it)
			//Set lit values
			if(unit_lit->sindex < 0)
				unit_lit->LitValue = 0;
			else if (unit_lit->sindex > 0)
				unit_lit->LitValue = 1;

			unit_lit->decision_level = 1; // because it is unit
			unit_lit->LitState = 1;

			//add it in the decision list without incrementing the decision level
			sat_state->decisions[sat_state->num_literals_in_decision++] = unit_lit;
			sat_state->current_decision_level = 1;
		}
	}
	//reset the flag
	FLAG_CASE3_UNIT_RESOLUTION = 0;
	// now the decision list if updated with all unit literals
	// run the two literal watch algorithm
//	if(two_literal_watch(sat_state) == 0) {
//		//contradiction
//		printf("Contradiction");
//		return 0;
//	}
//	else
//		return 1;
	return two_literal_watch(sat_state);

}

BOOLEAN unit_resolution(SatState* sat_state) {

//TODO: How to distinguish between the three cases!


	if(FLAG_CASE3_UNIT_RESOLUTION == 1){
		unit_resolution_case_3(sat_state);
	}
	if(FLAG_CASE2_UNIT_RESOLUTION == 1){
		unit_resolution_case_2(sat_state);
	}
	if(FLAG_CASE1_UNIT_RESOLUTION == 1){
		unit_resolution_case_1(sat_state);
	}






  // if all clauses are subsumed then return true;
//  unsigned long num_subsumed_clauses = 0;
//  for (unsigned long i =0; i <= sat_state->num_clauses_in_delta; i++){
//	  if(subsumed_clause(&sat_state->delta[i])){
//		  num_subsumed_clauses ++;
//		  continue;
//	  }
//	  else
//		  break;
//  }
//  if(num_subsumed_clauses == sat_state->num_clauses_in_delta){
//	  //all clauses are subsumed
//	  return 1;
//  }
//  else{
//	  // not all clauses are subsumed
//	  // take a new decision
//	  // is there a conflict?
//	  return 0;
//  }

 
  return 0; // dummy value
}


/******************************************************************************
 * This function should simply undo all set literals at the current decision
 * level
 ******************************************************************************/
void undo_unit_resolution(SatState* sat_state) {

	unsigned long num_reduced_decisions = 0;
	// undo the set literals at the current decision level
	for(unsigned long i = sat_state->num_literals_in_decision; i >= 1; i--){
		if(sat_state->decisions[i]->decision_level == sat_state->current_decision_level){
			sat_state->decisions[i]->decision_level = 0;
			sat_state->decisions[i]->LitState = 0;
			num_reduced_decisions ++;
		}
		//TODO (Performance enhancing):
		// we are incrementing the decision level one by one so once the decision level
		// of the literal is less than the current one then you can break. You don't have to
		// continue the loop
		if(sat_state->decisions[i]->decision_level < sat_state->current_decision_level){
			break;
		}

	}
	//update the current decision level
	sat_state->num_literals_in_decision = sat_state->num_literals_in_decision - num_reduced_decisions;
	sat_state->current_decision_level -- ;


  return; // dummy value
}


/******************************************************************************
 * This function should set literal lit to true and then perform unit resolution
 * It returns 1 if unit resolution succeeds, 0 otherwise
 *
 * Note if the current decision level is L in the beginning of the call, it
 * should be updated to L+1 so that the decision level of lit and all other
 * literals implied by unit resolution is L+1
 ******************************************************************************/
BOOLEAN decide_literal(Lit* lit, SatState* sat_state) {

	//update the literal parameters
	//Set lit values
	if(lit->sindex < 0)
		lit->LitValue = 0;
	else if (lit->sindex > 0)
		lit->LitValue = 1;

	//TODO:  flip the opposite literal -->handled in two literal watch

	lit->LitState = 1;

	// Update the decision level
	sat_state->current_decision_level ++;
	lit->decision_level = sat_state->current_decision_level;

	// here update the decision array of the sat_state
    sat_state->decisions[sat_state->num_literals_in_decision++] = lit;


  return unit_resolution(sat_state);
}


/******************************************************************************
 * This function should undo all set literals at the current decision level (you
 * can in fact call undo_unit_resolution(SatState*)) 
 *
 * Note if the current decision level is L in the beginning of the call, it
 * should be updated to L-1 before the call ends
 ******************************************************************************/
void undo_decide_literal(SatState* sat_state) {
	undo_unit_resolution(sat_state);
}


/******************************************************************************
 * This function must be called after a contradiction has been found (by unit
 * resolution), an asserting clause constructed, and backtracking took place to
 * the assertion level (i.e., the current decision level is the same as the
 * assertion level of the asserting clause)
 *
 * This function should add the asserting clause into the set of learned clauses
 * (so that unit resolution from there on would also take into account the
 * asserting clause), and then perform unit resolution 
 *
 * It returns 1 if unit resolution succeeds, which means the conflict is
 * cleared, and 0 otherwise (that is, we have a new asserting clause with a new
 * assertion level)
 *
 * Note since the learned clause is asserting and we are at the assertion level
 * of the clause, it will become a unit clause under the current setting 
 *
 * Also, if the learned clause itself is a unit clause, its assertion level must
 * be the same as the start level S, which is 1 (i.e., the level in
 * which no decision is made) 
 ******************************************************************************/
BOOLEAN add_asserting_clause(SatState* sat_state) {

  // ... TO DO ..

	return unit_resolution(sat_state);
}


/******************************************************************************
 * This function can be called after a contradiction has been found (by unit
 * resolution), an asserting clause constructed, and the conflict is not cleared
 * yet (that is, conflict_exists(SatState*) must return 1 at the time of call)
 *
 * It returns 1 if the current decision level is the same as the assertion level
 * of the asserting clause, 0 otherwise
 ******************************************************************************/
BOOLEAN at_assertion_level(SatState* sat_state) {

  // ... TO DO ..

  return 0; // dummy value
}


/******************************************************************************
 * It returns 1 if the current decision level is the same as the start level,
 * which is 1 (i.e., the level in which no decision is made), 0 otherwise
 ******************************************************************************/
BOOLEAN at_start_level(SatState* sat_state) {

	if(sat_state->current_decision_level == 1)
		return 1;
	else
		return 0;
}


/******************************************************************************
 * It returns 1 if there is a conflict in the current setting, 0 otherwise
 *
 * --Initially there is no conflict
 * --If unit resolution finds a contradiction, then we have a conflict
 * --A conflict is cleared when we backtrack to the assertion level, add the
 * asserting clause into the set of learned clauses, and successfully perform
 * unit resolution (i.e., the call add_asserting_clause(SatState*) returns 1)
 ******************************************************************************/
BOOLEAN conflict_exists(SatState* sat_state) {

  // ... TO DO ..

  return 0; // dummy value
}

/******************************************************************************
 * end
 ******************************************************************************/
