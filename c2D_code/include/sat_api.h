/******************************************************************************
 * The c2D Compiler Package
 * c2D version 1.00, May 24, 2015
 * http://reasoning.cs.ucla.edu/c2d
 ******************************************************************************/

#ifndef SATAPI_H_
#define SATAPI_H_

/******************************************************************************
 * sat_api.h shows the function prototypes implemented in libsat.a
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
 * function prototypes 
 ******************************************************************************/

/******************************************************************************
 * Variables
 ******************************************************************************/

//
//each variable has an index describing the variable such that
//(0 < index <= n), where n is the number of cnf variables
//that is, the index is greater than 0, and no greater than the number of cnf variables
//

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

//returns the number of variables in the cnf
c2dSize sat_var_count(const SatState* sat_state);

//returns the number of clauses mentioning a variable
//a variable is mentioned by a clause if one of its literals appears in the clause
c2dSize sat_var_occurences(const Var* var);

//returns the index^th clause that mentions a variable
//index starts from 0, and is less than the number of clauses mentioning the variable
//this cannot be called on a variable that is not mentioned by any clause
Clause* sat_clause_of_var(c2dSize index, const Var* var);

//returns 1 if a variable is marked, 0 otherwise
BOOLEAN sat_marked_var(const Var* var);

//marks a variable (which is not marked already)
void sat_mark_var(Var* var);

//unmarks a variable (which is marked already)
void sat_unmark_var(Var* var);

/******************************************************************************
 * Literals 
 ******************************************************************************/

//
//there are two types of literals: positive literals and negative literals
//each literal has an index describing the literal such that:
//[-n,...,-1] --> for negative literals where n is the number of cnf variables
//[1,...,n]   --> for positive literals where n is the number of cnf variables
//

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

//returns the weight of the literal (default weight is 1)
c2dWmc sat_literal_weight(const Lit* lit);

//sets the literal to true, and then runs unit resolution
//returns a learned clause if unit resolution detected a contradiction, NULL otherwise
Clause* sat_decide_literal(Lit* lit, SatState* sat_state);

//undoes the last literal decision and the corresponding implications obtained by unit resolution
void sat_undo_decide_literal(SatState* sat_state);

/******************************************************************************
 * Clauses 
 ******************************************************************************/

//
//each clause has an index describing the clause such that:
//(0 < index <= m), where m is the number of cnf clauses
//that is, the index is greater than 0, and no greater than the number of cnf clauses 
//
//a clause has an array consisting of its literals
//the array index starts from 0, and is less than the clause size
//

//returns a clause structure for the corresponding index 
Clause* sat_index2clause(c2dSize index, const SatState* sat_state);

//returns the index of a clause
c2dSize sat_clause_index(const Clause* clause);

//returns the literals of the clause
Lit** sat_clause_literals(const Clause* clause);

//returns the number of literals in the clause
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

//returns 1 if a clause is marked, 0 otherwise
BOOLEAN sat_marked_clause(const Clause* clause);

//marks a clause (which is not marked already)
void sat_mark_clause(Clause* clause);

//unmarks a clause (which is marked already)
void sat_unmark_clause(Clause* clause);

/******************************************************************************
 * SatState
 ******************************************************************************/

//constructs a SatState from an input cnf file
SatState* sat_state_new(const char* file_name);

//frees the SatState
void sat_state_free(SatState* sat_state);

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

#endif //SATAPI_H_

/******************************************************************************
 * end
 ******************************************************************************/
