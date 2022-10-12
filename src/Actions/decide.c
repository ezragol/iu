#include "decide.h"

char *decide_outcome(hashmap request_headers, hashmap params)
{
    hash *method = get_item("client_method", params);
    hash *path = get_item("client_method", params);

    if (strcmp(method->value, "POST") == 0 && strcmp(path->value, "/upload") == 0)
        return upload_file(request_headers, params);
    return strdup("404 not found");
}