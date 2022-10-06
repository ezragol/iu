#include "FileSystem/path.h"
#include "Options/args.h"
#include "Socket/connection.h"
#include "Actions/upload.h"

int main(int argc, char **argv)
{
    int size = calc_argument_size(argv, argc);

    struct addrinfo sockinfo;
    struct sockaddr_storage client_addr;
    argument *objects[size];
    arglist args = {objects, 0};

    char upload_path[MAX_PATH_LEN];

    int new_fd, sockfd;
    int addr_size = sizeof client_addr, upload_status, response_status;

    char response_body[MAX_BODY], response_headers[MAX_HEADERS] = {0};
    char client_ip[INET6_ADDRSTRLEN], client_buf[MAX_UPLOAD], response[MAX_HEADERS + MAX_BODY];

    parse_arguments(argv, argc, &args);
    bind_option(args, "upload-path", upload_path, 1);

    free_arguments(&args, 0);
    detail_socket(&sockinfo, sizeof sockinfo,
                  AF_UNSPEC, SOCK_STREAM, AI_PASSIVE);

    if ((sockfd = create_socket(4011, &sockinfo)) == -1)
        return 1;

    while (1)
    {
        if ((new_fd = establish_connection(
                 sockfd, client_addr, addr_size, client_ip)) == -1)
        {
            fprintf(stderr, "> error while establishing connection");
            continue;
        }

        if (!fork())
        {
            close(sockfd);

            if (upload_status = upload_file(upload_path, new_fd, client_buf))
                exit(1);

            sprintf(response_body, "Uploaded attached file to \"%s\"\n", upload_path);
            create_response(response, response_headers, response_body);
            if ((response_status = send_response(new_fd, response)) == -1)
                exit(1);

            close(new_fd);
            printf("> closing connection with %s\n", client_ip);
            exit(upload_status);
        }
        close(new_fd);
    }

    return 0;
}