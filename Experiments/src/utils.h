#ifndef LIST_H
#define LIST_H

typedef List;

List * list_new ():
void list_del (List * l);
void list_obliterate (List * l);

size_t list_size (List * l);
void * list_get (List * l, size_t idx);

void list_append (List * l, void * element);
void * list_pop (List * l);

#endif
