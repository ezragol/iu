#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

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
int get_argument_value(arglist *args, char *key, char *value);

int free_argument(argument *arg, int dyn);
int free_arguments(arglist *args, int dyn);