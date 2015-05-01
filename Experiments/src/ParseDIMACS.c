/*
 * ParseDIMACS.c
 *
 *  Created on: Apr 30, 2015
 *      Author: salma
 */

#include "ParseDIMACS.h"
#include <string.h>



int parseProblemLine(char* line, unsigned long* n, unsigned long* m){
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
				*n = atoi(pch);
			else
				*m = atoi(pch);

			countparameters++;
		}

		pch = strtok (NULL, " ");
	}

	if(pch)
	 free(pch);


	return 1;
}

int parseClause(char* line, unsigned long n, unsigned long m, long* clause){
	char* pch = NULL;
	int countvariables = 0;


	printf("%s\n",line);

	//separate the string into space separated tokens
	pch = strtok(line, " ");

	while (pch != NULL){

			printf("%s\n", pch);
//			if(countparameters == 0 )
//				*n = atoi(pch);
//			else
//				*m = atoi(pch);
			clause[countvariables++]=atol(pch);
//
//			countparameters++;
//		}

		pch = strtok (NULL, " ");
	}

	if(pch)
	 free(pch);


	return 1;
}



void DebugCNF(unsigned long n, unsigned long m, long *cnf){

	printf("CNF Debugging for %ld clauses \n", m);

	printf("%ld",*((cnf+0*0) + 0));
}


int parseDIMACS(FILE* cnf_file){

	char* line = NULL;
	size_t len = 0;
	ssize_t read;

	unsigned long n = 0; /// number of variables
	unsigned long m = 0; // number of clauses

	long cnf[m][n]; // a pointer of clauses which points to an array of variables =  long (*a)[]


	unsigned long clausecounter = 0;
	while((read = getline(&line, &len, cnf_file)) != -1){

		// ignore anything starts with a c  (comment line)
		if (line[0] == 'c') continue;

		// parse the line that starts with p (problem line  p cnf <number of variables n> <number of clauses m>)
		if((line)[0] == 'p'){
			parseProblemLine(line, &n, &m);
			printf("number of clauses: %ld, number of variables: %ld\n", m,n);
		}
		else
		{
			// read the CNF
			parseClause(line,n, m, cnf[clausecounter++]);

			printf("%ld\n", cnf[clausecounter][1]);

		}
	}


	//Debug
	DebugCNF(n,m, (long*)cnf);

	if(line)
		free(line);

	return 1; //dummy value
}



