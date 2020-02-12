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
    
    pid=waitpid(-1, &status, WNOHANG|WUNTRACED);
        
    // check if there are any zombie children
    // ex. if background process terminated or stopped
    if(pid > 0){
        printf("child %d has terminated with status %d\n", pid, status);

        if(WIFEXITED(status)){
            // remove job from list if it's a pg leader
            if(getJob(pid)){
                // change process state so parent can terminate child
              printf("setting %d to done\n", pid);
                change_process_state((pid_t) pid, DONE);
            }
        } else if(WIFSTOPPED(status)){
            // change state for child if it's the pg leader
            if(getJob(pid)){
               printf("setting %d to stopped\n", pid);
                change_process_state((pid_t) pid, STOPPED);
            }
        } else if(WIFSIGNALED(status)){
            printf("child killed by signal %d\n", WTERMSIG(status));
        }
    } else{
        printf("there was an error: %s\n", strerror(errno));
    }
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
      printf("child in background, can't interrupt\n");
        return;     // do nothing if child isn't in fg
    }
        
    int pgid = recentJob->pgid;

    // remove job from the list
    change_process_state(pgid, DONE);

    printf("sending %d signal to pg %d\n",signum, pgid);
    kill(-pgid, SIGINT);
}


void sigtstp_handler(int signum){
    printf("in the sigtstp handler with signum = %d\n", signum);

    // send the signal to most recent job
    Job *recentJob = getRecentJob();
    printf("the most recent job is %s\n", recentJob->jobString);
    if(recentJob == NULL){
       printf("no more jobs\n");
        return;
    } else if(recentJob->state == STOPPED){
       printf("should not be here\n");
        return;     // process already stopped
    }

    int pgid = recentJob->pgid;
    printf("sending %d signal to pg %d\n",signum, pgid);
    kill(-pgid, SIGTSTP);
    recentJob->state = STOPPED;     // update job struct
    recentJob->bg = true;


}
