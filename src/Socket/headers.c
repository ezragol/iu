#include "headers.h"

// strip headers from the given request buffer
int cut_headers(char *buf, headerarray *headers, char **content, int buf_size)
{
    char next_set[4] = "    ";
    int header_size = 0, last, nl_locations[MAX_HEADER_COUNT], dl_locations[MAX_HEADER_COUNT] = {0};

    if (buf_size < 4)
    {
        return -1;
    }

    while (strcmp(next_set, HEADERS_END) != 0 && header_size < MAX_HEADERS)
    {
        strncpy(next_set, buf + header_size, 4);
        header_size++;
    }

    if (header_size != 0 && header_size < MAX_HEADERS)
    {
        for (int i = 0; i < header_size; i++)
        {
            if (buf[i] == NEWLINE)
                nl_locations[headers->size++] = i + 1;
            else if (buf[i] == DELIMITER && headers->size >= 1 && dl_locations[headers->size - 1] == 0)
                dl_locations[headers->size - 1] = i;
        }

        last = nl_locations[0];
        headers->content = calloc(headers->size, sizeof(header));

        for (int i = 0; i < headers->size; i++)
        {
            int valuel, keylen = dl_locations[i] - nl_locations[i];
            header *h_ptr = &headers->content[i];

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
int read_header(headerarray *headers, char *value, char *key)
{
    for (int i = 0; i < headers->size; i++)
    {
        header h_ptr = headers->content[i];
        if (strcmp(h_ptr.key, key) == 0)
        {
            strcpy(value, h_ptr.value);
            return 1;
        }
    }
    return 0;
}

int free_headers(headerarray headers)
{
    for (int i = 0; i < headers.size; i++)
    {
        header h_ptr = headers.content[i];
        free(h_ptr.key);
        free(h_ptr.value);
    }
    free(headers.content);
    return 0;
}

int response_header(char *key, char *value, char *headers)
{
    char header[MAX_HEADER_SIZE];
    sprintf(header, "\n%s: %s", key, value);
    strncat(headers, header, MAX_HEADER_SIZE);
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
    response_header("Date", header, headers);
}