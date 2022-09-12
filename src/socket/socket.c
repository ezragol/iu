#include "socket.h"

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

// strip headers from the given request buffer
int cut_headers(char *buf, char **headers, char **content, int buf_size)
{
    char next_set[4] = "    ";
    int index = 0;
    if (buf_size < 4)
    {
        return 1;
    }

    while (strcmp(next_set, HEADERS_END) != 0 && index < MAX_HEADERS)
    {
        memcpy(next_set, buf + index, 4);
        index++;
    }

    if (index != 0 && index < MAX_HEADERS)
    {
        *headers = calloc(index + 1, sizeof(char));
        memcpy(*headers, buf, index);
    }
    else
    {
        *content = calloc(buf_size + 1, sizeof(char));
        memcpy(*content, buf, buf_size);
    }
    return 0;
}

// read the header with the given name (key)
// messy
int read_header(char *headers, char **value, char *key)
{
    char delimiter = ':', newline = '\n';
    int index = 0, length = strlen(headers), keylen = strlen(key);
    int *nl_locations = calloc(1, sizeof(int)), last, nl_length = 0;

    for (int i = 0; i < length; i++)
    {
        if (headers[i] == newline)
        {
            int temp[nl_length];
            nl_length++;
            if (nl_length > 1)
            {
                memcpy(temp, nl_locations, nl_length);
                free(nl_locations);
                nl_locations = calloc(nl_length, sizeof(int));
                memcpy(nl_locations, temp, nl_length);
            }
            nl_locations[nl_length - 1] = i + 1;
        }
    }

    last = nl_locations[0];

    for (int i = 1; i < nl_length; i++)
    {
        for (int j = last; j < nl_locations[i]; j++)
        {
            for (int t = 0; t < keylen; t++)
            {
                if (headers[j + t] != key[t])
                    break;
                if (t == keylen - 1)
                {
                    int valuelen = nl_locations[i] - j - t;
                    *value = calloc(valuelen, sizeof(char));
                    memcpy(*value, headers + j + t + 3, valuelen - 3);
                    free(nl_locations);
                    return 0;
                }
            }
            last = nl_locations[i];
        }
    }

    free(nl_locations);
    return 1;
}

// attach headers to response
int create_response(char *response, char *body)
{
    int length = strlen(body);
    snprintf(response, MAX_HEADERS + MAX_BODY, "%s%d%s\r\n\r\n%s",
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

    printf("\n> listening for connections on localhost:%d\n", port);
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

// remove the headers from the request buffer and put the remaining content into the given file location
// little messy
int upload_file(char *location, int client_fd, char *client_buf)
{
    FILE *fr = fopen(location, "w+");
    char *headers = NULL, *image_binary = NULL, *content_length_str;
    int client_size = 0, content_length = MAX_UPLOAD, recieve_size, status = 0;
    while (client_size < content_length && (recieve_size = read(client_fd, client_buf, sizeof(char) * MAX_UPLOAD)) > 0)
    {
        printf("> server recieved %d bytes\n", recieve_size);
        if (cut_headers(client_buf, &headers, &image_binary, recieve_size) == 1)
        {
            fprintf(stderr, "> error while separating file content from HTTP Request");
            return 1;
        }
        if (image_binary != NULL)
        {
            fwrite(image_binary, sizeof(char), recieve_size, fr);
            if (ferror(fr))
            {
                perror("> while copying uploaded file");
                return 1;
            }
        }

        if (image_binary != NULL)
        {
            free(image_binary);
            client_size += recieve_size;
        }
        else if (headers != NULL)
        {
            read_header(headers, &content_length_str, "Content-Length");
            content_length = atoi(content_length_str);
            free(headers);
            free(content_length_str);
        }
    }

    printf("> uploaded file to %s\n", location);
    fclose(fr);
    return 0;
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