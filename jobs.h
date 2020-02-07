#include <stdio.h> 

// job structure
typedef struct {
    enum state {RUNNING, STOPPED, DONE};
    int jobid;
    char *jobString;
    bool bg;
}

