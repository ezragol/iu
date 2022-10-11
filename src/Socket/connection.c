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
int create_response(char **response, char *headers, char *body, int status, char *message)
{
    int b_len = strlen(body) + 1;

    add_header("Server", "localhost", headers);
    sprintf(headers, "%s\nContent-Length: %d", headers, b_len);
    add_header("Content-Type", "text/plain; charset=utf-8", headers);
    time_header(headers);

    int h_len = strlen(headers), m_len = strlen(message);
    *response = calloc(b_len + h_len + m_len + 17, sizeof(char));
    sprintf(*response, "HTTP/1.1 %d %s%s\r\n\r\n%s", status, message, headers, body);

    return 0;
}

// get ready to listen on a socket with, and rework the child processes that will handle its requests
int create_socket(char port[5], struct addrinfo *details)
{
    struct addrinfo *server_info, *host;
    struct sigaction sig_action;
    int addr_lookup, sockfd, yes = 1;

    // all of these are potential leaks because freeaddrinfo doesn't get called, however it sort of exits either way so
    if ((addr_lookup = getaddrinfo(NULL, port, details, &server_info)) != 0)
    {
        fprintf(stderr, ">> couldn't load address info: %s\n", gai_strerror(addr_lookup));
        return -1;
    }

    for (host = server_info; host != NULL; host->ai_next)
    {
        if ((sockfd = socket(host->ai_family, host->ai_socktype, host->ai_protocol)) == -1)
        {
            perror(">> while creating server socket");
            return -1;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        {
            perror(">> while allowing socket reuse");
            return -1;
        }

        if (bind(sockfd, host->ai_addr, host->ai_addrlen) == -1)
        {
            close(sockfd);
            perror(">> while assigning server socket");
            return -1;
        }

        break;
    }

    freeaddrinfo(server_info);
    if (host == NULL)
    {
        fprintf(stderr, ">> couldn't find a suitable method for binding server\n");
        return -1;
    }

    if (listen(sockfd, 10) == -1)
    {
        perror(">> while listening to server socket");
        return -1;
    }

    sig_action.sa_handler = sigchld_handler;
    sigemptyset(&sig_action.sa_mask);
    sig_action.sa_flags = SA_RESTART;

    if (sigaction(SIGCHLD, &sig_action, NULL) == -1)
    {
        perror(">> while rewriting the child process");
        return -1;
    }

    printf(">> listening for connections on localhost:%s <<\n", port);
    return sockfd;
}

// listen on the given socket file descriptor
int establish_connection(int sockfd, struct sockaddr_storage client_addr, int size, char *client_ip)
{
    int new_fd = accept(sockfd, (struct sockaddr *)&client_addr, &size);
    int ip_size = sizeof(char) * INET6_ADDRSTRLEN;
    if (new_fd == -1)
    {
        perror(" >> while opening new connection ");
        return -1;
    }

    inet_ntop(client_addr.ss_family,
              read_client_ip((struct sockaddr *)&client_addr), client_ip, ip_size);
    printf("\n >> established connection with %s <\n", client_ip);
    return new_fd;
}

int process_connection(int client_fd, int (*action)(hashmap, char **, hashmap), hashmap params)
{
    hashmap request_headers = {.size = 0};
    hash *client_body;

    char content_length_str[MAX_HEADER_SIZE] = {0}, h_buf[MAX_UPLOAD] = {0},
         response_headers[MAX_HEADER_SIZE * MAX_HEADER_COUNT] = {0};

    char *response, *response_body;
    int recieved = 0, current_recv = 1, status = 0, content_length = 0, got_headers = 0, finished = 0;
    int callback_status, response_status;

    while (!finished)
    {
        if (!got_headers)
        {
            got_headers = 1;
            int header_length = read(client_fd, h_buf, MAX_UPLOAD);
            if (header_length == 0 || parse_headers(h_buf, &request_headers, header_length) == -1)
            {
                fprintf(stderr, "   >> error while reading request headers\n");
                return -1;
            }

            printf("  >> client sent request headers to server (%d bytes) << \n", header_length);

            if (read_item("Content-Length", content_length_str, request_headers))
            {
                add_item("client_body", NULL, &params);
                client_body = get_item("client_body", params);
                content_length = atoi(content_length_str);
                client_body->value = malloc(content_length * sizeof(char));
            }
            else
                break;
        }

        double read_size = (content_length - recieved) / MAX_UPLOAD;
        int remainder = content_length % MAX_UPLOAD;

        finished = read_size < 1;
        current_recv = read(
            client_fd, &client_body->value[recieved], finished ? remainder : MAX_UPLOAD);
        recieved += current_recv;

        printf("   >> total bytes recieved: %dB %s<< \n", recieved, finished ? "(final) " : "");
    }

    if ((callback_status = action(request_headers, &response_body, params)) == -1)
        return -1;

    create_response(&response, response_headers, response_body, 200, "OK");
    if ((response_status = send_response(client_fd, response)) == -1)
        return -1;

    free_hashmap(request_headers);
    free_hashmap(params);
    free(response);
    free(response_body);

    return 0;
}

// send a response to the client socket file descriptor
int send_response(int client_fd, char *content)
{
    int length = strlen(content), bytes;

    printf("  >> sending response <<\n");
    if ((bytes = write(client_fd, content, length)) == -1)
        return -1;
    return 0;
}