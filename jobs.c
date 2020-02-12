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

// allocates space for job struct and returns pointer
Job *createJob(STATE st, int jobid, char *jobString, int pgid, bool bg, Job *next){
    Job *jobPtr = (Job *) malloc(sizeof(Job));

    // copying all job data into struct
    jobPtr->state = st;
    jobPtr->jobid = jobid;
    jobPtr->jobString = (char *) malloc(sizeof(char)*(strlen(jobString)+1));
    strcpy(jobPtr->jobString, jobString);
    jobPtr->argv = parseString(jobString);
    jobPtr->pgid = pgid;
    jobPtr->bg = bg;
    jobPtr->next = next;
    return jobPtr;
}


// inserts Job at the end of the list
void addJob(Job *job){
//    extern Job *jobListHead;
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
 //   extern Job *jobListHead;
    Job *temp = jobListHead;

    char currentJob;
    char jobState[10];

    //if(temp == NULL){ printf("oh no\n");}
    //if(jobListHead == NULL){ printf("oh no 2\n");};

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
        temp = temp->next;
    }
}


// find job with pgid and removes it from job list while freeing its memory
// returns 0 if job was not found and 1 if deletion was successful
int removeJob(int pgid){
  //  extern Job *jobListHead;
    Job *temp = jobListHead;
    Job *prev = NULL;

    while(temp != NULL){
        if(pgid == temp->pgid){
            if(prev == NULL){
                // only one job in list
                freeJob(temp);
                jobListHead = NULL;
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
    }

    return maxID+1;
}
       

// changes process state for jobid to Jobstate
// returns 1 if successful, 0 otherwise
int change_process_state(pid_t pid, STATE jobState){
   // extern jobListHead;
    Job *temp = jobListHead;

    while(temp != NULL){
        if(temp->pgid == (int) pid){
            temp->state = jobState;
            return 1;
        }
    }
   
    return 0;
}


// get most recent job for signal handling
// returns NULL if there are no jobs
Job *getRecentJob(){
    //extern jobListHead;
    Job *temp = jobListHead;

    Job *prev = NULL;
    while(temp != NULL){
        prev = temp;
        temp = temp->next;
    }

    return prev;
}


// send SIGCONT to most recent background or stopped process
// and prints the process to stdout
void fgExec(){
    Job *jobPtr = fgGetJob();
   
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
        printf("job pid %d\n", temp->pgid);
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


// called instead of waitpid for child pid 
// allows parent to do blocking wait until child stops or terminates
// returns 1 if there were no errors, 0 otherwise
void waitForChild(int pid){
    // get job with pgid in parameter
    Job *childJobPtr = getJob(pid);
    if(childJobPtr == NULL) return;

    // waits until child changes state
    while(1){
        if(childJobPtr){
            if(childJobPtr->state != RUNNING){
                return;   // child stopped
            }
        } else{
            return; // child terminated
        }
    }
}
        

// frees job and all its variables
void freeJob(Job *job){
    for(int i = 0; (job->argv)[i] != NULL; i++){
        free((job->argv[i]));
    }

    free(job->argv);
    free(job);
}


// removes all jobs
void removeAllJobs(){
    Job *temp = jobListHead;
    if(temp == NULL) return;

    Job *prev = temp;
    temp = temp->next;
    while(temp != NULL){
        freeJob(prev);
        prev = temp;
        temp = temp->next;
    }
}
        


