/******************************************************************************
 * The c2D Compiler Package
 * c2D version 1.00, May 24, 2015
 * http://reasoning.cs.ucla.edu/c2d
 ******************************************************************************/

#ifndef VTREEAPI_H_
#define VTREEAPI_H_

/******************************************************************************
 * vtree_api.h shows the function prototypes implemented in libvtree.a
 *
 * To use the vtree library, one should first construct a vtree manager based on
 * a sat state (see sat_api.h)
 *
 * This vtree manager also constructs a vtree, which will be freed when the manager 
 * is freed. The details of the vtree structure can be found in c2d.h. Using
 * this structure, one can easily play with vtrees (e.g, vtree->left, vtree->right 
 * to access the children of an internal vtree node, or vtree->var to obtain the 
 * variable of a leaf vtree node)
 ******************************************************************************/

/******************************************************************************
 * function prototypes 
 ******************************************************************************/

//creates a new vtree manager given a sat state and some user-specified options
//this also constructs a vtree for the cnf embedded in a state state
//
//the following fields of the options structure used when constructing a vtree manager:
//--vtree_in_filename
//--vtree_type
//--vtree_method
//--vtree_count
//--initial_ubfs
//--final_ubfs
//--cache_capacity (used for component caching)
VtreeManager* vtree_manager_new(const SatState* sat_state, const c2dOptions* options);

//frees the vtree manager 
void vtree_manager_free(VtreeManager* manager);

//saves the vtree as a .vtree file (with a header describing the file format)
void vtree_save(const char* fname, const DVtree* vtree);

//saves the vtree as a dot file
void vtree_save_as_dot(const char* fname, const DVtree* vtree);

//prints the widths of the vtree
void vtree_print_widths(const DVtree* vtree);

//returns 1 if vtree is a leaf, 0 otherwise
BOOLEAN vtree_is_leaf(const DVtree* vtree);

//returns 1 if vtree is a Shannon vtree node (its left child is a leaf), 0 otherwise
BOOLEAN vtree_is_shannon_node(const DVtree* vtree);

//returns the Shannon variable of a Shannon vtree node (i.e., the variable of its left child)
Var* vtree_shannon_var(const DVtree* vtree);

#endif //VTREEAPI_H_

/******************************************************************************
 * end
 ******************************************************************************/
