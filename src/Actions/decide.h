#ifndef __ACTIONS__DECIDE_H_
#define __ACTIONS__DECIDE_H_

#include "./upload.h"

int match_request(char *method, char *path);
char *decide_outcome(hashmap *request_headers, hashmap *response_headers, hashmap *params);

#endif