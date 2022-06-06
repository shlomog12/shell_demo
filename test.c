#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "stdio.h"
#include "errno.h"
#include "stdlib.h"
#include "unistd.h"
#include <string.h>
#include <signal.h> 

int cnt = 0;

void signal_handler(int signal){
//   printf("Received SIGINT from CTRL-C but not quitting because it is overridden.\n");
    printf("You typed Control-C!\n");
    cnt++;
}


int main(){

    // signal(SIGINT,  signal_handler);
    while(1) {
        if (cnt > 5){
            return 0;
        }
    }
    // signal(SIGINT,  signal_handler);

    // signal(SIGINT, SIG_DFL);

}