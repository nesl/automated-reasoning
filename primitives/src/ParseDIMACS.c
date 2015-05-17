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
static int parseProblemLine(char* line, unsigned long n, unsigned long m){
	char* pch = NULL;
	int countparameters = 0;

	printf("%s\n",line);

	//separate the string into space separated tokens
	pch = strtok(line, " ");

	while (pch != NULL){
		// skip the p cnf park of the line
		if ( !strstr(pch, "p")  &&  !strstr(pch, "cnf") ) {
			printf("%s\n", pch);
			if(countparameters == 0 )
				n = atoi(pch);
			else
				m = atoi(pch);

			countparameters++;
		}

		pch = strtok (NULL, " ");
	}

	if(pch)
	 free(pch);


	return 1;
}

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

			literals[countvariables++].sindex = atol(pch);

		pch = strtok (NULL, " ");
	}

	if(pch)
	 free(pch);

	clause->literals = literals;
	clause->num_literals_in_clause = countvariables-1;
	clause->is_subsumed = 0;

	return countvariables-1; // the clause ends with 0 as a termination so we subtract this element to return teh correct value
}
/* **************************************************************************** */

#ifdef DEBUG
static void DebugCNF(unsigned long m, unsigned long n, long cnf[m][n]){

	printf("CNF Debugging for %ld clauses \n", m);
	printf("%ld",cnf[0][1]);
}
#endif

void parseDIMACS(FILE* cnf_file, SatState * sat_state){

	char * line = NULL;
	size_t len = 0;
	ssize_t read;

	//unsigned long n = 0; /// number of variables
	//unsigned long m = 0; // number of clauses
	//long cnf[m][n]; // a pointer of clauses which points to an array of variables =  long (*a)[]

	unsigned long clausecounter = 0;
	while((read = getline(&line, &len, cnf_file)) != -1){

		// ignore anything starts with a c  (comment line)
		if (line[0] == 'c') continue;

		// parse the line that starts with p (problem line  p cnf <number of variables n> <number of clauses m>)
		if((line)[0] == 'p'){
			//parseProblemLine(line, &n, &m);
			parseProblemLine(line, sat_state->num_variables_in_state, sat_state->num_clauses_in_delta);
			sat_state->variables =( (Var*) malloc(sizeof(Var)*sat_state->num_variables_in_state) );
			sat_state->delta = (Clause *) malloc(sizeof(Clause) * sat_state->num_clauses_in_delta );

#ifdef DEBUG
			printf("number of clauses: %ld, number of variables: %ld\n", sat_state->num_clauses_in_delta, sat_state->num_variables_in_state);
#endif
	}
		else
		{
			// read the CNF
			//unsigned long vars = parseClause(line,n, m, cnf[clausecounter++]);
			unsigned long vars = parseClause(line, &sat_state->delta[clausecounter++]);
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



