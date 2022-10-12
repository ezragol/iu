#ifndef __PARAMS_H_
#define __PARAMS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef struct s_hash {
    char *key;
    char *value;
} hash;

typedef struct s_hashmap {
    hash *items;
    unsigned int size;
} hashmap;

int add_item(char *key, char *value, hashmap *object);
int read_item(char *key, char *value, hashmap object);
hash *get_item(char *key, hashmap object);
int free_hashmap(hashmap map);

#endif