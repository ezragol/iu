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

#include "headers.h"

#define BACKLOG 2
#define MAX_BODY 4096

void sigchld_handler(int sig);
void *read_client_ip(struct sockaddr *sock_addr);

int translate_port(char *port_str, int port);
int detail_socket(struct addrinfo *details, int size, int ai_family, int ai_socktype, int ai_flags);

int create_response(char *response, char *headers, char *content);

int create_socket(int port_int, struct addrinfo *details);
int establish_connection(int sockfd, struct sockaddr_storage client_addr, int size, char *client_ip);
int send_response(int client_fd, char *content);

#endif