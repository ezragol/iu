#include "args.h"

int calc_argument_size(char **argv, int argc)
{
    int size = 0;
    for (int i = 1; i < argc; i++)
    {
        char *current_arg = argv[i];
        int clen = strlen(argv[i]);
        if (clen > 1 && current_arg[0] == '-')
            size++;
    }
    return size;
}

int parse_arguments(char **argv, int argc, arglist *args)
{
    for (int i = 1; i < argc; i++)
    {
        size_t clen_init = strlen(argv[i]), clen = clen_init;
        char *current_arg = argv[i], *next_arg = "TRUE";

        if (i + 1 < argc)
        {
            next_arg = argv[i + 1];
        }
        if (current_arg[0] == '-' && clen > 1)
        {
            args->size++;

            if (current_arg[1] == '-')
                clen--;

            argument **parsed = &args->objects[args->size - 1];
            *parsed = malloc(sizeof(argument));
            (*parsed)->key = strdup(current_arg + (clen_init - clen + 1));
            (*parsed)->value = strdup(next_arg);
        }
    }
    return 0;
}

int get_argument_value(arglist args, char *key, char *value)
{
    for (int i = 0; i < args.size; i++)
    {
        argument *arg = args.objects[i];
        if (arg->key[0] == key[0])
        {
            strcpy(value, arg->value);
            return 0;
        }
    }
    return 1;
}

int bind_option(arglist args, char *key, char *value, int allow_true)
{
    if (get_argument_value(args, key, value) &&
            get_argument_value(args, &key[0], value) ||
        !strcmp(value, "TRUE") && !allow_true)
    {
        strncpy(value, "\0", 1);
        return 1;
    };
    return 0;
}

int free_argument(argument *arg, int dyn)
{
    free(arg->key);
    free(arg->value);
    if (dyn)
        free(arg);
    return 0;
}

int free_arguments(arglist *args, int dyn)
{
    for (int i = 0; i < args->size; i++)
    {
        free_argument(args->objects[i], 1);
    }
    if (dyn)
        free(args);
}