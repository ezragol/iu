#ifndef __OPTIONS__ARGS_H_
#define __OPTIONS__ARGS_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef MAX_PATH_LEN
#define MAX_PATH_LEN 1024
#endif

#define MAX_NAMING_SCHEME_LEN 128

typedef struct s_argument {
    char *key;
    char *value;
} argument;

typedef struct s_arguments {
    argument **objects;
    int size;
} arglist;

int calc_argument_size(char **argv, int argc);
int parse_arguments(char **argv, int argc, arglist *args);
int get_argument_value(arglist args, char *key, char *value);

int bind_option(arglist args, char *key, char *value, int allow_true);

int free_argument(argument *arg, int dyn);
int free_arguments(arglist *args, int dyn);

#endif