#include "sat_api.h"


/******************************************************************************
 * SAT solver 
 ******************************************************************************/

//returns a literal which is free in the current setting of sat state  
//a NAIVE implementation no one would use in practice
//you are free to modify this (no need though)
Lit* get_free_literal(SatState* sat_state) {
  c2dSize var_count = sat_var_count(sat_state);
  for(c2dSize i=0; i<var_count; i++) { //go over variables
    Var* var  = sat_index2var(i+1,sat_state); //note index is i+1, not i
    Lit* plit = sat_pos_literal(var);
    Lit* nlit = sat_neg_literal(var);
    if(!sat_implied_literal(plit) && !sat_implied_literal(nlit)){
#ifdef DEBUG
    	printf("Decided Lit: %ld\n", plit->sindex);
    	return plit;
#endif
    }
  }
#ifdef DEBUG
  printf("All literals are implied -- no more free literal\n");
#endif
  return NULL; //all literals are implied
}

//if sat state is shown to be satisfiable, it returns NULL
//otherwise, a clause must be learned and it is returned
Clause* sat_aux(SatState* sat_state) {
  Lit* lit = get_free_literal(sat_state);
  if(lit==NULL) return NULL; //all literals are implied

  Clause* learned = sat_decide_literal(lit,sat_state);
  if(learned==NULL) learned = sat_aux(sat_state);
  	  sat_undo_decide_literal(sat_state); //I commented this

  if(learned!=NULL) { //there is a conflict
    if(sat_at_assertion_level(learned,sat_state)) {
      learned = sat_assert_clause(learned,sat_state);
      if(learned==NULL) return sat_aux(sat_state); //try again
      else return learned; //new clause learned, backtrack
    }
    else return learned; //backtrack (still conflict)
  }
  return NULL; //satisfiable
}

BOOLEAN sat(SatState* sat_state) {
  BOOLEAN ret = 0;
  if(sat_unit_resolution(sat_state)) ret = (sat_aux(sat_state)==NULL? 1: 0);
  sat_undo_unit_resolution(sat_state); // everything goes back to the initial state
  return ret;
}

int main(int argc, char* argv[]) {	
  char USAGE_MSG[] = "Usage: ./sat -c <cnf_file>\n";
  char* cnf_fname  = NULL;

  if(argc==3 && strcmp("-c",argv[1])==0) cnf_fname = argv[2];
  else {
    printf("%s",USAGE_MSG);
    exit(1);
  }
	
  //construct a sat state and then check satisfiability
  SatState* sat_state = sat_state_new(cnf_fname);
#ifdef DEBUG
  printf("END OF SAT_STATE_CONSTRUCT");
  print_all_clauses(sat_state);
#endif

#ifdef DEBUG
	  /* For test of two literal watch */
//  sat_decide_literal(sat_state->variables[0].negLit, sat_state); // -V1
//  sat_decide_literal(sat_state->variables[3].posLit, sat_state); // V4
//  sat_decide_literal(sat_state->variables[4].negLit, sat_state); //-V5


//	  /*test for conflict driven clause */
//  	  Clause* learned = sat_decide_literal(sat_state->variables[0].posLit, sat_state);  // A
//  //	  print_all_clauses(sat_state);
//  	  if(learned == NULL)
//  		  learned = sat_decide_literal(sat_state->variables[1].posLit, sat_state); //B
//  	//  print_all_clauses(sat_state);
//
//  	  if(learned == NULL)
//  		  learned = sat_decide_literal(sat_state->variables[2].posLit, sat_state); //C
//  	//  print_all_clauses(sat_state);
//
//  	  if(learned == NULL)
//  		  learned = sat_decide_literal(sat_state->variables[3].posLit, sat_state); //X
//  	//  print_all_clauses(sat_state);
//
//  	  if(learned != NULL){
//		  sat_undo_decide_literal(sat_state);
//		  if(sat_at_assertion_level(learned,sat_state)) {
//		        learned = sat_assert_clause(learned,sat_state);
//		  }
//	  }

#endif


	  if(sat(sat_state))
		  printf("SAT\n");
	  else
		  printf("UNSAT\n");



#ifdef DEBUG
//	  decide_literal(sat_state->variables[0].negLit, sat_state); // -V1
//	  decide_literal(sat_state->variables[3].posLit, sat_state); // V4
//	  decide_literal(sat_state->variables[4].negLit, sat_state); //-V5

	  for(unsigned long i = 0; i < sat_state->num_literals_in_decision; i++){
		  printf("%ld\n",sat_state->decisions[i]->sindex);
	  }
#endif

#ifdef DEBUG
  printf("END OF sat\n");
#endif

  sat_state_free(sat_state);
  return 0;
}

/******************************************************************************
 * end
 ******************************************************************************/
