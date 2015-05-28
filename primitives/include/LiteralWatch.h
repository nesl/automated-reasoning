/*
 * LiteralWatch.h
 *
 *  Created on: May 19, 2015
 *      Author: salma
 */
#ifndef LITERALWATCH_H_
#define LITERALWATCH_H_

#include "sat_api.h"


void add_watching_clause(Clause* clause, Lit* lit); //used in the conflict analysis as well
BOOLEAN two_literal_watch(SatState* sat_state);
void clear_init_literal_watch(); //used in undo unit resolution

#endif /* LITERALWATCH_H_ */




