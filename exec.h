#include <stdio.h>
#include "jobs.h"

// reads the tokens and executes yash commands
// when it sees <, >, or |
int yashExec(char **parsedcmd, int numTokens);

void jobExec(char **command, jobList *allJobs);
