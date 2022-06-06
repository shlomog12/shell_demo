#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "stdio.h"
#include "errno.h"
#include "stdlib.h"
#include "unistd.h"
#include <string.h>
#include <signal.h> 


int runPipe(char ***argvs, int len, int amend, int redirect, char* outfile){
  	int fildes[2];
	int fd = 0;
    int status = -1;
    for (size_t i = 0; i < len; i++){
        pipe(fildes);
		if (fork() == 0) {
			dup2(fd, 0);
            if (i == len-1){
                if (redirect == 1) {
                    fd = creat(outfile, 0660); 
                    close (STDOUT_FILENO) ; 
                    dup(fd); 
                    close(fd); 
                }else if (redirect == 2){
                    fd = creat(outfile, 0660); 
                    close (STDERR_FILENO) ; 
                    dup(fd); 
                    close(fd); 
                }
                close(fildes[0]);
			    status = execvp(argvs[i][0], argvs[i]);
            }else{
                dup2(fildes[1], 1);
                close(fildes[0]);
			    execvp(argvs[i][0], argvs[i]);
            }
			exit(1);
		}else {
            if (amend == 0){
                if (i == len-1) wait(&status);
                else  wait(NULL);
            }
			close(fildes[1]);
			fd = fildes[0];
		}
    }
    return status;
}

void signal_handler(int signal){
    printf("\nYou typed Control-C!\n");
}

void copyStr(char** oldStr, char** newStr,int len){
    for (size_t i = 0; i < len; i++){
        char* temp1 = (char*)malloc(strlen(oldStr[i])*sizeof(char));
        strcpy(temp1,oldStr[i]);
        newStr[i] = temp1;
    };
    newStr[len] = NULL;
}

void insertArgv(char** argvs[], char** argv, int len, int pos){
    argvs[pos] = (char**)malloc(len*10*sizeof(char));
    copyStr(argv, argvs[pos], len);
}

int initArgs(char ***argvs, int *lens){
    char command[1024];
    char *argv[10];

    fgets(command, 1024, stdin);
    command[strlen(command) - 1] = '\0';
    int piping = 0;
    int len = 0;
    char *token = strtok (command," ");
    while (token != NULL){
        argv[len] = token;
        token = strtok (NULL, " ");
        len++;
        if (token && !strcmp(token, "|")) {
            insertArgv(argvs, argv, len, piping);
            lens[piping] = len;
            piping++;
            len = 0;
            token = strtok (NULL, " ");
        }
    }
    argv[len] = NULL;
    lens[piping] = len;
    insertArgv(argvs,argv,len,piping);
    argvs[piping+1] = NULL;
    return piping;
}

void printArgvs(char ***argvs, int *lens, int piping){
    for (size_t i = 0; i <= piping; i++){
        int cur_len = lens[i];
        printf("************* i = %ld len = %d  ****************\n",i,cur_len);
        for (size_t j = 0; j < cur_len; j++){
            printf("j =%ld com = %s\n",j,argvs[i][j]);
        }
    }
}

void saveEnv(char **current_commends){
    int len = strlen(current_commends[0])+strlen(current_commends[1])+strlen(current_commends[2]);
    char* str =(char*) malloc(len*sizeof(char));
    strcat(str,current_commends[0]);
    strcat(str,current_commends[1]);
    strcat(str,current_commends[2]);
    putenv(str);
}

void printEnv(char *key){
    char val[30];
    if (!getenv(key)) val[0] = '\0';
    else snprintf(val, 10, "%s", getenv(key));
    printf("%s",val);
    if (val[strlen(val)-1] != '\n') printf("\n");
}

int main() {
    signal(SIGINT,  signal_handler);
    char *outfile;
    char *temp[10];
    char **argvs[20];
    int temp_len = -1;
    int len, amper, redirect, status, piping;
    int lens[20];
    int pos = 0;

    char* myprompt = "hello";
    // initArgs(argvs);
    while (1){
        printf("%s: ",myprompt);
        piping = initArgs(argvs,lens);
        // printArgvs(argvs, lens, piping);
        char** current_commends = argvs[0];
        len = lens[0];

        /* Is command empty */
        if (current_commends[0] == NULL) continue;

    // *****************start commants:*******************************
        if (!strcmp(current_commends[0], "!!")){
            if (temp_len == -1) continue;
            len = temp_len;
            copyStr(temp, current_commends, len);
        }else{
            copyStr(current_commends,temp,len);
            temp_len = len;
        }
        if (!strcmp(current_commends[0], "quit") || !strcmp(current_commends[0], "quit\n")){
            exit(0);
        }
        if ((len > 1)&&  !strcmp(current_commends[0], "cd")){
            chdir(current_commends[1]);
        }
        if ((len > 1 ) && (! strcmp(current_commends[0], "echo")) && (! strcmp(current_commends[1], "$?"))){
            sprintf(current_commends[1],"%d",status);
        }
        if ((len > 1 ) && (! strcmp(current_commends[0], "echo")) && current_commends[1][0]== '$'){
            printEnv(current_commends[1]+1); continue;
        }


        if ((len > 2) && (! strcmp(current_commends[0], "prompt")) && (! strcmp(current_commends[1], "="))){
            myprompt = current_commends[2];
            continue;
        }
        /* Does command line end with & */ 
        if (! strcmp(current_commends[len - 1], "&")) {
            amper = 1;
            current_commends[len - 1] = NULL;
        }else amper = 0; 


        if ((len > 2) && ! strcmp(current_commends[len - 2], ">")) {
            redirect = 1;
            current_commends[len - 2] = NULL;
            outfile = current_commends[len - 1];
            }
        else if ((len > 2) && !strcmp(current_commends[len - 2], "2>")){
            redirect = 2;
            current_commends[len - 2] = NULL;
            outfile = current_commends[len - 1];
        }else redirect = 0; 

        if (len > 1 && !strcmp(current_commends[0], "read")){
            char val_of_read[1024];
            fgets(val_of_read, 1024, stdin);
            char* read_input[3] = {current_commends[1],"=",val_of_read};
            saveEnv(read_input);
        }
        if (len > 2 && current_commends[0][0] == '$'){
            current_commends[0] = current_commends[0]+1;
            saveEnv(current_commends);
        }
      
        
        status = runPipe(argvs, piping+1,amper, redirect, outfile);

    }
}







