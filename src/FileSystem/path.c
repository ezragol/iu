#include "path.h"

int resolve_path(char *path, char *full_path)
{
    if (path[0] == '/' && path[1] != '.')
        strcpy(full_path, path);
    else
    {
        char *cwd = getcwd(NULL, 0);
        int cwd_len = strlen(cwd);
        snprintf(full_path, MAX_PATH_LEN, "%s/%s", cwd, path);
        free(cwd);
    }
    return 0;
}