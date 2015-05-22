/*
 * VSIDS.c
 *
 *  Created on: May 22, 2015
 *      Author: salma
 */

#include "VSIDS.h"

void initialize_vsids_counters (SatState * sat_state)
{
	for (unsigned long cidx = 0; cidx < sat_state->num_clauses_in_delta; cidx++)
	{
		Clause * cur_clause = & (sat_state->delta[cidx]);
		for (unsigned long lidx = 0; lidx < cur_clause->num_literals_in_clause; lidx++)
		{
			//Salma: You won't need the reference here because it is already a pointer
			//Lit * cur_lit = & (cur_clause->literals[lidx]);
			Lit * cur_lit = cur_clause->literals[lidx];
			cur_lit->vsids_counter++;
			printf("happening to %x\n", cur_lit);
		}
	}

	for (unsigned long vidx = 0; vidx < sat_state->num_variables_in_state; vidx++)
	{
		Var * cur_var = & (sat_state->variables[vidx]);
		Lit * pos_lit = cur_var->posLit;
		Lit * neg_lit = cur_var->negLit;

		printf("Literal %ld occurs %lu times (address %x)\n", pos_lit->sindex, pos_lit->vsids_counter, pos_lit);
		printf("Literal %ld occurs %lu times (address %x)\n", neg_lit->sindex, neg_lit->vsids_counter, neg_lit);
	}
	exit(0);

}
