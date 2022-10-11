#include "FileSystem/path.h"
#include "Options/args.h"
#include "Socket/connection.h"
#include "Actions/upload.h"

int main(int argc, char **argv)
{

    struct addrinfo sockinfo;
    struct sockaddr_storage client_addr;
    int size = calc_argument_size(argv, argc),
        new_fd, sockfd, addr_size = sizeof client_addr;

    argument *objects[size];
    arglist args = {objects, 0};

    char upload_path[MAX_PATH_LEN], client_ip[INET6_ADDRSTRLEN], port[5];

    parse_arguments(argv, argc, &args);
    if (bind_option(args, "upload-path", upload_path, 0))
    {
        fprintf(stderr, ">> Please specify an upload path for the server!\n");
        return 1;
    }
    if (bind_option(args, "port", port, 0))
        strcpy(port, "4011");
    
    free_arguments(&args, 0);
    detail_socket(&sockinfo, sizeof sockinfo,
                  AF_UNSPEC, SOCK_STREAM, AI_PASSIVE);

    if ((sockfd = create_socket(port, &sockinfo)) == -1)
        return 1;

    while (1)
    {
        if ((new_fd = establish_connection(
                 sockfd, client_addr, addr_size, client_ip)) == -1)
        {
            fprintf(stderr, " >> error while establishing connection");
            continue;
        }

        if (!fork())
        {
            close(sockfd);
            hashmap params = {.size = 0};
            add_item("upload_path", upload_path, &params);

            int status = process_connection(new_fd, &upload_file, params);
            printf(" >> closing connection with %s <<\n\n", client_ip);
            close(new_fd);

            if (status == -1)
                exit(1);
            exit(0);
        }
        close(new_fd);
    }

    return 0;
}