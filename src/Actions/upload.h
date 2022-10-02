#ifndef __ACTIONS__UPLOAD_H_
#define __ACTIONS__UPLOAD_H_

#include "../Socket/connection.h"
#include "../Socket/headers.h"

int upload_file(char *location, int client_fd, char *client_buf);

#endif