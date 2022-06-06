#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "stdio.h"
#include "errno.h"
#include "stdlib.h"
#include "unistd.h"
#include <string.h>
#include <signal.h> 


void copyStr(char** oldStr, char** newStr,int len){
    for (size_t i = 0; i < len; i++){
        char* temp1 = (char*)malloc(strlen(oldStr[i])*sizeof(char));
        strcpy(temp1,oldStr[i]);
        newStr[i] = temp1;
    };
    newStr[len] = NULL;
}

void signal_handler(int signal){
    printf("\nYou typed Control-C!\n");
    
}

int main() {
    signal(SIGINT,  signal_handler);

    char command[1024];
    char *token;
    char *outfile;
    int len, fd, amper, redirect, retid, status;
    char *argv[10];
    char *temp[10];
    int temp_len;

    char* myprompt = "hello";
    int x = 0;


    while (1){
        x++;

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
        
        if (! strcmp(argv[0], "!!")){
            len = temp_len;
            copyStr(temp, argv, len);
        }else{
            copyStr(argv,temp,len);
            temp_len = len;
        }
        if (!strcmp(argv[0], "quit")){
            return 0;
        }
    // *****************start commants:*******************************

        if ((len > 1)&&  !strcmp(argv[0], "cd")){
            chdir(argv[1]);
        }

        if ((len > 1 ) && (! strcmp(argv[0], "echo")) && (! strcmp(argv[1], "$?"))){
            sprintf(argv[1],"%d",status);
        }
        if ((len > 2) && (! strcmp(argv[0], "prompt")) && (! strcmp(argv[1], "="))){
            myprompt = argv[2];
            continue;
        }


        

        /* Does command line end with & */ 
        if (! strcmp(argv[len - 1], "&")) {
            amper = 1;
            argv[len - 1] = NULL;
        }else amper = 0; 


        if ((len > 2) && ! strcmp(argv[len - 2], ">")) {
            redirect = 1;
            argv[len - 2] = NULL;
            outfile = argv[len - 1];
            }
        else if ((len > 2) && ! strcmp(argv[len - 2], "2>")){
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
            status = execvp(argv[0], argv);
        }
        /* parent continues here */
        if (amper == 0)
            retid = wait(&status);
    }
}
