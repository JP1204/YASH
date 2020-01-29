#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "command.h"
#include "redirect.h"


int main(int argc, char *argv[]){
    int cpid;
    char *command;
    char **parsedcmd;

    printf("cmd:");
    while(command = getCommand()){
        if(strcmp(command, "exit") == 0){
          printf("exiting\n");
            free(command);
            exit(0);
        }

        parsedcmd = parseString(command);
        if(parsedcmd == NULL){
            // echo the command and prompt for new one
            printf("Invalid command\n%s\n", command);

            free(command);
            printf("cmd:");
            continue;
        } 

        cpid = fork();
        if(cpid == 0){
            // execute command
            if(strcmp(parsedcmd[1], ">") == 0){
                // redirect into file
                int ofd = creat(parsedcmd[2], 0644);
                dup2(ofd, STDOUT_FILENO);
                execlp(parsedcmd[0], parsedcmd[0], (char *) NULL);
            }

            execvp(parsedcmd[0], parsedcmd);
        } else{
            // frees memory to prompt user for new command
            wait((int *)NULL);
            freeAll(command, parsedcmd);
            printf("cmd:");
        }
    }
}

