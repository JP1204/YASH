#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "command.h"


int main(int argc, char *argv[]){
    int cpid;
    char *command;
    char **parsedcmd;

    printf("cmd:");
    while(command = getCommand()){
        if(strcmp(command, "exit") == 0){
            free(command);
            exit(0);
        }

        parsedcmd = parseString(command);
        if(parsedcmd == NULL){
            // echo the command and prompt for new one
            printf("%s\n", command);
            free(command);
            printf("cmd:");
            continue;
        }    

     printf("command is %s\n", command);
        cpid = fork();

        if(cpid == 0){
            // execute command
          //  execvp(parsedcmd[0], parsedcmd);
          break;
        } else{
            // free memory to prompt user for new command
            wait((int *)NULL);
            freeAll(command, parsedcmd);
            printf("cmd:");
        }
    }
}

