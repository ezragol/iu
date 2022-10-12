#include "headers.h"

// strip headers from the given request buffer
int parse_headers(char *buf, hashmap *headers, int buf_size)
{
    char next_set[4] = "    ";
    int header_size = 0, last, nl_locations[MAX_HEADER_COUNT], dl_locations[MAX_HEADER_COUNT] = {0};

    if (buf_size < 4)
    {
        fprintf(stderr, " >> client request is too short\n");
        return -1;
    }

    while (strcmp(next_set, HEADERS_END) != 0 && header_size < buf_size)
    {
        strncpy(next_set, buf + header_size, 4);
        header_size++;
    }

    for (int i = 0; i < header_size; i++)
    {
        if (buf[i] == NEWLINE)
            nl_locations[headers->size++] = i + 1;
        else if (buf[i] == DELIMITER && headers->size >= 1 && dl_locations[headers->size - 1] == 0)
            dl_locations[headers->size - 1] = i;
    }

    last = nl_locations[2];
    headers->items = calloc(headers->size + 2, sizeof(hash));
    int path_start = 0, path_length;

    for (int i = 0; i < last; i++)
    {
        if (buf[i] == ' ' && !path_start)
            path_start = i;
        else if (buf[i] == ' ')
            path_length = i - path_start;
    }

    hash *method_ptr = &headers->items[0];
    hash *path_ptr = &headers->items[1];

    method_ptr->key = strdup("method");
    path_ptr->key = strdup("path");
    method_ptr->value = strndup(buf, path_start);
    path_ptr->value = strndup(buf + path_start + 1, path_length - 1);

    for (int i = 2; i < headers->size; i++)
    {
        int valuel, keylen = dl_locations[i] - nl_locations[i];
        hash *h_ptr = &headers->items[i];

        if (i < headers->size - 1)
            valuel = nl_locations[i + 1] - dl_locations[i] - 4;
        else
            valuel = header_size - dl_locations[i] - 3;

        h_ptr->key = calloc(keylen + 1, sizeof(char));
        h_ptr->value = calloc(valuel + 1, sizeof(char));

        strncpy(h_ptr->key, buf + last, keylen);
        strncpy(h_ptr->value, buf + last + keylen + 2, valuel);
        last = nl_locations[i + 1];
    }
    return header_size + 2;
}

int add_header(char *key, char *value, char *headers)
{
    char header[MAX_HEADER_SIZE] = {0};
    header[0] = NEWLINE;
    strcat(header, key);
    strcat(header, ": ");
    strcat(header, value);
    strncat(headers, header, LARGE_HEADER_SIZE);
    return 0;
}

int time_header(char *headers)
{
    char *days[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    char *months[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
    char header[MAX_HEADER_SIZE - 6];
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    sprintf(header, "%s, %d %s %d %d:%d:%d GMT", days[tm.tm_wday], tm.tm_mday, months[tm.tm_mon], tm.tm_year + 1900, tm.tm_hour, tm.tm_min, tm.tm_sec);
    add_header("Date", header, headers);
}