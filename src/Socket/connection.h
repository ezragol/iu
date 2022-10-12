#ifndef __SOCKET__CONNECTION_H_
#define __SOCKET__CONNECTION_H_

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
#include <math.h>

#include "headers.h"
#include "../Actions/decide.h"

#define BACKLOG 2

typedef struct s_requestinfo {
    char path[MAX_REQUEST_PATH];
    char method[6];
} requestinfo;

void sigchld_handler(int sig);
void *read_client_ip(struct sockaddr *sock_addr);

int detail_socket(struct addrinfo *details, int size, int ai_family, int ai_socktype, int ai_flags);

int process_connection(int client_fd, hashmap params);
int create_response(char **response, char *headers, char *body, int status, char *message);
int send_response(int client_fd, char *content);

int create_socket(char port[5], struct addrinfo *details);
int establish_connection(int sockfd, struct sockaddr_storage client_addr, int size, char *client_ip);

#endif