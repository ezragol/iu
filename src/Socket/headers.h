#ifndef __SOCKET__HEADERS_H_
#define __SOCKET__HEADERS_H_

#define MAX_HEADERS 4096
#define MAX_HEADER_SIZE 128
#define HEADERS_END "\r\n\r\n"
#define NEWLINE '\n'
#define MAX_UPLOAD 4194304
#define MAX_REQUEST_PATH 512

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>

int cut_headers(char *buf, char *headers, char **content, int buf_size);
int read_header(char *headers, char *value, char *key);
int read_request_path(char *headers, char *path);
int read_request_method(char *headers, char *method);

int response_header(char *key, char *value, char *headers);
int time_header(char *headers);

#endif