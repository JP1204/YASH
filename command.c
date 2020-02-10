#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#define MAX_COMMAND_SIZE 2000
#define MAX_TOKEN_SIZE 30


// parses the string using spaces as the delimeter
// returns null if the command entered is an empty or null string
char **parseString(char *str){
    // check for null or empty string
    if(str == NULL || (strcmp(str, "") == 0)) return NULL;

    int maxTokens = 1;
    // find maximum of tokens possible
    for(int i = 0; *(str+i) != '\0'; i++){
        if(*(str+i) == ' '){
            maxTokens++;
        }
    }

    // create array of char * or strings
    char **parsedcmd = (char **) calloc(maxTokens+1, sizeof(char *)); 
    char *token = strtok(str, " ");
   
    int i = 0;
    while(token != NULL){
        // allocate memory for the token and copy over the contents
        parsedcmd[i] = (char *) malloc((strlen(token)+1)*sizeof(char));
        strcpy(parsedcmd[i], token);

        // get next token
        token = strtok(NULL, " ");
        i++;
    }

    // reallocate based on number of tokens
    parsedcmd = (char **) realloc(parsedcmd, (i+1)*sizeof(char *));
    parsedcmd[i] = NULL;    // null terminating array of pointers
    return parsedcmd;
}


// get full command and return the string
char *getCommand(){
    char *command = (char *) malloc(sizeof(char)*MAX_COMMAND_SIZE);

    char c = getc(stdin);
    int i = 0;

    while(c != '\n'){
        command[i] = c;
        c = getc(stdin);
        i++;
    }

    command[i] = '\0';
    command = (char *) realloc(command, sizeof(char)*(strlen(command)+1));

    return command;
}


// returns pointer to new parsed command from token i to token j (for piping left and right side)
// including token i but not including token j
char **getSubCommand(char **parsedcmd, int i, int j){
    char **subCommand = (char **) calloc((j-i+1), sizeof(char *));
    char **head = subCommand;
       
    while(i < j){
        // copy parsedcmd into subCommand
        *subCommand = (char *) malloc(sizeof(char)*(strlen(parsedcmd[i])+1));
        strcpy(*subCommand, parsedcmd[i]);
        subCommand++;
        i++;
    }

    *subCommand = NULL;
    return head;
}


// print all the tokens
void printTokens(char **parsedcmd){
    printf("the tokens are: ");
    for(int i = 0; parsedcmd[i] != NULL; i++){
        printf("%s ", parsedcmd[i]);
    }
    printf("\n");
}


// return the amount of tokens
int findNumTokens(char **parsedcmd){
    int numTokens = 0;
    char **temp = parsedcmd;

    while(*temp != NULL){
        numTokens++;
        temp++;
    }

    return numTokens;
}


// get information about current process
int getInfo(){
    printf("the session leader is %d\n", getsid(0));
    printf("pid is %d and pg id is %d\n", getpid(), getpgid(0));
    printf("the fg pgid is %d\n\n", tcgetpgrp(0));
}


// free everything to take in more commands
void freeAll(char *command, char**parsedcmd){
    free(command);
    char **start = parsedcmd;

    while(*parsedcmd != NULL){
        free(*parsedcmd);
        parsedcmd++;
    }

    free(start);
}
    
    
