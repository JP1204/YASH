#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include "jobs.h"
#include "command.h"


void signal_handler(int signum){
    switch(signum){
        case SIGINT:
            printf("sigint handler called, sending kill to %d\n", childPID);
            kill(-childPID, SIGINT);
            break;
    }
}


void sigchld_handler(int signum){
   printf("inside sigchld handler\n");
//    getInfo();
    int pid;
    int status;
    
    pid=waitpid(-1, &status, WNOHANG); 
        printf("child %d has terminated with status %d\n", pid, status);

    int count = 0;
    // find out how child exited
    if (WIFEXITED(status)) {  
        printf("child %d exited, status=%d\n", pid, WEXITSTATUS(status)); count++;
    } else if (WIFSIGNALED(status)) { 
        printf("child %d killed by signal %d\n", pid, WTERMSIG(status)); count++;
    } else if (WIFSTOPPED(status)) {  
        printf("%d stopped by signal %d\n", pid,WSTOPSIG(status));
        printf("Sending CONT to %d\n", pid);  
        sleep(4); 
        //sleep for 4 seconds before sending CONT  
        kill(pid,SIGCONT);
    } else if (WIFCONTINUED(status)) {  
        printf("Continuing %d\n",pid);
    } else{
        printf("exiting sigchld\n");
    }

    // give control back to parent
    tcsetpgrp(STDIN_FILENO, parentPID);
}


void sigint_handler(int signum){
    // interrupt the foreground process
    printf("in the sigint handler with signum = %d\n", signum);
    printf("the foreground pg is %d\n\n", getpid());
    kill(-childPID, SIGINT);
}


void sigtstp_handler(int signum){
    printf("in the sigstop handler with signum = %d\n", signum);
    kill(-childPID, SIGTSTP);
}
