#include "upload.h"

// remove the headers from the request buffer and put the remaining content into the given file location
// little messy
char *upload_file(hashmap headers, hashmap params)
{
    char *response_body;
    hash *location_v = get_item("upload_path", params);
    hash *content_length_h = get_item("Content-Length", headers);
    int content_length = 0;

    if (location_v == NULL)
    {
        fprintf(stderr, "   >> malformed upload-path, not writing to filen\n");
        exit(1);
    }

    char *location = location_v->value;

    if (content_length_h != NULL) {
        content_length = atoi(content_length_h->value);

        hash *body = get_item("client_body", params);
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

        fwrite(body->value, sizeof(char), content_length, fr);
        if (errno != 0)
        {
            perror("   >> while writing to file");
            exit(1);
        }

        printf("  >> uploaded %dB to %s <<\n", content_length, location);
        fclose(fr);
        response_body = calloc(30 + strlen(location), sizeof(char));
        sprintf(response_body, "Uploaded attached file to \"%s\"\n", location);
    }
    else
    {
        response_body = calloc(18, sizeof(char));
        strcpy(response_body, "No file attached\n");
    }
    return response_body;
}