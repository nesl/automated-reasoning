/*
 ============================================================================
 Name        : Experiments.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "ParseDIMACS.h"


int main(void) {
	puts("!!!Hello World!!!"); /* prints !!!Hello World!!! */

	  FILE* cnf_file = fopen("c17.cnf", "r");

	  if (cnf_file == 0){
		  // fopen returns 0, the NULL pointer, on failure
		  perror("Cannot open the CNF file");
		  exit(-1);
	  }
	  else{
		  // call the parser
		  parseDIMACS(cnf_file);
	  }

	  fclose(cnf_file);






	return EXIT_SUCCESS;
}
