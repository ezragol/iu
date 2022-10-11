#include "upload.h"

// remove the headers from the request buffer and put the remaining content into the given file location
// little messy
int upload_file(hashmap headers, char **response_body, hashmap params)
{
    char content_length_str[MAX_HEADER_SIZE];
    int content_length;
    char location[MAX_REQUEST_PATH];

    if (!read_item("upload_path", location, params))
    {
        fprintf(stderr, "   >> malformed upload-path, not writing to filen\n");
        return 1;
    }

    if (read_item("Content-Length", content_length_str, headers))
        content_length = atoi(content_length_str);
    else
        content_length = 0;

    if (content_length > 0)
    {
        hash *body = get_item("client_body", params);
        if (body == NULL)
        {
            fprintf(stderr, "   >> there was an error while processing the client's message\n");
            return 1;
        }

        FILE *fr = fopen(location, "wb");
        if (errno != 0)
        {
            perror("   >> while opening file");
            return 1;
        }

        fwrite(body->value, sizeof(char), content_length, fr);
        if (errno != 0)
        {
            perror("   >> while writing to file");
            return -1;
        }

        printf("  >> uploaded %d bytes to %s <<\n", content_length, location);
        fclose(fr);
        *response_body = calloc(30 + strlen(location), sizeof(char));
        sprintf(*response_body, "Uploaded attached file to \"%s\"\n", location);
        return 1;
    }

    *response_body = calloc(18, sizeof(char));
    strcpy(*response_body, "No file attached\n");
    printf("  >> request did not have a body, not writing anything << \n");
    return 0;
}