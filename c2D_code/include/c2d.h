/******************************************************************************
 * The c2D Compiler Package
 * c2D version 1.00, May 24, 2015
 * http://reasoning.cs.ucla.edu/c2d
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <getopt.h>
#include <assert.h>

#ifndef C2D_H_
#define C2D_H_

/******************************************************************************
 * printf controls
 ******************************************************************************/

//unsigned long, double
#define PRIvS "lu"
#define PRIwmcS "f"

/******************************************************************************
 * typedefs for c2D
 ******************************************************************************/

typedef char BOOLEAN; //signed

typedef unsigned long c2dSize;  //for variables, clauses, and various things
typedef signed long c2dLiteral; //for literals
typedef double c2dWmc;          //for (weighted) model count

//definition of BYTE should not change: it is assumed that BYTE has 8 bits
typedef unsigned char BYTE; // BYTE must be unsigned so that shifting works correctly
typedef unsigned long HASHCODE;

/******************************************************************************
 * typedefs for nnf_api 
 ******************************************************************************/

typedef unsigned long* NNF_NODE;
typedef struct nnf Nnf;
typedef struct nnf_manager NnfManager;

/******************************************************************************
 * typedefs for sat_api 
 ******************************************************************************/

typedef struct var Var;
typedef struct literal Lit;
typedef struct clause Clause;
typedef struct sat_state_t SatState;

/******************************************************************************
 * Structure for c2D options
 ******************************************************************************/

typedef struct {
  char* cnf_filename; //input cnf filename

  //vtree
  char* vtree_in_filename;  //input vtree filename
  char* vtree_out_filename; //output vtree file (.vtree, plain text)
  char* vtree_dot_filename; //output vtree file (.dot)
  char vtree_type;          //vtree type either 'i' or 'p'
  int vtree_method;         //vtree construction method either [0..3]
  int vtree_count;          //number of vtrees to be generated
  int initial_ubfs;         //initial ubfs
  int final_ubfs;           //final ubfs
  int cache_capacity;       //hash table capacity for the vtree

  //flags
  BOOLEAN in_memory;     //whether or not to save nnf to file
  BOOLEAN check_entail;  //check if the nnf entails the input cnf
  BOOLEAN count_models;  //count the models of the output nnf
  BOOLEAN model_counter; //only (weighted) model counter
  BOOLEAN help;          //help
} c2dOptions;

/******************************************************************************
 * Structure clause/variable sets
 ******************************************************************************/

//a set of cnf clauses
typedef struct clause_set_t {
  c2dSize size;
  Clause** set;
} Cset;

//a set of cnf variables
typedef struct var_set_t {
  c2dSize size;
  Var** set;
} Vset;

/******************************************************************************
 * Structure for vtree
 ******************************************************************************/

//vtree is a complete binary tree
typedef struct decision_vtree_t {
  struct decision_vtree_t* parent; //parent
  struct decision_vtree_t* left;   //left child
  struct decision_vtree_t* right;  //right child

  //position of vtree node in the vtree inorder
  c2dSize position; //start from 0
  
  c2dSize var_count; //number of variables in vtree      
  
  Var* var; //cnf variable associated with vtree (only for leaves)
  
  //cnf clause/variable sets 
  Cset* contextC;         //clauses that have variables inside and outside vtree
  Vset* context_out_vars; //vars of context clauses that are outside vtree
  Vset* context_in_vars;  //vars of context clauses that are inside  vtree
  c2dSize cached_size;    //context + 2*context_in_vars
  
  //cache
  BOOLEAN live_cache;
  BYTE* key;
  c2dSize key_size;      //how many cells/bytes in key
  HASHCODE key_hashcode; //index into hash table
  struct vtree_cache_entry_t* cache_entry;
} DVtree;

/******************************************************************************
 * Structures for vtree cache
 ******************************************************************************/

typedef union vtree_cache_value_t {
  c2dWmc   count; //to cache (weighted) model counts
  NNF_NODE node;  //to cache nnf nodes
} VtreeCV;
 
typedef struct vtree_cache_entry_t {
  DVtree* vtree;  //the vtree node that generated this entry
  BYTE* key;      //a pointer to the starting cell where the key is stored
  VtreeCV value;  //the value to which the key is mapped

  //a pointer to the next cache entry in the same collision list
  struct vtree_cache_entry_t* next;

  //keeping track of prev entry in collision list
  struct vtree_cache_entry_t** prev_next; 

  //a pointer to the next cache entry in the list of cache entries for a given vtree
  struct vtree_cache_entry_t* vtree_next;
} VtreeCE;

typedef struct {
  c2dSize capacity;  //the total number of buckets (collision lists) in cache
  VtreeCE** buckets; //the array where cache buckets are stored
  c2dSize count;     //the number of entries currently in cache
  c2dSize memory;    //the memory (in bytes) used to store cache entries
  c2dSize hits;      //the number of cache hits
  c2dSize misses;    //the number of cache misses
} VtreeCache;

/******************************************************************************
 * Structure for vtree manager
 ******************************************************************************/
 
typedef struct {
  DVtree* vtree;      //vtree associated with the manager
  VtreeCache* cache;  //cache associated with the manager
  DVtree** var_map;   //var_map[i] is the leaf vtree whose variable has index i>0
} VtreeManager;
 

/******************************************************************************
 * APIs for sat solver, nnf manager, and vtree manager
 ******************************************************************************/

#include "sat_api.h"
#include "nnf_api.h"
#include "vtree_api.h"

#endif //C2D_H_

/******************************************************************************
 * end
 ******************************************************************************/
