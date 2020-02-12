#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "command.h"
#include "redirect.h"
#include "jobs.h"

// looks for tokens such as <, >, | to execute yash command
void yashExec(char **parsedcmd, int numTokens){
    // look for pipe first
    for(int i = 0; i < numTokens; i++){
        if(strcmp(parsedcmd[i], "|") == 0){
            // get the left and right commands
            char **leftCommand = getSubCommand(parsedcmd, 0, i);
            char **rightCommand = getSubCommand(parsedcmd, i+1, numTokens);

            piping(leftCommand, rightCommand);
            exit(0);
        }
    }

    // look for redirection
    char **command;
    int numRedirect = 0;
    for(int i = 0; i < numTokens-1; i++){
        if(strcmp(parsedcmd[i], "<") == 0){
            // redirect in
            redirectIn(parsedcmd[i+1]);     // get file after the < token
            numRedirect++;
            if(numRedirect == 1){
                // get full command
                command = getSubCommand(parsedcmd, 0, i);
            }
        }
        if(strcmp(parsedcmd[i], ">") == 0){
            // redirect out
            redirectOut(parsedcmd[i+1]);
            numRedirect++;
            if(numRedirect == 1){
                // get full command
                command = getSubCommand(parsedcmd, 0, i);
            }
        }
        if(strcmp(parsedcmd[i], "2>") == 0){
            // redirect error  
            redirectErr(parsedcmd[i+1]);
            numRedirect++;
            if(numRedirect == 1){
                // get full command
                command = getSubCommand(parsedcmd, 0, i);
            }

        }
    }
    if(numRedirect > 0){
        execvp(command[0], command);
    } else{
        execvp(parsedcmd[0], parsedcmd);
    }
}


// check if fg, bg, or jobs are called and executes them
// otherwise, do nothing
// returns 1 if successful, 0 otherwise
int jobExec(char **command){
    if(strcmp(command[0], "jobs") == 0){
        printJobs();
        return 1;
    } else if(strcmp(command[0], "fg") == 0){
        // sends the process to the foreground
        fgExec();
        return 1;
        
    } else if(strcmp(command[0], "bg") == 0){
        bgExec();
        return 1;  
    }

    return 0;
}

