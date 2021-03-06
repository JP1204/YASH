#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdbool.h>


// Process structure
typedef struct Process {
    char *command;
    char **argv;
    struct Process *next;
} Process;


// Job structure
typedef enum {RUNNING, STOPPED, DONE} STATE;

typedef struct Job {
    STATE state; 
    int jobid;  // unique value
    char *jobString;
    //char **argv;
    int pgid;   // used for signal handling
    bool bg;
    struct Job *next;
} Job;


// Linked List of Jobs
typedef struct {
    Job *head;
} jobList;

Process *createProcess(char *command);

void addProcess(Process *process);

Job *createJob(STATE st, int jobid, char *jobString, int pgid, bool bg, Job *next);

void addJob(Job *job);

void printJobs();

int removeJob(int pgid);

Job *getJob(int pgid);

int getJobid();

int change_process_state(pid_t pid, STATE jobState);

Job *getRecentJob();

void fgExec();

Job *fgGetJob();

void bgExec();

Job *bgGetJob();

void waitForChild();

int getNumJobs();

void freeProcess(Process *p);

void freeJob(Job *job);

void removeAllProcesses();

void removeAllJobs();
