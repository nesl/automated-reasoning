#include "stdlib.h"
#include "stdio.h"

/****
 * This struct represents a "List"
 *
 * It does not currently fully implement a real list
 * It implements a stack, but also allows you to READ any index.
 * In other words, there is no arbitrary-index insertion or deletion.
 * Based on the comments in satapi.c I don't think those features
 * will be necessary for this project.
 */

typedef struct
{
	void ** elements;
	size_t population; // quantity of elements present
	size_t max_occupancy; // quantity of elements we have space for
} List;

List * list_new ()
{
	List * l = malloc(sizeof(List));
	l->elements = malloc(sizeof(void *)); // initialize pointer-array of size 1
	l->population = 0;
	l->max_occupancy = 1;

	return l;
}

/**
 * list_del --- deletes the list l
 *
 * Note that this does not call free() on l's elements
 *
 * Use this if the list contains pointers to things stored elsewhere
 *
 */
void list_del (List * l)
{
	free(l->elements);
	free(l);
}

/**
 * list_obliterate --- deletes the list l and everything in it
 *
 * This function frees all of l's elements before deleting l itself
 */
void list_obliterate (List * l)
{
	for (size_t idx = 0; idx < l->population; idx++)
	{
		free(l->elements[idx]);
	}

	list_del(l);
}

size_t list_size (List * l)
{
	return l->population;
}

void * list_get (List * l, size_t idx)
{
	if (idx < list_size(l))
		return l->elements[idx];
	else
	{
		fprintf(stderr, "WARNING: tried to look up an invalid list index\n");
		return NULL;
	}
}

void list_append (List * l, void * element)
{
	if ( l->population == l->max_occupancy)
	{
		l->elements = realloc(l->elements, sizeof(void *) * l->max_occupancy * 2);
		l->max_occupancy *= 2;
	}

	l->elements[l->population++] = element;
}

void * list_pop (List * l)
{
	if (0 < l->population)
		return l->elements[l->population--];
	else
	{
		fprintf(stderr, "WARNING: tried to pop from an empty list\n");
		return NULL;
	}
}

/* Some test cases
int main ()
{
	List * l = list_new();

	list_append(l, "Hello");
	list_append(l, "Hallo");
	list_append(l, "Hola");
	list_append(l, "Nihao");
	list_append(l, "Wei");
	list_append(l, "Bueno");

	for (size_t idx = 0; idx < list_size(l); idx++)
	{
		printf("List element %zu: %s\n", idx, (char *) list_get(l, idx));
	}
	printf("\n");

	list_pop(l);
	list_pop(l);
	list_pop(l);
	list_pop(l);
	list_pop(l);
	list_pop(l);
	list_pop(l);

	for (size_t idx = 0; idx < list_size(l); idx++)
	{
		printf("List element %zu: %s\n", idx, (char *) list_get(l, idx));
	}
	printf("\n");

	list_append(l, "Howdy");
	list_append(l, "Hiya");
	list_append(l, "What's up?");

	for (size_t idx = 0; idx < list_size(l) + 3; idx++)
	{
		printf("List element %zu: %s\n", idx, (char *) list_get(l, idx));
	}

	list_del(l);
	return 0;
}
*/
