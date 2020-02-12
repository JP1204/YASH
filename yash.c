#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdbool.h>
#include "command.h"
#include "redirect.h"
#include "exec.h"
#include "signal.h"


// global list of all jobs
Job *jobListHead = NULL;

int main(int argc, char *argv[]){
    int cpid;
    char *command;
    char **parsedcmd;

    // set all signals to execute handlers in signal.c
    if(signal(SIGCHLD, sigchld_handler) == SIG_ERR) printf("sigchld error\n");
    if(signal(SIGTTIN, SIG_IGN)==SIG_ERR) printf("sigttin error\n");
    if(signal(SIGTTOU, SIG_IGN)==SIG_ERR) printf("sigttou error\n"); 
    if(signal(SIGINT, sigint_handler)==SIG_ERR) printf("sigint error\n");   
    if(signal(SIGTSTP, sigtstp_handler)==SIG_ERR) printf("sigtstp error\n");

    // start yash session
    printf("# ");
    while(command = getCommand()){
        // keep a copy of original command before parsing
       printf("original command is %s\n", command);
        char Command[MAX_COMMAND_SIZE];
        strcpy(Command, command);

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
          // printf("Invalid command\n%s\n", command);
            free(command);
            printf("# ");
            continue;
        } 

        // execute job commands (e.g. fg, bg, jobs)
        if(jobExec(parsedcmd)){
            free(command);
            printf("# ");
            continue;
        }

        // find the number of tokens
        int numTokens = findNumTokens(parsedcmd);
        // check if it's a bg or fg process
        bool bg;
        if(strcmp(parsedcmd[numTokens-1], "&") == 0){ 
            bg = true;
            char **temp = parsedcmd;

            parsedcmd = getSubCommand(temp, 0, numTokens-1);    // get arguments without the &
            Command[strlen(Command)-2] = '\0';
            free(temp);     // free previous arguments
        } else {
            bg = false;
        }
           
        cpid = fork();
        if(cpid == 0){
            // set child pid
            printf("in child\n");

            // creates new pg and makes child pg leader 
            setpgid(0, 0);     
         //   getInfo();

            // look through tokens and let yash execute if it's a valid command
            yashExec(parsedcmd, numTokens);
            exit(0);
        } else{
            // create a job and add it to the jobList
            Job *jobPtr;
            jobPtr = createJob(RUNNING, getJobid(), Command, cpid, bg, NULL);
            addJob(jobPtr);
            
            // wait if child is a foreground process (blocking wait)
            if(!bg){
                // wait for child if it's in the fg
                waitForChild(cpid);
            }

            // frees memory to prompt user for new command
            freeAll(command, parsedcmd);
            printf("# ");
        }
    }
}

