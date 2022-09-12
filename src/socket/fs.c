#include "fs.h"

int resolve_path(char *path, char *full_path)
{
    int addition_len = strlen(path);
    if (path[0] == '/' && path[1] != '.')
        strncpy(full_path, path, addition_len);
    else
    {
        char *cwd = getcwd(NULL, 0);
        int cwd_len = strlen(cwd);
        snprintf(full_path, MAX_PATH_LEN, "%s/%s", cwd, path);
        free(cwd);
    }
    return 0;
}