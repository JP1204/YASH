#include <stdio.h>
#include <string.h>
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
           
          printf("%s\n", leftCommand[0]);
            printTokens(leftCommand);
            printTokens(rightCommand);
        }
    }

    // look for redirection
    for(int i = 0; i < numTokens; i++){
        if(strcmp(parsedcmd[i], "<") == 0){
            // redirect in

        }
        if(strcmp(parsedcmd[i], ">") == 0){
            char **leftCommand = getSubCommand(parsedcmd, 0, i);
            redirectOut(leftCommand, parsedcmd[i+1]);
        }
    }
}
