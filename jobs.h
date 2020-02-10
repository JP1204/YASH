#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>

// global pid variables for main parent and child
int parentPID;
int childPID;

// Job structure
typedef enum {RUNNING, STOPPED, DONE} STATE;

typedef struct Job {
    STATE state; 
    int jobid;  // unique value
    char *jobString;
    int pgid;   // used for signal handling
    bool bg;
    struct Job *next;
} Job;


// Linked List of Jobs
typedef struct {
    Job *head;
} jobList;


Job *createJob(STATE st, int jobid, char *jobStr, bool bg, Job *next);

jobList *createJobList();

void addJob(Job *job, jobList *list);

void printJobs(jobList *list);

int removeJob(int jobid, jobList *list);

int change_process_state(pid_t pid, STATE jobState, jobList *list);

Job *getRecentJob(jobList *list);
