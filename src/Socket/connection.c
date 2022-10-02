#include "connection.h"

// https://beej.us/guide/bgnet/html/#a-simple-stream-server

// wait for the child process (sig) to finish, so that establish_connection can reuse it
void sigchld_handler(int sig)
{
    int errno_copy = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0)
        ;
    errno = errno_copy;
}

// copy port (int) to port_str (string)
int translate_port(char *port_str, int port)
{
    if (port < 0 || port > 65536)
        return 0;

    sprintf(port_str, "%d", port);
    return 1;
}

// return the sin(6)_addr field of the given socket address (ipv4 and ipv6)
void *read_client_ip(struct sockaddr *sock_addr)
{
    if (sock_addr->sa_family == AF_INET)
        return &(((struct sockaddr_in *)sock_addr)->sin_addr);

    return &(((struct sockaddr_in6 *)sock_addr)->sin6_addr);
}

// fill out the given socket address info struct
int detail_socket(struct addrinfo *details, int size, int ai_family, int ai_socktype, int ai_flags)
{
    memset(details, 0, size);
    details->ai_family = ai_family;
    details->ai_socktype = ai_socktype;
    details->ai_flags = ai_flags;
    return 0;
}

// attach headers to response
int create_response(char *response, char *body)
{
    int length = strlen(body);
    snprintf(response, MAX_BODY, "%s%d%s\r\n\r\n%s",
             "HTTP/1.1 200 OK\nContent-Length: ", length, "\nContent-Type: text/plain; charset=utf-8", body);

    return 0;
}

// get ready to listen on a socket with, and rework the child processes that will handle its requests
int create_socket(int port, struct addrinfo *details)
{
    struct addrinfo *server_info, *host;
    struct sigaction sig_action;
    char port_str[6];
    int addr_lookup, sockfd, yes = 1;

    if (!translate_port(port_str, port))
        return -1;

    // all of these are potential leaks because freeaddrinfo doesn't get called, however it sort of exits either way so
    if ((addr_lookup = getaddrinfo(NULL, port_str, details, &server_info)) != 0)
    {
        fprintf(stderr, "> couldn't load address info: %s\n", gai_strerror(addr_lookup));
        return -1;
    }

    for (host = server_info; host != NULL; host->ai_next)
    {
        if ((sockfd = socket(host->ai_family, host->ai_socktype, host->ai_protocol)) == -1)
        {
            perror("> while creating server socket");
            return -1;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        {
            perror("> while allowing socket reuse");
            return -1;
        }

        if (bind(sockfd, host->ai_addr, host->ai_addrlen) == -1)
        {
            close(sockfd);
            perror("> while assigning server socket");
            continue;
        }

        break;
    }

    freeaddrinfo(server_info);
    if (host == NULL)
    {
        fprintf(stderr, "> couldn't find a suitable method for binding server\n");
        return -1;
    }

    if (listen(sockfd, 10) == -1)
    {
        perror("> while listening to server socket");
        return -1;
    }

    sig_action.sa_handler = sigchld_handler;
    sigemptyset(&sig_action.sa_mask);
    sig_action.sa_flags = SA_RESTART;

    if (sigaction(SIGCHLD, &sig_action, NULL) == -1)
    {
        perror("> while rewriting the child process");
        return -1;
    }

    printf("> listening for connections on localhost:%d\n", port);
    return sockfd;
}

// listen on the given socket file descriptor
int establish_connection(int sockfd, struct sockaddr_storage client_addr, int size, char *client_ip)
{
    int new_fd = accept(sockfd, (struct sockaddr *)&client_addr, &size);
    int ip_size = sizeof(char) * INET6_ADDRSTRLEN;
    if (new_fd == -1)
    {
        perror("> while opening new connection");
        return -1;
    }

    inet_ntop(client_addr.ss_family,
              read_client_ip((struct sockaddr *)&client_addr), client_ip, ip_size);
    printf("\n> established connection with %s\n", client_ip);
    return new_fd;
}

// send a response to the client socket file descriptor
int send_response(int client_fd, char *content)
{
    int length = strlen(content), bytes;

    if ((bytes = send(client_fd, content, length, 0)) == -1)
        return -1;

    printf("> sending response\n");
    return 0;
}