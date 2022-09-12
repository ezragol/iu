#include "socket/socket.h"
#include "socket/fs.h"

int main(int argc, char **argv)
{
    struct addrinfo sockinfo;
    struct sockaddr_storage client_addr;
    detail_socket(&sockinfo, sizeof sockinfo,
                  AF_UNSPEC, SOCK_STREAM, AI_PASSIVE);

    int new_fd, sockfd;
    int addr_size = sizeof client_addr, upload_status;
    char client_ip[INET6_ADDRSTRLEN];
    char client_buf[MAX_UPLOAD];
    char upload_path[MAX_PATH_LEN];

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
            upload_status = upload_file(upload_path, new_fd, client_buf);
            close(new_fd);
            printf("> closing connection with %s\n", client_ip);
            exit(upload_status);
        }
        close(new_fd);
    }

    return 0;
}