#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "command.h"


int main(int argc, char *argv[]){
    int cpid;
    char *command;
    char **parsedcmd;

    while(command = (char *) readline("cmd:")){
        parsedcmd = parseString(command);
        cpid = fork();

        if(cpid = 0){
            execvp(parsedcmd[0], parsedcmd);
        } else{
            wait((int *)NULL);
        }
    }
}

