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
		// skip the p cnf park of the line
		if ( !strstr(pch, "p")  &&  !strstr(pch, "cnf") ) {
			printf("%s\n", pch);
			if(countparameters == 0 ){
				sat_state->num_variables_in_state = atoi(pch);
				sat_state->variables =( (Var*) malloc(sizeof(Var) * sat_state->num_variables_in_state) );
			}
			else{
				sat_state->num_clauses_in_delta = atoi(pch);
				sat_state->delta =( (Clause *) malloc(sizeof(Clause) * sat_state->num_clauses_in_delta) );
			}
			countparameters++;
		}

		pch = strtok (NULL, " ");
	}

	if(pch)
	 free(pch);


	return 1;
}


/******************************************************************************
 * Parse Clause line
 * -- For each line in the DIMACS file construct a new clause struct
 * -- initialize the array of literals this clause contains
 ******************************************************************************/
static unsigned long parseClause(char* line, Clause* clause){
	char* pch = NULL;
	unsigned long countvariables = 0;
	Lit* literals = (Lit *) malloc(sizeof (Lit));
#ifdef DEBUG
	printf("%s\n",line);
#endif
	//separate the string into space separated tokens
	pch = strtok(line, " ");

	while (pch != NULL){
#ifdef DEBUG
			printf("%s\n", pch);
#endif

			literals[countvariables].sindex = atol(pch);
			literals[countvariables].LitState = 0; // free literal
			literals[countvariables].decision_level = 1;
			literals[countvariables].list_of_clauses =  (unsigned long *) malloc (sizeof(unsigned long) );
			literals[countvariables].LitValue = 'd'; // initialize to free literal

			countvariables ++;

		pch = strtok (NULL, " ");
	}

	if(pch)
	 free(pch);


	clause->literals = literals;
	clause->num_literals_in_clause = countvariables-1;
	clause->is_subsumed = 0;

	// For the two literal watch // just initialize here
	clause->u=(Lit *) malloc(sizeof (Lit));;
	clause->v=(Lit *) malloc(sizeof (Lit));;


	return countvariables-1; // the clause ends with 0 as a termination so we subtract this element to return the correct value

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
			unsigned long vars = parseClause(line, &sat_state->delta[clausecounter++]);
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



