/*
 * VSIDS.h
 *
 *  Created on: May 22, 2015
 *      Author: salma
 */

#ifndef VSIDS_H_
#define VSIDS_H_

#include "sat_api.h"

void initialize_vsids_scores (SatState * sat_state);
void update_vsids_scores (SatState * sat_state);
Lit* vsids_get_free_literal (SatState* sat_state);

#endif /* VSIDS_H_ */
