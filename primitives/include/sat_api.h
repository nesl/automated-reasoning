#ifndef SATAPI_H_
#define SATAPI_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/******************************************************************************
 * sat_api.h shows the function prototypes you should implement to create libsat.a
 *
 * To use the sat library, a sat state should be constructed from an input cnf file
 *
 * The sat state supports functions that are usually used to implement a sat solver,
 * such as deciding variables and learning clauses. It also supports functions for
 * obtaining stats about the input cnf, such as the number of variables and clauses.
 *
 * The functions supported by a sat state are sufficient to implement a modern sat
 * solver (also known as CDC solver).
 *
 * A sat state has a "decision level", which is incremented when a literal is
 * decided, and decremented when a literal is undecided. Hence, each decided
 * literal is associated with a decision level. Similarly, literals which are implied
 * by unit resolution are also associated with a decision level.
 *
 * A learned clause is associated with an "assertion level". The clause can be 
 * asserted (added) to a state state only when the decision level of the sat 
 * state equals to the clause's assertion level.
 ******************************************************************************/



/******************************************************************************
 * MACROS
 ******************************************************************************/

/* Testing null pointers in C*/
#define FREE(ptr) do{ \
    	free((ptr));  \
    (ptr) = NULL;     \
  }while(0)


/******************************************************************************
 * typedefs
 ******************************************************************************/

typedef char BOOLEAN; //signed

typedef unsigned long c2dSize;  //for variables, clauses, and various things
typedef signed long c2dLiteral; //for literals
typedef double c2dWmc;          //for (weighted) model count

/******************************************************************************
 * Forward declarations
 ******************************************************************************/
typedef struct var Var;
typedef struct literal Lit;
typedef struct clause Clause;



/******************************************************************************
 * Basic structures
 ******************************************************************************/
/******************************************************************************
 * Variables:
 * --You must represent variables using the following struct
 * --Variable index must start at 1, and is no greater than the number of cnf variables
 * --Index of a variable must be of type "c2dSize"
 * --The field "mark" below and its related functions should not be changed
 ******************************************************************************/

struct var {
	c2dSize index; 							//variable index
	Lit* posLit; 							// Keep track of the variable positive literal
	Lit* negLit; 							// keep track of the variable negative literal
	Clause** list_clause_of_variables;  		//	a list of clauses that have the variable inside it
	unsigned long num_of_clauses_of_variables;
	unsigned long max_size_list_of_clause_of_variables;

	/* for the non-chronological backtracking UIP */
	// the unit clause used for implying the (variable) is said to be the antecedent of this literal(variable)
	Clause* antecedent;									// antecedent clause

	BOOLEAN mark; 							//THIS FIELD MUST STAY AS IS

};


/******************************************************************************
 * Literals:
 * --You must represent literals using the following struct 
 * --Positive literals' indices range from 1 to n (n is the number of cnf variables)
 * --Negative literals' indices range from -n to -1 (n is the number of cnf variables)
 * --Index of a literal must be of type "c2dLiteral"
 ******************************************************************************/

struct literal {
	c2dLiteral sindex;									// literal index
	BOOLEAN LitState;			  						// whether it is set or not
	unsigned long decision_level; 						// if it is decided or implied what is the level of the literal in this setting
	BOOLEAN LitValue;									// whether it has value true, false, or free(not set)
	Var* variable;										// pointer the variable of the literal

	/** for the two literal watch data structure */
	unsigned long* list_of_watched_clauses;  			// List of clause indices that contain this literal as a watched literal
	unsigned long num_watched_clauses;
	unsigned long max_size_list_watched_clauses;


	/** For variable order algorithm VSIDS*/
	unsigned long vsids_score; 							// for use in variable selection

};


/******************************************************************************
 * Clauses: 
 * --You must represent clauses using the following struct 
 * --Clause index must start at 1, and is no greater than the number of cnf clauses
 * --Index of a clause must be of type "c2dSize"
 * --A clause must have an array consisting of its literals
 * --The index of literal array must start at 0, and is less than the clause size
 * --The field "mark" below and its related functions should not be changed
 ******************************************************************************/

