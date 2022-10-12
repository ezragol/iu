#ifndef __ACTIONS__UPLOAD_H_
#define __ACTIONS__UPLOAD_H_

#include "../Headers/hashmap.h"

char *upload_file(hashmap *request_headers, hashmap *response_headers, hashmap *params);

#endif