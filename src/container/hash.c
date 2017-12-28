#include <stdlib.h>
#include <stdio.h>
#include <container/hash.h>
#include <container/vector.h>

void (*destroyKey)(void*) = NULL;
void (*destroyValue)(void*) = NULL;

HashNode* hash_node_init(void *key, void *value)
{
    HashNode *node = malloc(sizeof(HashNode));
    *node = (HashNode){key,value};
    return node;
}

void hash_node_destroy(HashNode *n)
{
    puts("destroying hash node");
    if (n != NULL)
    {
        if (destroyKey != NULL) destroyKey(n->key);
        if (destroyValue != NULL) destroyValue(n->value);
        free(n);
    }
}

Hash* hash_init(void *hash, void *compare)
{
    Hash *h = malloc(sizeof(Hash));
    if (hash == NULL)
    {
        h->size = 1;
        h->data = malloc(1*sizeof(Vector));
        h->hash = NULL;
        h->cmp =  (int (*)(void*,void*))compare;;
    } else {
        h->size = HASH_SIZE;
        h->data = malloc(HASH_SIZE*sizeof(Vector));
        h->hash = (int (*)(void*))hash;
        h->cmp = (int (*)(void*,void*))compare;
        for (int i=0; i<HASH_SIZE; i++) (h->data)[i] = vector_init();
    }
    return h;
}

void* hash_get_node(Hash* h, void *key)
{
    int index = h->hash(key);
    Vector *v = (h->data)[index];
    for (int i=0; i<vector_size(v); i++)
    {
        HashNode *temp = (HashNode*)vector_get(v,i);
        void *tempKey = temp->key;
        void *tempVal = temp->value;
        if (h->cmp == NULL) {
            if (tempKey == key) {
                return temp;
            }
        }
        else if (h->cmp(tempKey,key)==0) {
            return temp;
        }
    }
    return NULL;
}

void hash_set(Hash *h, void *key, void *value)
{
    HashNode *existing = hash_get_node(h,key);

    if (existing!=NULL)
    {
        existing->value = value;
        return;
    }

    int index = h->hash(key);
    Vector *v = (h->data)[index];
    HashNode *node = hash_node_init(key,value);
    vector_add(v,(void*)node);
}


void* hash_get(Hash *h, void *key)
{
    if (h == NULL) return NULL;
    HashNode* node = hash_get_node(h,key);
    if (node!=NULL) return node->value;
    return NULL;
}

void hash_destroy(Hash *h, void *key, void *value)
{
    if (h == NULL) return;
    destroyKey = (void (*)(void*))key;
    destroyValue =  (void (*)(void*))value;
    for (int i=0; i<h->size; i++)
    {
        vector_destroy((h->data)[i], hash_node_destroy);
    }
    free(h);
}
