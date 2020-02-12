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
#include <errno.h>


void sigchld_handler(int signum){
   printf("inside sigchld handler\n");
    int pid;
    int status;
    
    pid=waitpid(-1, &status, WNOHANG);
        
    // check if there are any zombie children
    // ex. if background process terminated or stopped
    if(pid > 0){
        printf("child %d has terminated with status %d\n", pid, status);

        if(WIFEXITED(status)){
            // remove job from list if it's a pg leader
            if(getJob(pid)){
               printf("removing job %d\n", pid);
                removeJob(pid);
            }
        } else if(WIFSTOPPED(status)){
            // change state for child if it's the pg leader
            if(getJob(pid)){
               printf("setting %d to stopped\n", pid);
                change_process_state((pid_t) pid, STOPPED);
            }
        }
    }

    // check if most recent child has stopped or exited
}


void sigint_handler(int signum){
    // interrupt the foreground process
    printf("in the sigint handler with signum = %d\n", signum);

    // send the signal to most recent job
    Job *recentJob = getRecentJob();
    if(recentJob == NULL){
     // printf("there are no jobs to interrupt");
        return;     // do nothing if there are no processes
    } else if(recentJob->bg){
     // printf("child in background, can't interrupt\n");
        return;     // do nothing if child isn't in fg
    }
        
    int pgid = recentJob->pgid;

    // remove job from the list
    if(removeJob(pgid)) printf("removed pgid %d from the list\n", pgid);
    else printf("didn't remove job\n");

    printf("sending %d signal to pg %d\n",signum, pgid);
    kill(-pgid, SIGINT);
}


void sigtstp_handler(int signum){
    printf("in the sigstop handler with signum = %d\n", signum);

    // send the signal to most recent job
    Job *recentJob = getRecentJob();
    if(recentJob == NULL){
        return;
    } else if(recentJob->state == STOPPED){
        return;     // process already stopped
    }

    int pgid = recentJob->pgid;
    recentJob->state = STOPPED;     // update job struct
    recentJob->bg = true;

    printf("sending %d signal to pg %d\n",signum, pgid);
    kill(-pgid, SIGTSTP);
}
