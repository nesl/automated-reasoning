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


#define END_OF_CLAUSE_LINE 		0
#define MALLOC_GROWTH_RATE 	   	2


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
				//allocate memory for n of variables and at the end the number of implications will equal to the number of variables;
				sat_state->variables =( (Var*) malloc(sizeof(Var) * sat_state->num_variables_in_state) );
				//sat_state->implications =( (Lit **) malloc(sizeof(Lit*) * sat_state->num_variables_in_state) );
				sat_state->decisions =( (Lit **) malloc(sizeof(Lit*) * sat_state->num_variables_in_state) );
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

//	if(pch)
//	 free(pch);


	// initialize the n variables and 2n literals;
	for(unsigned long i = 0; i < sat_state->num_variables_in_state; i++ ){
		printf("in loop i = %ld\n", i);

		sat_state->variables[i].index = i+1; 								// variables start with index 1 to n ;
		sat_state->variables[i].negLit = (Lit*) malloc(sizeof(Lit) );
		sat_state->variables[i].negLit->sindex = i+1 - (2*(i+1));			// negative literals start with -1 to -n;
		sat_state->variables[i].negLit->LitState = 0; 						// free literal
		sat_state->variables[i].negLit->decision_level = 1;
		sat_state->variables[i].negLit->LitValue = 'u'; 					// initialize to unassigned literal
		sat_state->variables[i].negLit->num_watched_clauses = 0;
		sat_state->variables[i].negLit->list_of_watched_clauses = (unsigned long*) malloc(sizeof(unsigned long)); // will be realloc when it expands
		sat_state->variables[i].negLit->max_size_list_watched_clauses = 1;



		sat_state->variables[i].posLit = (Lit*) malloc(sizeof(Lit) );
		sat_state->variables[i].posLit = (Lit*) malloc(sizeof(Lit) );
		sat_state->variables[i].posLit->sindex = i+1;						// negative literals start with -1 to -n;
		sat_state->variables[i].posLit->LitState = 0; 						// free literal
		sat_state->variables[i].posLit->decision_level = 1;
		sat_state->variables[i].posLit->LitValue = 'u';
		sat_state->variables[i].posLit->num_watched_clauses = 0;// initialize to free literal
		sat_state->variables[i].posLit->list_of_watched_clauses = (unsigned long*) malloc(sizeof(unsigned long)); // will be realloc when it expands
		sat_state->variables[i].posLit->max_size_list_watched_clauses = 1;
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

	// initialize memory of clause literals with one element and then increase the size by double if needed
	int MIN_CAPACITY = 1;

	clause->literals = (Lit**) malloc(sizeof(Lit*) * MIN_CAPACITY);


	//separate the string into space separated tokens
	pch = strtok(line, " ");

	while (pch != NULL){
#ifdef DEBUG
			printf("%s\n", pch);
#endif
			if(atol(pch) == END_OF_CLAUSE_LINE){
				break;
			}

			if(countvariables >= MIN_CAPACITY){
				// needs to realloc the size
				MIN_CAPACITY = countvariables * MALLOC_GROWTH_RATE;
				clause->literals = (Lit**) realloc( clause->literals, sizeof(Lit*) * MIN_CAPACITY);
			}

			signed long value = atol(pch);
			 Var* var =  &(sat_state->variables[abs(value) - 1]); // because array is filled from 0 to n-1
			 if(value < 0)
				 clause->literals[countvariables] = neg_literal(var);
			 else if (value > 0)
				 clause->literals[countvariables] = pos_literal(var);

			countvariables ++;

		pch = strtok (NULL, " ");
	}


	clause->num_literals_in_clause = countvariables;
	clause->is_subsumed = 0;

	// For the two literal watch // just initialize here
	clause->L1 =  clause->literals[0]; // first literal
	clause->L2 =  clause->literals[1]; // second literal

	return countvariables;
}
/* **************************************************************************** */



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
			sat_state->delta[clausecounter].cindex = clausecounter;
			unsigned long vars = parseClause(sat_state, line, &sat_state->delta[clausecounter++]);
#ifdef DEBUG
			printf("Number of variables: %ld\n", vars);
#endif

		}
	}

//	if(line)
//		free(line);

}



