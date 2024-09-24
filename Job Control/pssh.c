#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <readline/readline.h>
#include <sys/wait.h>
#include <fcntl.h>


#include "builtin.h"
#include "parse.h"

/*******************************************
 * Set to 1 to view the command line parse *
 *******************************************/
#define DEBUG_PARSE 0
#define READ_SIDE 0
#define WRITE_SIDE 1


void print_banner ()
{
    printf ("                    ________   \n");
    printf ("_________________________  /_  \n");
    printf ("___  __ \\_  ___/_  ___/_  __ \\ \n");
    printf ("__  /_/ /(__  )_(__  )_  / / / \n");
    printf ("_  .___//____/ /____/ /_/ /_/  \n");
    printf ("/_/ Type 'exit' or ctrl+c to quit\n\n");
}


/* **returns** a string used to build the prompt
 * (DO NOT JUST printf() IN HERE!)
 *
 * Note:
 *   If you modify this function to return a string on the heap,
 *   be sure to free() it later when appropirate!  */
static char* build_prompt ()
{
    char prompt[1024];
    if (getcwd(prompt, sizeof(prompt)) != NULL){
        fprintf(stdout, "%s", prompt);
    }
    else{
        fprintf(stderr, "cwd failed");
    }
    return  "$ ";
}

/* return true if command is found, either:
 *   - a valid fully qualified path was supplied to an existing file
 *   - the executable file was found in the system's PATH
 * false is returned otherwise */
static int command_found (const char* cmd)
{
    char* dir;
    char* tmp;
    char* PATH;
    char* state;
    char probe[PATH_MAX];

    int ret = 0;

    if (access (cmd, X_OK) == 0)
        return 1;

    PATH = strdup (getenv("PATH"));

    for (tmp=PATH; ; tmp=NULL) {
        dir = strtok_r (tmp, ":", &state);
        if (!dir)
            break;

        strncpy (probe, dir, PATH_MAX-1);
        strncat (probe, "/", PATH_MAX-1);
        strncat (probe, cmd, PATH_MAX-1);

        if (access (probe, X_OK) == 0) {
            ret = 1;
            break;
        }
    }

    free (PATH);
    return ret;
}

void file_redirect(char *file, int redirect_side) {
    int fd;
    if (redirect_side == READ_SIDE) {
        fd = open(file, O_RDONLY);
        if (fd < 0) {
            perror("Failed to open input file");
            exit(EXIT_FAILURE);
        }
        if (dup2(fd, STDIN_FILENO) == -1) {
            perror("Failed to redirect input");
            exit(EXIT_FAILURE);
        }
    } else if (redirect_side == WRITE_SIDE) {
        fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd < 0) {
            perror("Failed to open output file");
            exit(EXIT_FAILURE);
        }
        if (dup2(fd, STDOUT_FILENO) == -1) {
            perror("Failed to redirect output");
            exit(EXIT_FAILURE);
        }
    }
    close(fd);
}

/* Called upon receiving a successful parse.
 * This function is responsible for cycling through the
 * tasks, and forking, executing, etc as necessary to get
 * the job done! */
void execute_tasks (Parse* P)
{
    unsigned int t;
    int fd[2];
    int prev = 0;
    pid_t* pids = malloc(P->ntasks*sizeof(pid_t));


    for (t = 0; t < P->ntasks; t++) {
        if (t < (P->ntasks-1)) {
            if (pipe(fd) == -1) {
                fprintf(stderr, "pipe failed\n");
                exit(EXIT_FAILURE);
            }
        }

        if (is_builtin (P->tasks[t].cmd)) { 
            
            if (P->infile) { //redirect input
                file_redirect(P->infile, READ_SIDE);
            }
            if (P->outfile) { //redirect output
                file_redirect(P->outfile, WRITE_SIDE);
            }

            builtin_execute(P->tasks[t]);

            //after executing a built-in, reset the standard input/output
            if (P->infile) {
                dup2(STDIN_FILENO, fileno(stdin));
            }
            if (P->outfile) {
                dup2(STDOUT_FILENO, fileno(stdout));
            }
        }
        else if (command_found(P->tasks[t].cmd)){
            pid_t pid = fork();
            if (pid < 0){ //failed
                fprintf(stderr, "fork failed");
                exit(EXIT_FAILURE);
            }
            
            else if (pid > 0){ //parent
                pids[t] = pid;
                if (t < (P->ntasks -1)){
                    close(fd[WRITE_SIDE]); //close write end
                    if (prev != 0) {
                        close(prev); 
                    }
                    prev = fd[READ_SIDE]; //store read end
                }
                else {
                    close(fd[READ_SIDE]);

                }
            }
            
            else { //child
                if (prev != 0){
                    dup2(prev, STDIN_FILENO);
                    close(prev);
                }
                if (t == 0 && P->infile) { //first command
                    file_redirect(P->infile, READ_SIDE);
                }
                else if (t < P->ntasks - 1) { //up to last command
                    if (dup2(fd[WRITE_SIDE], STDOUT_FILENO) == -1) {
                        perror("dup2 failed for fd[1] to STDOUT");
                        exit(EXIT_FAILURE);
                    }
                    close(fd[WRITE_SIDE]); // close write end of the current pipe
                }
                else if (P->outfile && t == P->ntasks - 1) {  //last command
                    file_redirect(P->outfile, WRITE_SIDE);
                }
                execvp(P->tasks[t].cmd, P->tasks[t].argv);
                fprintf(stderr, "pssh: found but can't exec: %s\n", P->tasks[t].cmd);
                exit(EXIT_FAILURE);
            }
        }
    }
    for (unsigned int i = 0; i < P->ntasks; i++) {
        int status;
        waitpid(pids[i], &status, 0);
    }

    free(pids);
}


int main (int argc, char** argv)
{
    char* cmdline;
    Parse* P;

    print_banner ();

    while (1) {
        cmdline = readline (build_prompt());
        if (!cmdline)       /* EOF (ex: ctrl-d) */
            exit (EXIT_SUCCESS);

        P = parse_cmdline (cmdline);
        if (!P)
            goto next;

        if (P->invalid_syntax) {
            printf ("pssh: invalid syntax\n");
            goto next;
        }

#if DEBUG_PARSE
        parse_debug (P);
#endif

        execute_tasks (P);

    next:
        parse_destroy (&P);
        free(cmdline);
    }
}
