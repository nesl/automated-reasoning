#include "satapi.h"
#include <fcntl.h>


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

  // ... TO DO ..

  
  return NULL; // dummy valued
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

  // ... TO DO ..
	Lit l;
	l.sindex = var->index;
	l.isPositive = 't';
  
  return &l; // just so you know, this won't work
	// l exists only in this stack frame so returning it will produce garbage
}

Lit* neg_literal(Var* var) {

  // ... TO DO ..
	Lit l;
	l.sindex = var->index - (2*var->index);  // if index was 1000 now i want the neg literal then it is -1000 which is 1000 - 2(1000)
    l.isNegative = 't';

  return &l; // dummy value
}

BOOLEAN set_literal(Lit* lit) {

  // ... TO DO ..
	//lit->LitState
  
  return lit->LitState; // dummy value
}

/******************************************************************************
 * Given a clause index i, you should return the corresponding clause 
 * structure (notice you must return a pointer to the structure)
 *
 * Note clause indices range from 1 to m where m is the number of clauses 
 ******************************************************************************/
Clause* index2clausep(unsigned long i, SatState* sat_state) {

  // ... TO DO ..
	Clause c = *((sat_state->Delta)+i);

  return &c; // dummy value
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

  // ... TO DO ..
	// if any of its literals have the LitState =1
	// This is buggy ... you need to check the limits of the for loop and the number of literals and make sure they work right together

	Lit* clause_set_lit = clause->setLit;
//
//	for (unsigned int i = 0; i<= clause->NumLit; i++){
//		if((*clause_set_lit)[i]->LitState == 't')
//			return 1;
//	}

 
  return 0; // dummy value
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

  // ... TO DO ..
  SatState* sat_state;
  FILE* cnf_file = fopen(cnf_fname, "r");
  
  if (cnf_file == 0){
	  // fopen returns 0, the NULL pointer, on failure
	  perror("Cannot open the CNF file");
	  exit(-1);
  }
  else{
	  // call the parser
	  sat_state = (SatState*)parseDIMACS(cnf_file);
  }

  fclose(cnf_file);

  return sat_state; // dummy value
}

void free_sat_state(SatState* sat_state) {

  // ... TO DO ..
 
  return; // dummy value
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
BOOLEAN unit_resolution(SatState* sat_state) {

  // ... TO DO ..
 
  return 0; // dummy value
}


/******************************************************************************
 * This function should simply undo all set literals at the current decision
 * level
 ******************************************************************************/
void undo_unit_resolution(SatState* sat_state) {

  // ... TO DO ..

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

  // ... TO DO ..

  return 0; // dummy value
}


/******************************************************************************
 * This function should undo all set literals at the current decision level (you
 * can in fact call undo_unit_resolution(SatState*)) 
 *
 * Note if the current decision level is L in the beginning of the call, it
 * should be updated to L-1 before the call ends
 ******************************************************************************/
void undo_decide_literal(SatState* sat_state) {

  // ... TO DO ..

  return; // dummy value
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

  return 0; // dummy value
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

  // ... TO DO ..

  return 0; // dummy value
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
