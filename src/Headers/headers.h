#ifndef __HEADERS__HEADERS_H_
#define __HEADERS__HEADERS_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <stdbool.h>

#include "./hashmap.h"

#define MAX_HEADER_SIZE 128
#define LARGE_HEADER_SIZE 1024
#define MAX_HEADER_COUNT 64

#define HEADERS_END "\r\n\r\n"
#define NEWLINE '\n'
#define DELIMITER ':'

#define MAX_UPLOAD 65536
#define MAX_BODY 524288
#define MAX_REQUEST_PATH 512

int parse_request_headers(char *buf, hashmap *headers, int buf_size);
int destruct_response_headers(hashmap *response_headers, char *response_body);

int add_header(char *key, char *value, char *headers);
int time_header(hashmap *headers);

#endif