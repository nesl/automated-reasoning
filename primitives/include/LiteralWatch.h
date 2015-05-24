/*
 * LiteralWatch.h
 *
 *  Created on: May 19, 2015
 *      Author: salma
 */
#ifndef LITERALWATCH_H_
#define LITERALWATCH_H_

#include "satapi.h"


void add_watching_clause(Clause* clause, Lit* lit);
BOOLEAN two_literal_watch(SatState* sat_state);


#endif /* LITERALWATCH_H_ */




