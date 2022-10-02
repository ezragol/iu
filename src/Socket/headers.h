#ifndef __SOCKET__HEADERS_H_
#define __SOCKET__HEADERS_H_

#define MAX_HEADERS 1024
#define MAX_HEADER_SIZE 128
#define HEADERS_END "\r\n\r\n"
#define NEWLINE '\n'
#define MAX_UPLOAD 4194304

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

int cut_headers(char *buf, char *headers, char **content, int buf_size);
int read_header(char *headers, char *value, char *key);

int response_header(char *key, char *value, char *headers);

#endif