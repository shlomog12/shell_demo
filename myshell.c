#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "stdio.h"
#include "errno.h"
#include "stdlib.h"
#include "unistd.h"
#include <string.h>

int main() {
char command[1024];
char *token;
char *outfile;
int len, fd, amper, redirect, retid, status;
char *argv[10];
char* myprompt = "hello";

while (1){
    printf("%s: ",myprompt);
    fgets(command, 1024, stdin);
    command[strlen(command) - 1] = '\0';

    /* parse command line */
    len = 0;
    token = strtok (command," ");
    while (token != NULL){
        argv[len] = token;
        token = strtok (NULL, " ");
        len++;
    }
    argv[len] = NULL;

    /* Is command empty */
    if (argv[0] == NULL) continue;




// start commants:


    if ((len > 2) && (! strcmp(argv[0], "prompt")) && (! strcmp(argv[1], "="))){
        strcpy(myprompt,argv[2]);
        continue;
    }




    /* Does command line end with & */ 
    if (! strcmp(argv[len - 1], "&")) {
        amper = 1;
        argv[len - 1] = NULL;
    }else amper = 0; 


    if (! strcmp(argv[len - 2], ">")) {
        redirect = 1;
        argv[len - 2] = NULL;
        outfile = argv[len - 1];
        }
    else if (! strcmp(argv[len - 2], "2>")){
        redirect = 2;
        argv[len - 2] = NULL;
        outfile = argv[len - 1];
    }else
        redirect = 0; 

    /* for commands not part of the shell command language */ 

    if (fork() == 0) { 
        /* redirection of IO ? */
        if (redirect == 1) {
            fd = creat(outfile, 0660); 
            close (STDOUT_FILENO) ; 
            dup(fd); 
            close(fd); 
            /* stdout is now redirected */
        }else if (redirect == 2){
            fd = creat(outfile, 0660); 
            close (STDERR_FILENO) ; 
            dup(fd); 
            close(fd); 
        }
        execvp(argv[0], argv);
    }
    /* parent continues here */
    if (amper == 0)
        retid = wait(&status);
}
}
