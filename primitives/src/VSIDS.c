/*
 * VSIDS.c
 *
 *  Created on: May 22, 2015
 *      Author: salma
 */

#include "VSIDS.h"

#include <stdlib.h>

void initialize_vsids_scores (SatState * sat_state)
{
	for (unsigned long cidx = 0; cidx < sat_state->num_clauses_in_delta; cidx++)
	{
		Clause * cur_clause = & (sat_state->delta[cidx]);
		for (unsigned long lidx = 0; lidx < cur_clause->num_literals_in_clause; lidx++)
		{
			Lit * cur_lit = cur_clause->literals[lidx];
			cur_lit->vsids_score++;
		}
	}
}

void update_vsids_scores (SatState * sat_state)
{
	printf("Updating!\n");

	if (rand() % 4) // very rough 25% probability
	{
		// divide all the "counters" by a constant
		for (unsigned long vidx = 0; vidx < sat_state->num_variables_in_state; vidx++)
		{
			Var cur_var = sat_state->variables[vidx];
			Lit * pos = cur_var.posLit;
			Lit * neg = cur_var.negLit;
			pos->vsids_score = pos->vsids_score << 2;
			neg->vsids_score = neg->vsids_score << 2;
			// need to test to determine a magic number by which to decrement
		}
	}

	Clause alpha = sat_state->alpha;
	for (unsigned long lidx = 0; lidx < alpha.num_literals_in_clause; lidx++)
	{
		alpha.literals[lidx]->vsids_score++;
	}
}
