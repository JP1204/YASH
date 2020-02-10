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
          
         //   printTokens(leftCommand);
         //   printTokens(rightCommand);

            piping(leftCommand, rightCommand);
            exit(0);
        }
    }

    // look for redirection
    int numRedirect = 0;
    for(int i = 0; i < numTokens-1; i++){
        if(strcmp(parsedcmd[i], "<") == 0){
            // redirect in
            char **leftCommand = getSubCommand(parsedcmd, 0, i);
            redirectIn(parsedcmd[i+1]);     // get file after the < token
            numRedirect++;
        }
        if(strcmp(parsedcmd[i], ">") == 0){
            // redirect out
            char **leftCommand = getSubCommand(parsedcmd, 0, i);
            redirectOut(parsedcmd[i+1]);
            numRedirect++;
        }
        if(strcmp(parsedcmd[i], "2>") == 0){
            // redirect error  
            char **leftCommand = getSubCommand(parsedcmd, 0, i);
            redirectErr(parsedcmd[i+1]);
            numRedirect++;
        }
    }

    // check if redirection is found
    // if so execute the command
    if(numRedirect > 0){
        execlp(parsedcmd[0], parsedcmd[0], (char *) NULL);
    } else {
        // if not, execute bash version
        printf("executing bash version\n");
        execvp(parsedcmd[0], parsedcmd);
      printf("command failed\n");
    }
}


// check if fg, bg, or jobs are called and executes them
// otherwise, do nothing
void jobExec(char **command, jobList *allJobs){
    if(findNumTokens(command) > 1) return;

    if(strcmp(command[0], "jobs") == 0){
      //printf("jobs executing now\n");
        printJobs(allJobs);
        exit(0);
    } else if(strcmp(command[0], "fg") == 0){
        
    } else if(strcmp(command[0], "bg") == 0){
       
    }
}

