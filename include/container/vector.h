#ifndef VECTOR_H
#define VECTOR_H

typedef struct
{
    int size;
    int capacity;
    void **data;
} Vector;

Vector* vector_init(void);
//adds the result of the function to the vector until the function returns null
Vector* vector_init_with_function(void*);
void vector_add(Vector *v, void *data);
int vector_size(Vector *v);
void* vector_get(Vector *l, int i);
void vector_destroy(Vector *v, void *destroy);

#endif
