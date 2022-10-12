#ifndef __HEADERS__PARAMS_H_
#define __HEADERS__PARAMS_H_

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
int read_item(char *key, char *value, hashmap *object);
hash *get_item(char *key, hashmap *object);
char *get_item_value(char *key, hashmap *object);

int calc_hashmap_size(hashmap *map, int headers);
int free_hashmap(hashmap *map);

#endif