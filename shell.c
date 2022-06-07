#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "stdio.h"
#include "errno.h"
#include "stdlib.h"
#include "unistd.h"
#include <string.h>
#include <signal.h> 


int runPipe(char*** argvs, int len, int amend, int redirect, char* outfile){
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

void printArgvs(char*** argvs, int *lens, int piping){
    for (size_t i = 0; i <= piping; i++){
        int cur_len = lens[i];
        printf("************* i = %ld len = %d  ****************\n",i,cur_len);
        for (size_t j = 0; j < cur_len; j++){
            printf("j =%ld com = %s\n",j,argvs[i][j]);
        }
    }
}

void saveEnv(char* key, char* value){
    int len = strlen(key)+strlen(value)+1;
    char* str =(char*) malloc(len*sizeof(char));
    strcat(str,key);
    strcat(str,"=");
    strcat(str,value);
    putenv(str);
}

char* getEnvByKey(char* key){
    // printf("k = %s\n",key);
    char* val = (char*) malloc(30);
    if (!getenv(key)){
        val[0] = '\0';
    } 
    else {
        snprintf(val, 30, "%s", getenv(key));
        // printf("val = %s\n",val);
    }
    return val;
}

void printEnv(char* key){
    char* val = getEnvByKey(key);
    printf("%s",val);
    if (val[strlen(val)-1] != '\n') printf("\n");
}

void extractEnv(char** commends ,int len){
    for (size_t i = 0; i < len; i++){
        if (commends[i][0] == '$'){
            commends[i] = getEnvByKey(commends[i]+1);
        } 
    }
}

void spacialInput(char** current_commends,int len , char** myprompt){
        char* first = current_commends[0];
        extractEnv(current_commends+1 ,len-1);
        if (!strcmp(first, "quit") || !strcmp(first, "quit\n")) exit(0);
        else if (len <= 1) return;
        else if (!strcmp(first, "cd")) chdir(current_commends[1]);
        else if (!strcmp(first, "read")){
            char val_of_read[1024];
            fgets(val_of_read, 1024, stdin);
            saveEnv(current_commends[1],val_of_read);
        }
        else if (len == 2) return;
        else if (!strcmp(first, "prompt") && !strcmp(current_commends[1], "=")) *myprompt = current_commends[2];
        else if (first[0] == '$' && !strcmp(current_commends[1], "=")){
            current_commends[0] = current_commends[0]+1;
            saveEnv(current_commends[0],current_commends[2]);
        }
}

void saveStatus(int status){
    char string_of_staus[10];
    sprintf(string_of_staus,"%d",status);
    saveEnv("?",string_of_staus);
}

int getAmper(char** current_commends, int len){
    if (strcmp(current_commends[len - 1], "&")) return 0;
    current_commends[len - 1] = NULL;
    return 1;
}

int getRedirect(char** current_commends,int len){
    if ((len > 2) && !strcmp(current_commends[len - 2], ">")) return 1;
    if ((len > 2) && !strcmp(current_commends[len - 2], "2>")) return 2;
    return 0; 
}

int main() {
    signal(SIGINT,  signal_handler);
    char* outfile;
    char* temp[10];
    char** argvs[20];
    int temp_len = -1;
    int len, amper, redirect, status, piping;
    int lens[20];

    char* myprompt = "hello";

    while (1){
        printf("%s: ",myprompt);
        piping = initArgs(argvs,lens);
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
        spacialInput(current_commends, len, &myprompt);
        int amper = getAmper(current_commends,len);
        int redirect = getRedirect(current_commends,len);
        if (redirect > 0){
            current_commends[len - 2] = NULL;
            outfile = current_commends[len - 1];
        }

        status = runPipe(argvs, piping+1,amper, redirect, outfile);
        saveStatus(status);
    }
}






