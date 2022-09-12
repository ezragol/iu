#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BACKLOG 2
#define MAX_UPLOAD 4194304
#define MAX_HEADERS 1024
#define HEADERS_END "\r\n\r\n"

void sigchld_handler(int sig);
void *read_client_ip(struct sockaddr *sock_addr);

int translate_port(char *port_str, int port);
int detail_socket(struct addrinfo *details, int size, int ai_family, int ai_socktype, int ai_flags);

int cut_headers(char *buf, char **headers, char **content, int buf_size);
int read_header(char *headers, char **value, char *key);

int create_socket(int port_int, struct addrinfo *details);
int establish_connection(int sockfd, struct sockaddr_storage client_addr, int size, char *client_ip);

int upload_file(char *location, int client_fd, char *client_buf);