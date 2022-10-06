#ifndef __SOCKET__HEADERS_H_
#define __SOCKET__HEADERS_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <stdbool.h>

#define MAX_HEADERS 65536
#define MAX_HEADER_SIZE 128
#define LARGE_HEADER_SIZE 1024
#define MAX_HEADER_COUNT MAX_HEADERS / MAX_HEADER_SIZE

#define HEADERS_END "\r\n\r\n"
#define NEWLINE '\n'
#define DELIMITER ':'

#define MAX_UPLOAD 4194304
#define MAX_REQUEST_PATH 512

typedef struct s_header {
    char *key;
    char *value;
} header;

typedef struct s_headers {
    header *content;
    int size;
} headerarray;

int cut_headers(char *buf, headerarray *headers, char **content, int buf_size);
int read_header(headerarray *headers, char *value, char *key);
int free_headers(headerarray headers);

int response_header(char *key, char *value, char *headers);
int time_header(char *headers);

#endif