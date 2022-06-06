#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "stdio.h"
#include "errno.h"
#include "stdlib.h"
#include "unistd.h"
#include <string.h>
#include <signal.h> 


// void foo(int fd, char **argvs[]){
//     printf("***********************\n");
//     printf("arv1[0] = %s\n",argvs[0][0]);
//     printf("arv2[0] = %s\n",argvs[1][0]);
//     printf("arv3[0] = %s\n",argvs[2][0]);
//     printf("***********************\n");
//     int fildes[2];
//     pipe (fildes);
//     if (fork() == 0) { 
//         /* first component of command line */ 
//         close(STDOUT_FILENO); 
//         dup(fildes[0]); 
//         close(fildes[0]); 
//         close(fildes[1]);
//         close(fildes[2]); 
//             /* stdout now goes to pipe */ 
//             /* child process does command */ 
//         execvp(argvs[0][0], argvs[0]);
//     } 
//         /* 2nd command component of command line */ 
//     if (fork() == 0){
//         close(STDIN_FILENO);
//         dup(fildes[1]);
//         close(fildes[1]); 
//         close(fildes[0]);
//         close(fildes[2]); 
//         execvp(argvs[1][0], argvs[1]);

//     }
//     close(STDIN_FILENO);
//     dup(fildes[2]);
//     close(fildes[2]); 
//     close(fildes[1]);
//     close(fildes[0]); 
//         /* standard input now comes from pipe */ 
//     execvp(argvs[2][0], argvs[2]);
// }



static void pipeline(char ***cmd){
	int fd[2];
	pid_t pid;
	int fdd = 0;				/* Backup */

	while (*cmd != NULL) {
		pipe(fd);				/* Sharing bidiflow */
		if ((pid = fork()) == -1) {
			perror("fork");
			exit(1);
		}
		else if (pid == 0) {
			dup2(fdd, 0);
			if (*(cmd + 1) != NULL) {
				dup2(fd[1], 1);
			}
			close(fd[0]);
			execvp((*cmd)[0], *cmd);
			exit(1);
		}
		else {
			wait(NULL); 		/* Collect childs */
			close(fd[1]);
			fdd = fd[0];
			cmd++;
		}
	}
}


// void foo4(){


// }

int runPipe(char ***cmd, int len, int amper){
  	int fildes[2];
	int fdd = 0;
    int status = -1;
    for (size_t i = 0; i < len; i++){
        pipe(fildes);
		if (fork() == 0) {
			dup2(fdd, 0);
            if (i == len-1){
                close(fildes[0]);
			    status = execvp(cmd[i][0], cmd[i]);
            }else{
                dup2(fildes[1], 1);
                close(fildes[0]);
			    execvp(cmd[i][0], cmd[i]);
            }
			exit(1);
		}else {
            if (i == len-1 && amper == 0) wait(&status);
            else wait(NULL); 
			close(fildes[1]);
			fdd = fildes[0];
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


int main() {
    signal(SIGINT,  signal_handler);

    char command[1024];
    char *token;
    char *outfile;
    char *argv[10], *temp[10];
    char **argvs[20];
    int len, fd, amper, redirect, retid, status, temp_len, piping;
    int lens[20];
    int pos = 0;

    char* myprompt = "hello";
    int x = 0;


    while (1){
        x++;
        printf("%s: ",myprompt);
        fgets(command, 1024, stdin);
        command[strlen(command) - 1] = '\0';

        piping = 0;
        /* parse command line */
      
        
        len = 0;
        token = strtok (command," ");
        while (token != NULL){
            argv[len] = token;
            token = strtok (NULL, " ");
            // printf("\nerror 70\n");
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

        // copyStr(argv,argvs[0],lens[0]);

        // for (size_t i = 0; i <= piping; i++){
        //     int cur_len = lens[i];
        //     printf("************* i = %ld len = %d  ****************\n",i,cur_len);
        //     for (size_t j = 0; j < cur_len; j++){
        //         printf("j =%ld com = %s\n",j,argvs[i][j]);
        //     }
        // }


        char** current_commends = argvs[0];
        len = lens[0];
        // printf("cur[0] = %s\n",current_commends[0]);
        

        /* Is command empty */
        if (current_commends[0] == NULL) continue;
        if (! strcmp(current_commends[0], "!!")){
            len = temp_len;
            copyStr(temp, current_commends, len);
        }else{
            copyStr(current_commends,temp,len);
            temp_len = len;
        }
        if (!strcmp(current_commends[0], "quit")){
            // return 0;
            exit(0);
        }
    // *****************start commants:*******************************

        if ((len > 1)&&  !strcmp(current_commends[0], "cd")){
            chdir(current_commends[1]);
        }

        if ((len > 1 ) && (! strcmp(current_commends[0], "echo")) && (! strcmp(current_commends[1], "$?"))){
            sprintf(current_commends[1],"%d",status);
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
        else if ((len > 2) && ! strcmp(current_commends[len - 2], "2>")){
            redirect = 2;
            current_commends[len - 2] = NULL;
            outfile = current_commends[len - 1];
        }else
            redirect = 0; 

        /* for commands not part of the shell command language */ 

        int fildes[2];
        // foo(redirect, piping, fd, outfile, fildes, argvs);


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
            status = runPipe(argvs, piping+1, amper);
            printf("\nstt77 = %d\n",status);
        }
        
        /* parent continues here */
        // if (amper == 0) retid = wait(&status);
    }
}







