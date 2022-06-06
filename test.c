/*
** pipex.c - multipipes support
*/

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>

/*
 * loop over commands by sharing
 * pipes.
 */
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

/*
 * Compute multi-pipeline based
 * on a command list.
 */


void setEnv(char *key, char* val){
	if(!getenv(key)){
        fprintf(stderr, "The environment variable %s was not found.\n", key);
        exit(1);
    }
	if(snprintf(val, 10, "%s", getenv(key)) >= 10){
        fprintf(stderr, "BUFSIZE of %d was too small. Aborting\n",10);
        exit(1);
    }
}



int main(int argc, char *argv[]){
	putenv("person=moshe");
	// char aa[10];
	// char *envvar = "person";
	// //  if(!getenv(envvar)){
    // //     fprintf(stderr, "The environment variable %s was not found.\n", envvar);
    // //     exit(1);
    // // }
	// if(!getenv(envvar)){
    //     fprintf(stderr, "The environment variable %s was not found.\n", envvar);
    //     exit(1);
    // }
	// if(snprintf(aa, 10, "%s", getenv(envvar)) >= 10){
    //     fprintf(stderr, "BUFSIZE of %d was too small. Aborting\n",10);
    //     exit(1);
    // }
	char val[20];
	setEnv("person",val);
    printf("person: %s\n",val) ;
	// snprintf(aa, 10, "%s", getenv("person"));
	// printf("aa =%s\n",aa);
	// char *ls[] = {"ls", "-al", NULL};
	// char *rev[] = {"rev", NULL};
	// char *nl[] = {"nl", NULL};
	// char *cat[] = {"cat", "-e", NULL};
    // char *cat[] = {"cat", "file1.txt", NULL};
	// char *sort[] = {"sort", NULL};
	// char *uniq[] = {"uniq", NULL};


	// // char *cat[] = {"cat", "-e", NULL};
	// // char **cmd[] = {ls, rev, nl, cat, NULL};
    // char **cmd[] = {cat, sort, uniq, NULL};
	// pipeline(cmd);
	return (0);
}