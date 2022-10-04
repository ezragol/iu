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
    int nl_locations[MAX_HEADERS / MAX_HEADER_SIZE] = {0}, last, nl_length = 0;

    for (int i = 0; i < length; i++)
    {
        if (headers[i] == NEWLINE)
        {
            nl_locations[++nl_length] = i + 1;
        }
    }

    int start = 0;
    char method[6];
    char path[MAX_REQUEST_PATH];

    while (headers[start] != ' ' && start < nl_locations[1])
    {
        method[start] = headers[start];
        start++;
    }

    if (key == "method")
    {
        strcpy(value, method);
        return 0;
    }
    else if (key == "path")
    {
        for (int i = ++start; headers[i] != ' ' && i < nl_locations[1]; i++)
        {
            path[i - start] = headers[i];
        }
        strcpy(value, path);
        return 0;
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
                    return 0;
                }
            }
            last = nl_locations[i];
        }
    }
    return 1;
}

int read_request_path(char *headers, char *path)
{
    read_header(headers, path, "path");
}

int read_request_method(char *headers, char *method)
{
    read_header(headers, method, "method");
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