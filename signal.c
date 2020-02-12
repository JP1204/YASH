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
    int pid;
    int status;
    
    pid=waitpid(-1, &status, WNOHANG|WUNTRACED);
        
    // check if there are any zombie children
    // ex. if background process terminated or stopped
    if(pid > 0){
        if(WIFEXITED(status)){
            // remove job from list if it's a pg leader
            if(getJob(pid)){
                // change process state so parent can terminate child
                change_process_state((pid_t) pid, DONE);
            }
        } else if(WIFSTOPPED(status)){
            // change state for child if it's the pg leader
            if(getJob(pid)){
                change_process_state((pid_t) pid, STOPPED);
            }
        } else if(WIFSIGNALED(status)){
         //   printf("child killed by signal %d\n", WTERMSIG(status));
        }
    } 
}


void sigint_handler(int signum){
    // interrupt the foreground process
    // send the signal to most recent job
    Job *recentJob = getRecentJob();
    if(recentJob == NULL){
        return;     // do nothing if there are no processes
    } else if(recentJob->bg){
        return;     // do nothing if child isn't in fg
    }
        
    int pgid = recentJob->pgid;

    // remove job from the list
    change_process_state(pgid, DONE);
    kill(-pgid, SIGINT);
}


void sigtstp_handler(int signum){
    // send the signal to most recent job
    Job *recentJob = getRecentJob();
    if(recentJob == NULL){
        return;
    } else if(recentJob->state == STOPPED){
        return;     // process already stopped
    }

    int pgid = recentJob->pgid;
    kill(-pgid, SIGTSTP);
    recentJob->state = STOPPED;     // update job struct
}
