#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "builtin.h"
#include "parse.h"

static char* builtin[] = {
    "exit",   /* exits the shell */
    "which",  /* displays full path to command */
    NULL
};


int is_builtin (char* cmd)
{
    int i;

    for (i=0; builtin[i]; i++) {
        if (!strcmp (cmd, builtin[i]))
            return 1;
    }

    return 0;
}


void builtin_execute (Task T)
{
    if (!strcmp (T.cmd, "exit")) {
        exit (EXIT_SUCCESS);
    }
    else if (!strcmp(T.cmd, "which")) {
        if(T.argv[1] != NULL){
            if (is_builtin(T.argv[1])){ //builtin command
                printf("%s: shell built-in command\n", T.argv[1]);
            }
            else { //If a fully qualified path or relative path is supplied to an executable program, then that path should simply be printed to stdout
                int checker = 0;
                if (strchr(T.argv[1], '/') != NULL) {
                    if (access(T.argv[1], X_OK) == 0){
                        printf("%s\n", T.argv[1]);
                        checker = 1;
                    }
                }
                else{
                    char fullPath[1024];
                    char *path = getenv("PATH");
                    if (path != NULL){
                        char *pathCopy = strdup(path);
                        char *pather = strtok(pathCopy, ":");
                        while (pather != NULL) {
                            snprintf(fullPath, sizeof(fullPath), "%s/%s", pather, T.argv[1]);
                            if (access(fullPath, X_OK) == 0) {
                                printf("%s\n", fullPath);
                                checker = 1;
                                break;
                            }
                            pather = strtok(NULL, ":");
                        }
                        free(pathCopy);
                    }
                }
                //print nothing if not found
                if (!checker){
                }
            }
        }
    }
    else {
        printf ("pssh: builtin command: %s (not implemented!)\n", T.cmd);
        }
}
