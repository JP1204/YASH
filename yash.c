#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "command.h"
#include "redirect.h"
#include "exec.h"


int main(int argc, char *argv[]){
    int cpid;
    char *command;
    char **parsedcmd;

    printf("cmd:");
    while(command = getCommand()){
        // look for exit message
        if(strcmp(command, "exit") == 0){
          printf("exiting session\n");
            free(command);
            exit(0);
        }

        parsedcmd = parseString(command);
        // check for invalid command
        if(parsedcmd == NULL){
            // echo the command and prompt for new one
            printf("Invalid command\n%s\n", command);

            free(command);
            printf("cmd:");
            continue;
        } 

        int numTokens = findNumTokens(parsedcmd);

        // look through tokens and let yash execute if it's a valid command
        yashExec(parsedcmd, numTokens);

/*
        cpid = fork();
        if(cpid == 0){
            // find the number of tokens
          printf("inside child exec\n");
            int numTokens = findNumTokens(parsedcmd);

          printf("there are %d tokens\n", numTokens);

            yashExec(parsedcmd, numTokens);

            execvp(parsedcmd[0], parsedcmd);
          printf("command did not work\n");
            exit(0);
        } else{
            // frees memory to prompt user for new command
            wait((int *)NULL);
            freeAll(command, parsedcmd);
            printf("cmd:");
        }*/
    }
}

