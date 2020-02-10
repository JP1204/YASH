#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "jobs.h"


// allocates space for job struct and returns pointer
Job *createJob(STATE st, int jobid, char *jobString, int pgid, bool bg, Job *next){
    Job *jobPtr = (Job *) malloc(sizeof(Job));

    // copying all job data into struct
    jobPtr->state = st;
    jobPtr->jobid = jobid;
    jobPtr->jobString = (char *) malloc((strlen(jobString)+1)*sizeof(char));
    strcpy(jobPtr->jobString, jobString);
    jobPtr->pgid = pgid;
    jobPtr->bg = bg;
    jobPtr->next = next;
    return jobPtr;
}


// allocates space for head ptr in linked list
jobList *createJobList(){
    jobList *list = (jobList *) malloc(sizeof(jobList));
    list->head = NULL;
    return list;
}


// inserts Job at the end of the list
void addJob(Job *job, jobList *list){
  printf("adding new job\n");
    Job *temp = list->head;
    if(temp == NULL){
        // no jobs in list
        list->head = job;
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
void printJobs(jobList *list){
    Job *temp = list->head;
    int count = 1;
    char currentJob;
    char jobState[10];

    if(temp == NULL){ printf("oh no\n");}
    if(list->head == NULL){ printf("oh no 2\n");};

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
        printf("[%d] %c %s\t\t%s\n", count, currentJob, jobState, temp->jobString);
        temp = temp->next;
        count++;
    }
}


// find job with jobid and removes it from job list while freeing it's memory
// returns -1 if job was not found and 1 if deletion was successful
int removeJob(int jobid, jobList *list){
    Job *temp = list->head;
    Job *prev = NULL;

    while(temp != NULL){
        if(jobid == temp->jobid){
            if(prev == NULL){
                // only one job in list
                free(temp);
                list->head = NULL;
                return 1;
            } else {
                prev->next = temp->next;
                free(temp);
                return 1;
            }
        }
    }

    return -1;
}


// changes process state for jobid to Jobstate
// returns 1 if successful, 0 otherwise
int change_process_state(pid_t pid, STATE jobState, jobList *list){
    Job *temp = list->head;

    while(temp != NULL){
        if(temp->jobid == (int) pid){
            temp->state = jobState;
            return 1;
        }
    }
   
    return 0;
}


// get most recent job for signal handling

