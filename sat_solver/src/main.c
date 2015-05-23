#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "satapi.h"


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

  //this is the place for heuristic and variable order
  //for now just pick the first free literal encountered

	Lit * max_lit = NULL;
	unsigned long max_score = 0;

	for (unsigned long vidx = 0; vidx < sat_state->num_variables_in_state; vidx++)
	{
		Var cur_var = sat_state->variables[vidx];

		Lit * cur_lit;

		cur_lit = cur_var.posLit;

		if (cur_lit->vsids_score > max_score)
		{
			max_score = cur_score;
			max_lit = cur_lit;
		}
		else if (cur_lit->vsids_score == max_score)
		{
			// break ties with some randomization
			if (rand() % 2) {
				max_score = cur_score;
				max_lit = cur_lit;
			}
		}

		cur_lit = cur_var.negLit;

		// ugly
		if (cur_lit->vsids_score > max_score)
		{
			max_score = cur_score;
			max_lit = cur_lit;
		}
		else if (cur_lit->vsids_score == max_score)
		{
			// break ties with some randomization
			if (rand() % 2) {
				max_score = cur_score;
				max_lit = cur_lit;
			}
		}
	}

  return max_lit;
}

BOOLEAN sat_aux(SatState* sat_state) {
  Lit* lit = get_free_literal(sat_state);
  if(lit==NULL) return 1; // all literals are implied

  BOOLEAN ret = 0;

  if(decide_literal(lit,sat_state)) ret = sat_aux(sat_state);
  undo_decide_literal(sat_state);

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
  if(unit_resolution(sat_state)) ret = sat_aux(sat_state);
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
  if (sat_state != NULL) return 1;
//  if(sat(sat_state)) printf("SAT\n");
//  else printf("UNSAT\n");
//  free_sat_state(sat_state);

  return 0;
}

/******************************************************************************
 * end
 ******************************************************************************/
