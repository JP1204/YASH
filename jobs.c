#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include "jobs.h"
#include "command.h"

extern Job * jobListHead;
extern Process *processListHead;

// allocates space for Process struct and returns pointer
Process *createProcess(char *command){
    Process *p = (Process *) malloc(sizeof(Process));

    // copying all process data into struct
    p->command = (char *) malloc(sizeof(char)*(strlen(command)+1));
    strcpy(p->command, command);
    p->argv = parseString(command);
    p->next = NULL;
 
    return p;
}


// add process to end of list
void addProcess(Process *process){
    Process *temp = processListHead;

    if(temp == NULL){
        // no jobs in list
        processListHead = process;
        process->next = NULL;
        return;
    }

    Process *prev = NULL;
    while(temp != NULL){
        prev = temp;
        temp = temp->next;
    }

    // inserts job
    prev->next = process;
    process->next = NULL;
} 


// allocates space for job struct and returns pointer
Job *createJob(STATE st, int jobid, char *jobString, int pgid, bool bg, Job *next){
    Job *jobPtr = (Job *) malloc(sizeof(Job));

    // copying all job data into struct
    jobPtr->state = st;
    jobPtr->jobid = jobid;
    jobPtr->jobString = (char *) malloc(sizeof(char)*(strlen(jobString)+1));
    strcpy(jobPtr->jobString, jobString);
    jobPtr->pgid = pgid;
    jobPtr->bg = bg;
    jobPtr->next = next;

    return jobPtr;
}


// inserts Job at the end of the list
void addJob(Job *job){
    Job *temp = jobListHead;

    if(temp == NULL){
        // no jobs in list
        jobListHead = job;
        job->next = NULL;
        return;
    }

    Job *prev = NULL;
    while(temp != NULL){
        prev = temp;
        temp = temp->next;
    }

    // inserts job
    prev->next = job;
    job->next = NULL;
}


// prints all the jobs similar to the job command in bash
void printJobs(){
    Job *temp = jobListHead;

    char currentJob;
    char jobState[10];

  //  if(temp == NULL){ printf("oh no\n");}
  //  if(jobListHead == NULL){ printf("oh no 2\n");};

//    printf("got here\n");
    while(temp != NULL){
        // check if job is most recent job (last in list)
        if(temp->next == NULL) { currentJob = '+'; }
        else { currentJob = '-'; }

        switch(temp->state){
            case RUNNING:
                strcpy(jobState, "Running"); break;
            case STOPPED:
                strcpy(jobState, "Stopped"); break;
            case DONE:
                strcpy(jobState, "Done"); break;
        }
        printf("[%d] %c %s\t\t%s\n", temp->jobid, currentJob, jobState, temp->jobString);
    //    printf("in loop\n");
        temp = temp->next;
    }
}


// find job with pgid and removes it from job list while freeing its memory
// returns 0 if job was not found and 1 if deletion was successful
int removeJob(int pgid){
    Job *temp = jobListHead;
    Job *prev = NULL;

    while(temp != NULL){
        //printf("the job is %s\n", temp->jobString);

        if(pgid == temp->pgid){
            if(prev == NULL){
                // only one job in list
                freeJob(temp);
                jobListHead = NULL;
              //  printf("remove sucessful");
                return 1;
            } else {
                prev->next = temp->next;
                freeJob(temp);
                return 1;
            }
        }

        prev = temp;
        temp = temp->next;
    }

    return 0;
}


// get job with pgid
// returns NULL if not found
Job *getJob(int pgid){
    Job *temp = jobListHead;

    while(temp != NULL){
        if(temp->pgid == pgid){
            return temp;
        }
        
        temp = temp->next;
    }

    return NULL;
}


// gets the highest job in list and adds 1 for a unique id
int getJobid(){
    Job *temp = jobListHead;

    int maxID = 0;
    while(temp != NULL){
        if(temp->jobid > maxID){
            maxID = temp->jobid;
        }

        temp = temp->next;
    }
   
    return maxID+1;
}
       

// changes process state for jobid to Jobstate
// returns 1 if successful, 0 otherwise
int change_process_state(pid_t pid, STATE jobState){
    Job *temp = jobListHead;

    while(temp != NULL){
        if(temp->pgid == (int) pid){
            temp->state = jobState;
            return 1;
        }

        temp = temp->next;
    }
   
    return 0;
}


// get most recent job for signal handling
// returns NULL if there are no jobs
Job *getRecentJob(){
    Job *temp = jobListHead;

//printf("finally here\n");
    Job *prev = NULL;
 //   printf("noooo\n");
    while(temp != NULL){
  //      printf("another one\n");
        prev = temp;
        temp = temp->next;
   //     printf("another loop\n");
    }

    return prev;
}


// send SIGCONT to most recent background or stopped process
// and prints the process to stdout
void fgExec(){
    Job *jobPtr = getRecentJob();
   
  printf("%s", jobPtr->jobString);
    if(jobPtr->bg)
        printf(" &\n");
    else
        printf("\n");

    if(jobPtr == NULL){
        return;
    }

    int pgid = jobPtr->pgid;
    jobPtr->state = RUNNING;
    jobPtr->bg = false;
    
    kill(pgid, SIGCONT);
    
    // wait for new fg process to stop or terminate
    waitForChild(pgid);
}


// gets most recent background or stopped process
// returns NULL if there is none
Job *fgGetJob(){
   printf("getting fg job now\n");
    Job *temp = jobListHead;
    Job *recent = NULL;     // keeps track of most recent process for fg

    while(temp != NULL){
      //  printf("job pid %d\n", temp->pgid);
        if(temp->bg || (temp->state == STOPPED)){
            recent = temp;
        }
        
        temp = temp->next;
    }

    if(recent == NULL){
        return NULL; 
    }
    
    return recent;
}


int getNumJobs(){
    Job *temp = jobListHead;

    int count = 0;
    while(temp != NULL){
        count++;
        temp = temp->next;
    }

    return count;
}


// called instead of waitpid for child pid 
// allows parent to do blocking wait until child stops or terminates
// returns 1 if there were no errors, 0 otherwise
void waitForChild(int pid){
    // get job with pgid in parameter
    Job *childJobPtr = getJob(pid);
    if(childJobPtr == NULL) return;

   printf("gonna wait on child %d\n", pid);

    // waits until child changes state
    while(1){
     //   printf("stuck  \n");
        if(childJobPtr){
      //      printf("child still here\n");
            if(childJobPtr->state != RUNNING){
                // remove job from list
                if(childJobPtr->state == DONE){
                    printf("stop waiting because child finished\n");
                    removeJob(childJobPtr->pgid);
                    return;
                }

         //      printf("hello, %d?\n", pid);
                printf("stop waiting because child stopped\n");
                return;   // child stopped
            }
        } else{
            return; // child terminated
        }
    }
}
        

// frees process and all its variables
void freeProcess(Process *p){
    freeAll(p->command, p->argv);
    free(p);
}


// frees job and all its variables
void freeJob(Job *job){
    free(job->jobString);
    free(job);
}


void removeAllProcesses(){
    Process *temp = processListHead;
    if(temp == NULL) return;

    Process *next = NULL;
    while(temp != NULL){
        next = temp->next;
        freeProcess(temp);
        temp = next;
    }
}
            

// removes all jobs
void removeAllJobs(){
    Job *temp = jobListHead;
    if(temp == NULL) return;

    Job *next = NULL;
    while(temp != NULL){
        next = temp->next;
        freeJob(temp);
        temp = next;
    }
}
        
