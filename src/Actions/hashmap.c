#include "hashmap.h"

int add_item(char *key, char *value, hashmap *object)
{
    object->size++;
    if (object->size == 1)
        object->items = malloc(sizeof(hashmap));
    else
        object->items = realloc(object->items, object->size * sizeof(hashmap));

    if (object->items == NULL)
    {
        fprintf(stderr, " >> could not reallocate space for action paramaters!\n");
        return -1;
    }
    hash *next = &(object->items)[object->size - 1];
    next->key = strdup(key);
    next->value = value != NULL ? strdup(value) : NULL;
}

int read_item(char *key, char *value, hashmap object)
{
    for (int i = 0; i < object.size; i++)
    {
        hash next = object.items[i];
        if (strcmp(next.key, key) == 0)
        {
            strcpy(value, next.value);
            return 1;
        }
    }
    return 0;
}

hash *get_item(char *key, hashmap object)
{
    for (int i = 0; i < object.size; i++)
    {
        if (strcmp(object.items[i].key, key) == 0)
            return &object.items[i];
    }
    return NULL;
}

int free_hashmap(hashmap map)
{
    for (int i = 0; i < map.size; i++)
    {
        hash h_ptr = map.items[i];
        free(h_ptr.key);
        free(h_ptr.value);
    }
    free(map.items);
    return 0;
}