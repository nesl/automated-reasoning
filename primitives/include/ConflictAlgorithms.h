/*
 * ConflictAlgorithms.h
 *
 *  Created on: May 22, 2015
 *      Author: salma
 */

#ifndef CONFLICTALGORITHMS_H_
#define CONFLICTALGORITHMS_H_

#include "sat_api.h"

void add_clause_to_gamma(SatState* sat_state);


//void chronological_backtracking(SatState* sat_state);

Clause* CDCL_non_chronological_backtracking_first_UIP(SatState* sat_state);


#endif /* CONFLICTALGORITHMS_H_ */
