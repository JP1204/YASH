#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>


// redirects stdout of command to file
void redirectOut(char **command, char *file){
    int cpid = fork();

    if(cpid == 0){
        printf("printing output of %s to file %s\n", command[0], file);
        // opens file with read and write permissions
        // but creates the file if it doesn't exist
        int fd = open(file, O_WRONLY|O_APPEND|O_CREAT, 0644);
 
        // check if open succeeded
        if(fd == -1){
            printf("file %s could not be opened\n", file);
            exit(0);
        }

        dup2(fd, STDOUT_FILENO);   // changes stdout to the file pointer
        execvp(command[0], command);
        exit(0);   // indicates exec fail
    } else{
        // wait for child process to finish
        wait(NULL);
    }
}


void redirectIn(char **command, char *file){
    int cpid = fork();

    if(cpid == 0){
        printf("feeding file %s into commmand %s", file, command[0]);
    } else{
        // wait for child process to finish
        wait(NULL);
    }
}

    

