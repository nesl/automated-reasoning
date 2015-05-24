#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "satapi.h"


#ifdef DEBUG
	BOOLEAN choose1 = 0;
	BOOLEAN choose2 = 0;
	BOOLEAN choose3 = 0;
#endif

/******************************************************************************
 * SAT solver 
 ******************************************************************************/


/******************************************************************************
 * Apart from the primitives in satapi.h, you should implement the following
 * function
 *
 * It returns some literal which is free in the current setting of sat_state  
 * If all literals are set, then it returns NULL
 ******************************************************************************/
Lit* get_free_literal(SatState* sat_state) {

  // ... TO DO ..
	// assign an value for debugging
#ifdef DEBUG
	if(choose1 == 0){
		choose1 = 1;
		return sat_state->variables[0].negLit;

	}
	else if (choose2 == 0){
		choose2 = 1;
		return sat_state->variables[3].posLit;

	}
	else if(choose3 == 0){
		choose3 =1;
		return sat_state->variables[4].negLit;

	}
	else
		return NULL;
#endif

	return NULL;

}

BOOLEAN sat_aux(SatState* sat_state) {
  Lit* lit = get_free_literal(sat_state);
  if(lit==NULL)
	  return 1; // all literals are implied

  BOOLEAN ret = 0;

  if(decide_literal(lit,sat_state)) ret = sat_aux(sat_state);
  undo_decide_literal(sat_state);   //TODO: I don't know why undo decide literal is called ... the ret value in the stack is not correct

  if(ret==0) { // there is a conflict
    if(at_assertion_level(sat_state) && add_asserting_clause(sat_state))
      return sat_aux(sat_state); // try again
    else
      return 0; // backtrack (still conflict)
  }
  else return 1; // satisfiable
}

BOOLEAN sat(SatState* sat_state) {
  BOOLEAN ret = 0;
  if(unit_resolution(sat_state))  ret = sat_aux(sat_state);
  undo_unit_resolution(sat_state); // everything goes back to the initial state

  return ret;
}


char USAGE_MSG[] = "Usage: ./sat -in_cnf <cnf_file>\n";

int main(int argc, char* argv[]) {	

  char* cnf_fname = NULL;
  if(argc==3 && strcmp("-in_cnf",argv[1])==0) 
    cnf_fname = argv[2];
  else {
    printf("%s",USAGE_MSG);
    exit(1);
  }
	
  // construct a sat state and then check satisfiability
  SatState* sat_state = construct_sat_state(cnf_fname);
  if (sat_state != NULL){
	  // for debugging use example in text chapter 3
#ifdef DEBUG
	  /* For test of two literal watch */
	  /* decide_literal(sat_state->variables[0].negLit, sat_state); // -V1
	  decide_literal(sat_state->variables[3].posLit, sat_state); // V4
	  decide_literal(sat_state->variables[4].negLit, sat_state); //-V5
	  */

	  /*test for conflict driven clause */
	  if(!decide_literal(sat_state->variables[0].posLit, sat_state)) undo_unit_resolution(sat_state); // A
	  if(!decide_literal(sat_state->variables[1].posLit, sat_state)) undo_unit_resolution(sat_state); // B
	  if(!decide_literal(sat_state->variables[2].posLit, sat_state)) undo_unit_resolution(sat_state); // C
	  if(!decide_literal(sat_state->variables[3].posLit, sat_state)) undo_unit_resolution(sat_state); // X

#endif


//	  if(sat(sat_state))
//		  printf("SAT\n");
//	  else
//		  printf("UNSAT\n");



#ifdef DEBUG
//	  decide_literal(sat_state->variables[0].negLit, sat_state); // -V1
//	  decide_literal(sat_state->variables[3].posLit, sat_state); // V4
//	  decide_literal(sat_state->variables[4].negLit, sat_state); //-V5

	  for(unsigned long i = 0; i < sat_state->num_literals_in_decision; i++){
		  printf("%ld\n",sat_state->decisions[i]->sindex);
	  }
#endif
  }
#ifdef DEBUG
  printf("END OF SAT_STATE_CONSTRUCT");
#endif

  free_sat_state(sat_state);
  return 0;
}

/******************************************************************************
 * end
 ******************************************************************************/
