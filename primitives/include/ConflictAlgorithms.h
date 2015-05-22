/*
 * ConflictAlgorithms.h
 *
 *  Created on: May 22, 2015
 *      Author: salma
 */

#ifndef CONFLICTALGORITHMS_H_
#define CONFLICTALGORITHMS_H_

#include "satapi.h"

void chronological_backtracking(SatState* sat_state);

void CDCL_non_chronological_backtracking(SatState* sat_state);


#endif /* CONFLICTALGORITHMS_H_ */
