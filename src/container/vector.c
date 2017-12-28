#include <stdlib.h>
#include <stdio.h>
#include <container/vector.h>


Vector* vector_init(void)
{
    //allocates vector and initializes with default values
    Vector *v = malloc(sizeof(Vector));
    *v = (Vector){0,10,malloc(sizeof(void*) * 10)};
    return v;
}

Vector* vector_init_with_function(void *f)
{
    void*(*func)(void) = (void*(*)(void))f;
    Vector *v = vector_init();
    void *d;
    while((d=func()) != NULL) vector_add(v,d);
    return v;
}

void vector_add(Vector *v, void *data)
{
    if (v == NULL) return;

    //resizes the data array when necessary
    if (v->size == v->capacity)
    {
        v->data = realloc(v->data, sizeof(void*) * (v->capacity+10));
        v->capacity += 10;
    }

    v->data[v->size] = data;
    v->size++;
}

int vector_size(Vector *v)
{
    return v->size;
}

void* vector_get(Vector *v, int i)
{
    if (i<v->size && v>=0) return v->data[i];
    else return NULL;
}

void vector_destroy(Vector *v, void *destroy)
{
    if (v == NULL) return;

    if (destroy != NULL)
    {
        for (int i=0; i<v->size; i++)
        {
            if (v->data==NULL)
            ((void (*)(void*))destroy)(v->data[i]);
        }
    }
    free (v->data);
    free (v);
}
