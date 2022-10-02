#include "headers.h"

// strip headers from the given request buffer
int cut_headers(char *buf, char *headers, char **content, int buf_size)
{
    char next_set[4] = "    ";
    int index = 0;
    if (buf_size < 4)
    {
        return -1;
    }

    while (strcmp(next_set, HEADERS_END) != 0 && index < MAX_HEADERS)
    {
        strncpy(next_set, buf + index, 4);
        index++;
    }

    if (index != 0 && index < MAX_HEADERS)
    {
        memcpy(headers, buf, index);
        return 0;
    }
    else
    {
        *content = calloc(buf_size + 1, sizeof(char));
        memcpy(*content, buf, buf_size);
    }
    return 1;
}

// read the header with the given name (key)
// messy
int read_header(char *headers, char *value, char *key)
{
    int index = 0, length = strlen(headers), keylen = strlen(key);
    int *nl_locations = calloc(1, sizeof(int)), last, nl_length = 0;

    for (int i = 0; i < length; i++)
    {
        if (headers[i] == NEWLINE)
        {
            int temp[nl_length];
            nl_length++;
            if (nl_length > 1)
            {
                memcpy(temp, nl_locations, nl_length);
                free(nl_locations);
                nl_locations = calloc(nl_length, sizeof(int));
                memcpy(nl_locations, temp, nl_length);
            }
            nl_locations[nl_length - 1] = i + 1;
        }
    }

    last = nl_locations[0];
    for (int i = 1; i < nl_length; i++)
    {
        for (int j = last; j < nl_locations[i]; j++)
        {
            for (int t = 0; t < keylen; t++)
            {
                if (headers[j + t] != key[t])
                    break;
                if (t == keylen - 1)
                {
                    int valuelen = nl_locations[i] - j - t;
                    strncpy(value, headers + j + t + 3, valuelen - 3);
                    free(nl_locations);
                    return 0;
                }
            }
            last = nl_locations[i];
        }
    }

    free(nl_locations);
    return 1;
}