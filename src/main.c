#include "socket/upload.h"
#include "socket/fs.h"

int main(int argc, char **argv)
{
    struct addrinfo sockinfo;
    struct sockaddr_storage client_addr;
    detail_socket(&sockinfo, sizeof sockinfo,
                  AF_UNSPEC, SOCK_STREAM, AI_PASSIVE);

    int new_fd, sockfd;
    int addr_size = sizeof client_addr, upload_status, response_status;
    char response_body[MAX_BODY];
    char client_ip[INET6_ADDRSTRLEN], client_buf[MAX_UPLOAD], upload_path[MAX_PATH_LEN], response[MAX_HEADERS + MAX_BODY];

    resolve_path("../uploads/test.jpg", upload_path);
    if ((sockfd = create_socket(4011, &sockinfo)) == -1)
        return 1;

    while (1)
    {
        if ((new_fd = establish_connection(
                 sockfd, client_addr, addr_size, client_ip)) == -1)
        {
            perror("> while establishing connection");
            continue;
        }

        if (!fork())
        {
            close(sockfd);

            if (upload_status = upload_file(upload_path, new_fd, client_buf))
                exit(1);

            sprintf(response_body, "Uploaded attached file to \"%s\"\n", upload_path);
            create_response(response, response_body);
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