#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "command.h"
#include "redirect.h"
#include "exec.h"
#include "signal.h"
extern int childPID;


int main(int argc, char *argv[]){
    int cpid;
    char *command;
    char **parsedcmd;

    // start list for jobs until yash finishes
    jobList *allJobs = createJobList();

    // set all signals to execute handlers in signal.c
  //  if(signal(SIGCHLD, signal_handler) == SIG_ERR) printf("sigchld error\n");
    if(signal(SIGTTOU, SIG_IGN)==SIG_ERR) printf("sigttou error\n");   // allows child to give control back to parent

    if(signal(SIGINT, signal_handler)==SIG_ERR) printf("sigint error\n");   // allows child to give control back to parent
//    if(signal(SIGTSTP, sigtstp_handler)==SIG_ERR) printf("sigtstp error\n");

    // start yash session

    printf("# ");
    while(command = getCommand()){
        // keep a copy of original command before parsing
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
            printf("Invalid command\n%s\n", command);

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
          printf("new command is ");
          printTokens(parsedcmd);
            free(temp);     // free previous arguments
        } else {
            bg = false;
        }

        cpid = fork();
        if(cpid == 0){
            // set child pid
            childPID = getpid();
            printf("child pid is %d\n", childPID);

            printf("in child\n");

//            getInfo();

            // check for job commands (e.g. fg, bg, jobs)
            jobExec(parsedcmd, allJobs);
           
            // creates new pg and makes child pg leader 
            setpgid(0, 0);     
            // if(signal(SIGINT, sigint_handler) == SIG_ERR) printf("second sigint error\n");
           

            // look through tokens and let yash execute if it's a valid command
            yashExec(parsedcmd, numTokens);
            exit(0);
        } else{
            // set parent pid
            parentPID = getpid(); 

          // getInfo();
            // create a job and add it to the jobList
            Job *jobPtr = createJob(RUNNING, getpid(), Command, bg, NULL);
            addJob(jobPtr, allJobs);

            // wait if child is a foreground process (blocking wait)
            if(!bg){
              printf("pg %d is now in the foreground\n", tcgetpgrp(0));
                int status; 
                int cPID = waitpid(cpid, &status, WUNTRACED);  
              printf("finished waiting for foreground process, child %d exited\n", cPID);
              printf("the status is %d\n", status);

                // see if the child was interrupted or stopped
                if(WIFSTOPPED(status)){
                    printf("the child was stopped by signal %d", (int) WSTOPSIG(status));
                }
            }

            // getInfo();

            // otherwise continue
            // frees memory to prompt user for new command
            freeAll(command, parsedcmd);
            printf("# ");
        }
    }
}

