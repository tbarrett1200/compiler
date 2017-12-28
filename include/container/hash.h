#ifndef HASH_MAP_H
#define HASH_MAP_H

#include <container/vector.h>

struct Hash;

#define HASH_SIZE 256

typedef struct
{
    void *key;
    void *value;
} HashNode;

typedef struct Hash
{
    int size;
    Vector **data;
    int (*cmp)(void*, void*);
    int (*hash)(void*);
} Hash;

Hash* hash_init(void *hash, void *compare);
void hash_set(Hash*, void *key, void *value);
void* hash_get(Hash*, void *key);
void hash_destroy(Hash *h, void *destroyKey, void *desroyValue);

#endif /* hash_map.h */
