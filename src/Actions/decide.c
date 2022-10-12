#include "decide.h"

int match_request(char *method, char *path)
{
    
}

char *decide_outcome(hashmap *request_headers, hashmap *response_headers, hashmap *params)
{
    char *method = get_item_value("request_method", request_headers);
    char *path = get_item_value("request_path", request_headers);

    if (strcmp(method, "POST") == 0 && strcmp(path, "/upload") == 0)
        return upload_file(request_headers, response_headers, params);
    else {
        add_item("response_status", "404 NOT FOUND", response_headers);
        return strdup("404 not found");
    }
}