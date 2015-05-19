/*
 * ParseDIMACS.c
 *
 *  Created on: Apr 30, 2015
 *      Author: salma
 */

#define _GNU_SOURCE
#include "ParseDIMACS.h"
#include <string.h>
#include <stdio.h>


#define END_OF_CLAUSE_LINE 0

/**Helper functions */
/* **************************************************************************** */



/******************************************************************************
 * Parse Problem line
 * -- Get the definition of the problem by getting the number of clauses and
 * -- number of variables
 * -- initialize space in the memory for their sizes
 ******************************************************************************/
static int parseProblemLine(char* line, SatState* sat_state){
	char* pch = NULL;
	int countparameters = 0;

	printf("%s\n",line);

	//separate the string into space separated tokens
	pch = strtok(line, " ");

	while (pch != NULL){
		// skip the p cnf part of the line
		if ( !strstr(pch, "p")  &&  !strstr(pch, "cnf") ) {
			printf("%s\n", pch);
			if(countparameters == 0 ){
				sat_state->num_variables_in_state = atoi(pch);
				//allocate memory for n of variables;
				sat_state->variables =( (Var*) malloc(sizeof(Var) * sat_state->num_variables_in_state) );
				//allocate memory for 2n literals
				sat_state->positiveLiterals = ( (Lit*) malloc(sizeof(Lit) * sat_state->num_variables_in_state) );
				sat_state->negativeLiterals = ( (Lit*) malloc(sizeof(Lit) * sat_state->num_variables_in_state) );
			}
			else{
				sat_state->num_clauses_in_delta = atoi(pch);
				// allocate memory for m clauses
				sat_state->delta =( (Clause *) malloc(sizeof(Clause) * sat_state->num_clauses_in_delta) );
			}
			countparameters++;
		}

		pch = strtok (NULL, " ");
	}

	if(pch)
	 free(pch);


	// initialize the variables and literals;
	for(unsigned long i = 0; i < sat_state->num_variables_in_state; i++ ){

		sat_state->positiveLiterals[i].sindex = i+1; 			// positive literals start with 1 to n;
		sat_state->negativeLiterals[i].sindex = i+1 - (2*(i+1));// negative literals start with -1 to -n;

		sat_state->positiveLiterals[i].LitState = 0; // free literal
		sat_state->positiveLiterals[i].decision_level = 1;
		sat_state->positiveLiterals[i].list_of_watched_clauses =  (unsigned long *) malloc (sizeof(unsigned long) );
		sat_state->positiveLiterals[i].LitValue = 'd'; // initialize to free literal

		sat_state->negativeLiterals[i].LitState = 0; // free literal
		sat_state->negativeLiterals[i].decision_level = 1;
		sat_state->negativeLiterals[i].list_of_watched_clauses =  (unsigned long *) malloc (sizeof(unsigned long) );
		sat_state->negativeLiterals[i].LitValue = 'd'; // initialize to free literal


		sat_state->variables[i].index = i+1; 					// variables start with index 1 to n ;
		sat_state->variables[i].posLit = &sat_state->positiveLiterals[i];
		sat_state->variables[i].negLit = &sat_state->negativeLiterals[i];
	}


	return 1;
}


/******************************************************************************
 * Parse Clause line
 * -- For each line in the DIMACS file construct a new clause struct
 * -- initialize the array of literals this clause contains
 ******************************************************************************/
static unsigned long parseClause(SatState* sat_state, char* line, Clause* clause){
	char* pch = NULL;
	unsigned long countvariables = 0;
	//Lit* literals = (Lit *) malloc(sizeof (Lit));
#ifdef DEBUG
	printf("%s\n",line);
#endif
	//separate the string into space separated tokens
	pch = strtok(line, " ");

	while (pch != NULL){
#ifdef DEBUG
			printf("%s\n", pch);
#endif
//TODO: enhance by checking if atol(pch) == 0 then this is the end of the clause line and no need to allocate memory for this
			if(atol(pch) == END_OF_CLAUSE_LINE){
				break;
			}

//			// This is wrong --> this means that for the same literal it will be different struct because it is in different clause!!
//			literals[countvariables].sindex = atol(pch);
//			literals[countvariables].LitState = 0; // free literal
//			literals[countvariables].decision_level = 1;
//			literals[countvariables].list_of_watched_clauses =  (unsigned long *) malloc (sizeof(unsigned long) );
//			literals[countvariables].LitValue = 'd'; // initialize to free literal


			signed long value = atol(pch);
			 Var* var =  &sat_state->variables[abs(value) - 1]; // because array is filled from 0 to n-1
			 if(value < 0)
				 clause->literals[countvariables] = neg_literal(var);
			 else if (value > 0)
				 clause->literals[countvariables] = pos_literal(var);

			countvariables ++;

		pch = strtok (NULL, " ");
	}

	if(pch)
	 free(pch);


	//clause->literals = literals;
	clause->num_literals_in_clause = countvariables -1 ;
	clause->is_subsumed = 0;

	// For the two literal watch // just initialize here
	//clause->u=(Lit *) malloc(sizeof (Lit));
	//clause->v=(Lit *) malloc(sizeof (Lit));
	clause->u =  clause->literals[0]; // first literal
	clause->v =  clause->literals[1]; // second literal

	return countvariables -1 ; // TODO: Check this: the clause ends with 0 as a termination so we subtract 1  to return the correct value

}
/* **************************************************************************** */

#ifdef DEBUG
//static void DebugCNF(unsigned long m, unsigned long n, long cnf[m][n]){
//
//	printf("CNF Debugging for %ld clauses \n", m);
//	printf("%ld",cnf[0][1]);
//}
#endif





/******************************************************************************
 * Parse the DIMACS file that contains the CNF
 * -- fill in the sat_state data structure with initialized spaces in memory
 ******************************************************************************/
void parseDIMACS(FILE* cnf_file, SatState * sat_state){

	char * line = NULL;
	size_t len = 0;
	ssize_t read;

	unsigned long clausecounter = 0;
	while((read = getline(&line, &len, cnf_file)) != -1){

		// ignore anything starts with a c  (comment line)
		if (line[0] == 'c') continue;

		// parse the line that starts with p (problem line  p cnf <number of variables n> <number of clauses m>)
		if((line)[0] == 'p'){
			parseProblemLine(line, sat_state);
#ifdef DEBUG
			printf("number of clauses: %ld, number of variables: %ld\n", sat_state->num_clauses_in_delta, sat_state->num_variables_in_state);
#endif
		}
		else
		{
			// read the CNF
			unsigned long vars = parseClause(sat_state, line, &sat_state->delta[clausecounter++]);
			//TODO: Add the variables in the clause to sat_state->variables.
			printf("Number of variables: %ld", vars);

#ifdef DEBUG
			//printf("access clause %ld and element 2 is %ld, number of var is this clause is %ld\n",clausecounter,cnf[clausecounter][2],vars);
#endif
		}
	}

#ifdef DEBUG
//	printf("Clause Counter %ld\n", clausecounter);
//
//	for(unsigned long c = 0; c < clausecounter; c++ ){
//		printf("Debugging Clause number %ld\n", c );
//		for(unsigned long v = 0; v < n; n++){
//			printf("%ld\n",cnf[c][v]);
//		}
//	}
	//DebugCNF(m,n, cnf);

#endif

	if(line)
		free(line);

}