struct clause {

	c2dSize cindex;  						//clause index

  	Lit** literals; 						//literal array
  	unsigned long num_literals_in_clause;
	unsigned long max_size_list_literals;

	BOOLEAN is_subsumed;

	/** For the two literal watch data structure. The two literals that the clause is watching*/
	Lit* L1; //first literal
	Lit* L2; //second literal

	BOOLEAN mark; //THIS FIELD MUST STAY AS IS

};

/******************************************************************************
 * SatState: 
 * --The following structure will keep track of the data needed to
 * condition/uncondition variables, perform unit resolution, and so on ...
 ******************************************************************************/

typedef struct sat_state_t {

	  Clause* delta;
	  unsigned long* gamma; // Gamma is just a list of indices of clauses that were added to delta to avoid having double copy of the same clause
	  Lit** decisions;
	  Lit** implications;
	  Clause* alpha;

	  // Variables in the problem space and their positive and negative literals.
	  // keep them here now because I don't know where else to allocate them
	  Var* variables; 															//Array of variables

	  unsigned long  num_clauses_in_delta;  // m + added any new clauses
	  unsigned long  num_clauses_in_gamma;
	  unsigned long  num_literals_in_decision;
	  unsigned long  num_literals_in_implications;
	  unsigned long  num_variables_in_cnf; //n
	  unsigned long  num_clauses_in_cnf;

	  unsigned long max_size_list_gamma;
	  unsigned long max_size_list_delta; 	//TODO: (may need to remove this) needs to update size of delta by adding the gamma clauses to it

	  unsigned long current_decision_level;

	  Clause* conflict_clause; // if contradiction happen at the current state then this clause is the cause of contradiction


} SatState;

/******************************************************************************
 * API: 
 * --Using the above structures you must implement the following functions 
 * --Incomplete implementations of the functions can be found in sat_api.c
 * --These functions are all you need for the knowledge compiler
 * --You must implement each function below
 * --Note that most of the functions can be implemented in 1 line or so
 ******************************************************************************/

/******************************************************************************
 * function prototypes 
 ******************************************************************************/

/******************************************************************************
 * Variables
 ******************************************************************************/

//returns a variable structure for the corresponding index
Var* sat_index2var(c2dSize index, const SatState* sat_state);

//returns the index of a variable
c2dSize sat_var_index(const Var* var);

//returns the variable of a literal
Var* sat_literal_var(const Lit* lit);

//returns 1 if the variable is instantiated, 0 otherwise
//a variable is instantiated either by decision or implication (by unit resolution)
BOOLEAN sat_instantiated_var(const Var* var);

//returns 1 if all the clauses mentioning the variable are subsumed, 0 otherwise
BOOLEAN sat_irrelevant_var(const Var* var);

//returns the number of variables in the cnf of sat state
c2dSize sat_var_count(const SatState* sat_state);

//returns the number of clauses mentioning a variable
//a variable is mentioned by a clause if one of its literals appears in the clause
c2dSize sat_var_occurences(const Var* var);

//returns the index^th clause that mentions a variable
//index starts from 0, and is less than the number of clauses mentioning the variable
//this cannot be called on a variable that is not mentioned by any clause
Clause* sat_clause_of_var(c2dSize index, const Var* var);

/******************************************************************************
 * Literals 
 ******************************************************************************/
//Added API: returns 1 if the literal is asserted in the current context, 0 otherwise
BOOLEAN sat_is_asserted_literal(Lit* lit);
//Added API returns 1 is the literal is resolved in the current context, 0 otherwise
BOOLEAN sat_is_resolved_literal(Lit* lit);

//returns a literal structure for the corresponding index
Lit* sat_index2literal(c2dLiteral index, const SatState* sat_state);

//returns the index of a literal
c2dLiteral sat_literal_index(const Lit* lit);

//returns the positive literal of a variable
Lit* sat_pos_literal(const Var* var);

