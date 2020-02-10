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
    printf("printing output to file %s\n", file);
    // opens file with read and write permissions
    // but creates the file if it doesn't exist
    int fd = open(file, O_WRONLY|O_TRUNC|O_CREAT, 0644);
 
    // check if open succeeded
    if(fd == -1){
        printf("file %s could not be opened\n", file);
        exit(0);
    }

    dup2(fd, STDOUT_FILENO);   // changes stdout to the file pointer
}


void redirectIn(char *file){
    printf("feeding file %s in\n", file);
    int fd = open(file, O_RDONLY, 0644);

    // check if open succeeded (file exists)
    if(fd == -1){
        // fail command if file doesn't exist
        printf("file %s does not exist\n", file);

        exit(0);
    }

    // change stdin to file
    dup2(fd, STDIN_FILENO);
}


void redirectErr(char *file){
    printf("printing error to %s\n", file);
    int fd = open(file, O_RDONLY|O_CREAT, 0644);

    // check if open succeeded 
    if(fd == -1){
        printf("file %s could not be opened\n", file);
        exit(0);
    }

    // change stderr to file
    dup2(fd, STDERR_FILENO);
}


void piping(char **leftCommand, char **rightCommand){
    int pipefd[2], status, done=0;
    pipe(pipefd);

    int cpid = fork();
    if(cpid == 0){  // left child process
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO); // set output to left of pipe to send
        
        int numTokens = findNumTokens(leftCommand);
        yashExec(leftCommand, numTokens);
        printf("left command failed\n");
    }

    cpid = fork();
    if(cpid == 0){  // right child process
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);  // set input to right of pipe to receive

        int numTokens = findNumTokens(rightCommand);
        yashExec(rightCommand, numTokens);
        printf("right command failed\n");
    }

    close(pipefd[0]);
    close(pipefd[1]);

    waitpid(-1, &status, 0);
    waitpid(-1, &status, 0);
}





