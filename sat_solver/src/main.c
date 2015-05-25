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

	Lit * max_lit = NULL;
	unsigned long max_score = 0;

	for (unsigned long vidx = 0; vidx < sat_state->num_variables_in_state; vidx++)
	{
		Var cur_var = sat_state->variables[vidx];

		Lit * cur_lit;

		cur_lit = cur_var.posLit;

		if (cur_lit->vsids_score > max_score)
		{
			if (is_free_literal(cur_lit))
			{
				max_score = cur_lit->vsids_score;
				max_lit = cur_lit;
			}
		}
		else if (cur_lit->vsids_score == max_score)
		{
			if (is_free_literal(cur_lit))
			{
				// break ties with some randomization
				if (rand() % 2) {
					max_score = cur_lit->vsids_score;
					max_lit = cur_lit;
				}
			}
		}

		cur_lit = cur_var.negLit;

		if (cur_lit->vsids_score > max_score)
		{
			if (is_free_literal(cur_lit))
			{
				max_score = cur_lit->vsids_score;
				max_lit = cur_lit;
			}
		}
		else if (cur_lit->vsids_score == max_score)
		{
			if (is_free_literal(cur_lit))
			{
				// break ties with some randomization
				if (rand() % 2) {
					max_score = cur_lit->vsids_score;
					max_lit = cur_lit;
				}
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
  if(sat(sat_state)) printf("SAT\n");
  else printf("UNSAT\n");

	printf("Hello.\n");

	Lit * l = get_free_literal(sat_state);
	printf("The sindex of l is %d\n", l->sindex);

	for (unsigned long vidx = 0; vidx < sat_state->num_variables_in_state; vidx++) {
		printf("index = %d\n", sat_state->variables[vidx].index);
		printf("  posLit: ");
		printf("    sindex = %d ", sat_state->variables[vidx].posLit->sindex);
		printf("score  = %d\n", sat_state->variables[vidx].posLit->vsids_score);
		printf("  negLit: ");
		printf("    sindex = %d ", sat_state->variables[vidx].negLit->sindex);
		printf("score = %d\n", sat_state->variables[vidx].negLit->vsids_score);
	}

	Clause newalpha;
	newalpha.literals = malloc(sizeof(Lit *) * 3);
	newalpha.num_literals_in_clause = 3;
	newalpha.literals[0] = index2varp(4, sat_state)->negLit;
	newalpha.literals[1] = index2varp(10, sat_state)->posLit;
	newalpha.literals[2] = index2varp(8, sat_state)->negLit;
	sat_state->alpha = newalpha;
	update_vsids_scores(sat_state);

	l = get_free_literal(sat_state);
	printf("The sindex of l is %d\n", l->sindex);

	for (unsigned long vidx = 0; vidx < sat_state->num_variables_in_state; vidx++) {
		printf("index = %d\n", sat_state->variables[vidx].index);
		printf("  posLit: ");
		printf("    sindex = %d ", sat_state->variables[vidx].posLit->sindex);
		printf("score  = %d\n", sat_state->variables[vidx].posLit->vsids_score);
		printf("  negLit: ");
		printf("    sindex = %d ", sat_state->variables[vidx].negLit->sindex);
		printf("score = %d\n", sat_state->variables[vidx].negLit->vsids_score);
	}

  free_sat_state(sat_state);
  return 0;
}

/******************************************************************************
 * end
 ******************************************************************************/
