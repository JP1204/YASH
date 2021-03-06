#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#include "exec.h"
#include "command.h"


// redirects stdout of command to file
void redirectOut(char *file){
    // opens file with read and write permissions
    // but creates the file if it doesn't exist
    int fd = open(file, O_WRONLY|O_TRUNC|O_CREAT, 0644);
 
    // check if open succeeded
    if(fd == -1){
        exit(0);
    }

    dup2(fd, STDOUT_FILENO);   // changes stdout to the file pointer
}


void redirectIn(char *file){
    int fd = open(file, O_RDONLY, 0644);

    // check if open succeeded (file exists)
    if(fd == -1){
        // fail command if file doesn't exist
        exit(0);
    }

    // change stdin to file
    dup2(fd, STDIN_FILENO);
}


void redirectErr(char *file){
    int fd = open(file, O_WRONLY|O_TRUNC|O_CREAT, 0644);

    // check if open succeeded 
    if(fd == -1){
        exit(0);
    }

    // change stderr to file
    dup2(fd, STDERR_FILENO);
}


void piping(char **leftCommand, char **rightCommand){
    int pipefd[2], status, done=0;
    pipe(pipefd);
    int c1pid = getpid();

    int cpid = fork();
    if(cpid == 0){
        close(pipefd[0]);   // closes right end of pipe for left command
        dup2(pipefd[1], STDOUT_FILENO); // set output to left of pipe to send
    
        // puts child1 into main child
        setpgid(0, c1pid);

        int numTokens = findNumTokens(leftCommand);
        yashExec(leftCommand, numTokens);
    }

    cpid = fork();
    if(cpid == 0){  // right child process
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);  // set input to right of pipe to receive

        // puts child2 into same pg as child1
        setpgid(0, c1pid); 

        int numTokens = findNumTokens(rightCommand);
        yashExec(rightCommand, numTokens);
    }

    close(pipefd[0]);
    close(pipefd[1]);

    waitpid(-1, &status, 0);
    waitpid(-1, &status, 0);
}





