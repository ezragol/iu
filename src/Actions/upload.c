#include "upload.h"

// remove the headers from the request buffer and put the remaining content into the given file location
// little messy
char *upload_file(hashmap *request_headers, hashmap *response_headers, hashmap *params)
{
    char *response_body;
    char *location = get_item_value("upload_path", params);
    char *content_length_str = get_item_value("Content-Length", request_headers);
    int content_length = 0;

    if (location == NULL)
    {
        fprintf(stderr, "   >> malformed upload-path, not writing to filen\n");
        exit(1);
    }

    if (content_length_str != NULL) {
        content_length = atoi(content_length_str);

        char *body = get_item_value("body", params);
        if (body == NULL)
        {
            fprintf(stderr, "   >> there was an error while processing the client's message\n");
            exit(1);
        }

        FILE *fr = fopen(location, "wb");
        if (errno != 0)
        {
            perror("   >> while opening file");
            exit(1);
        }

        fwrite(body, sizeof(char), content_length, fr);
        if (errno != 0)
        {
            perror("   >> while writing to file");
            exit(1);
        }

        printf("  >> uploaded %dB to %s <<\n", content_length, location);
        fclose(fr);
        response_body = calloc(30 + strlen(location), sizeof(char));
        add_item("response_status", "200 OK", response_headers);
        sprintf(response_body, "Uploaded attached file to \"%s\"\n", location);
    }
    else
    {
        add_item("response_status", "400 BAD REQUEST", response_headers);
        response_body = calloc(18, sizeof(char));
        strcpy(response_body, "No file attached\n");
    }
    return response_body;
}