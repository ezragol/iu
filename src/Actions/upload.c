#include "upload.h"

// remove the headers from the request buffer and put the remaining content into the given file location
// little messy
int upload_file(char *location, int client_fd, char *client_buf)
{
    FILE *fr = fopen(location, "wb");
    headerarray headers = {.size = 0};
    if (errno != 0)
    {
        perror("> while opening file");
        return 1;
    }
    char content_length_str[MAX_HEADER_SIZE], *image_binary;
    int client_size = 0, content_length = MAX_UPLOAD, recieve_size, status = 0;

    while (client_size < content_length && (recieve_size = read(client_fd, client_buf, sizeof(char) * MAX_UPLOAD)) > 0)
    {
        int request_type;
        printf("> server recieved %d bytes\n", recieve_size);

        if (
            (request_type = cut_headers(client_buf, &headers, &image_binary, recieve_size)) == -1)
        {
            fprintf(stderr, "> error while separating file content from HTTP Request");
            return 1;
        }

        if (request_type)
        {
            fwrite(image_binary, sizeof(char), recieve_size, fr);
            if (errno != 0)
            {
                perror("> while writing to file");
                return 1;
            }
            free(image_binary);
            client_size += recieve_size;
        }
        else
        {
            if (read_header(&headers, content_length_str, "Content-Length"))
            {
                content_length = atoi(content_length_str);
            }
            else
                content_length = 0;
        }
    }

    printf("> uploaded %d bytes to %s\n", content_length, location);
    free_headers(headers);
    fclose(fr);
    return 0;
}