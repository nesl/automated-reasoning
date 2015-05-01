/*
 * ParseDIMACS.c
 *
 *  Created on: Apr 30, 2015
 *      Author: salma
 */

#include "ParseDIMACS.h"
#include <string.h>

SatState* parseDIMACS(FILE* cnf_file){

	char* line = NULL;
	int len = 0;
	int read;

	while((read = getline(&line, &len, cnf_file)) != -1){

		// ignore anything starts with a c  (comment line)
		if ((*line)[0] == 'c') continue;
		// parse the line that starts with p (problem line  p cnf <number of variables n> <number of clauses m>)
		if((*line)[0] == 'p'){
			// read in the values


		}


	}

	if(line)
		free(line);

	return NULL; //dummy value
}
