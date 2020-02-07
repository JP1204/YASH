#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "command.h"
#include "redirect.h"

// looks for tokens such as <, >, | to execute yash command
void yashExec(char **parsedcmd, int numTokens){
    
    // look for pipe first
    for(int i = 0; i < numTokens; i++){
        if(strcmp(parsedcmd[i], "|") == 0){
            // get the left and right commands
            char **leftCommand = getSubCommand(parsedcmd, 0, i);
            char **rightCommand = getSubCommand(parsedcmd, i+1, numTokens);
          
            printTokens(leftCommand);
            printTokens(rightCommand);

            piping(leftCommand, rightCommand);
            exit(0);
        }
    }

    // look for redirection
    for(int i = 0; i < numTokens; i++){
        if(strcmp(parsedcmd[i], "<") == 0){
            // redirect in
            char **leftCommand = getSubCommand(parsedcmd, 0, i);
            redirectIn(leftCommand, parsedcmd[i+1]);
            exit(0);
        }
        if(strcmp(parsedcmd[i], ">") == 0){
            // redirect out
            char **leftCommand = getSubCommand(parsedcmd, 0, i);
            redirectOut(leftCommand, parsedcmd[i+1]);
            exit(0);
        }
        if(strcmp(parsedcmd[i], "2>") == 0){
            // redirect error  
            char **leftCommand = getSubCommand(parsedcmd, 0, i);
            redirectErr(leftCommand, parsedcmd[i+1]);
            exit(0);
        }
    }

    // if no piping or redirection is found, execute bash version
    execvp(parsedcmd[0], parsedcmd);
}
