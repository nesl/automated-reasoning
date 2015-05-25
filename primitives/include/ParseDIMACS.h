/*
 * ParseDIMACS.h
 *
 *  Created on: Apr 30, 2015
 *      Author: salma
 */

#ifndef PARSEDIMACS_H_
#define PARSEDIMACS_H_

#include "sat_api.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void parseDIMACS(FILE* cnf_file, SatState* sat_state);



#endif /* PARSEDIMACS_H_ */