//returns the negative literal of a variable
Lit* sat_neg_literal(const Var* var);

//returns 1 if the literal is implied, 0 otherwise
//a literal is implied by deciding its variable, or by inference using unit resolution
BOOLEAN sat_implied_literal(const Lit* lit);


//sets the literal to true, and then runs unit resolution
//returns a learned clause if unit resolution detected a contradiction, NULL otherwise
Clause* sat_decide_literal(Lit* lit, SatState* sat_state);

//undoes the last literal decision and the corresponding implications obtained by unit resolution
void sat_undo_decide_literal(SatState* sat_state);

/******************************************************************************
 * Clauses 
 ******************************************************************************/
#ifdef DEBUG
void print_clause(Clause* clause);
void print_all_clauses(SatState* sat_state);
#endif

//returns a clause structure for the corresponding index
Clause* sat_index2clause(c2dSize index, const SatState* sat_state);

//returns the index of a clause
c2dSize sat_clause_index(const Clause* clause);

//returns the literals of a clause
Lit** sat_clause_literals(const Clause* clause);

//returns the number of literals in a clause
c2dSize sat_clause_size(const Clause* clause);

//returns 1 if the clause is subsumed, 0 otherwise
BOOLEAN sat_subsumed_clause(const Clause* clause);

//returns the number of clauses in the cnf of sat state
c2dSize sat_clause_count(const SatState* sat_state);

//returns the number of learned clauses in a sat state (0 when the sat state is constructed)
c2dSize sat_learned_clause_count(const SatState* sat_state);

//adds clause to the set of learned clauses, and runs unit resolution
//returns a learned clause if unit resolution finds a contradiction, NULL otherwise
//
//this function is called on a clause returned by sat_decide_literal() or sat_assert_clause()
//moreover, it should be called only if sat_at_assertion_level() succeeds
Clause* sat_assert_clause(Clause* clause, SatState* sat_state);

//Added API: Update the state of the clause if one of its literals is asserted
void sat_update_clauses_state(Lit* lit);

//Added API: Undo the state of the clause at backtracking when we do undo unit resolution
void sat_undo_clauses_state(Lit* lit);

/******************************************************************************
 * SatState
 ******************************************************************************/

//constructs a SatState from an input cnf file
SatState* sat_state_new(const char* file_name);

//frees the SatState
void sat_state_free(SatState* sat_state);

void variable_cleanup (Var * variable);
void literal_free (Lit * literal);

//applies unit resolution to the cnf of sat state
//returns 1 if unit resolution succeeds, 0 if it finds a contradiction
BOOLEAN sat_unit_resolution(SatState* sat_state);

//undoes sat_unit_resolution(), leading to un-instantiating variables that have been instantiated
//after sat_unit_resolution()
void sat_undo_unit_resolution(SatState* sat_state);

//returns 1 if the decision level of the sat state equals to the assertion level of clause,
//0 otherwise
//
//this function is called after sat_decide_literal() or sat_assert_clause() returns clause.
//it is used to decide whether the sat state is at the right decision level for adding clause.
BOOLEAN sat_at_assertion_level(const Clause* clause, const SatState* sat_state);

/******************************************************************************
 * The functions below are already implemented for you and MUST STAY AS IS
 ******************************************************************************/

//returns the weight of a literal (which is simply 1 for our purposes)
c2dWmc sat_literal_weight(const Lit* lit);

//returns 1 if a variable is marked, 0 otherwise
BOOLEAN sat_marked_var(const Var* var);

//marks a variable (which is not marked already)
void sat_mark_var(Var* var);

//unmarks a variable (which is marked already)
void sat_unmark_var(Var* var);

//returns 1 if a clause is marked, 0 otherwise
BOOLEAN sat_marked_clause(const Clause* clause);

//marks a clause (which is not marked already)
void sat_mark_clause(Clause* clause);

//unmarks a clause (which is marked already)
void sat_unmark_clause(Clause* clause);

#endif //SATAPI_H_

/******************************************************************************
 * end
 ******************************************************************************/
