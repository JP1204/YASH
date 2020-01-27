#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
      //printf("the token is %s with length %ld\n", token, strlen(token));

        // allocate memory for the token and copy over the contents
        parsedcmd[i] = (char *) malloc((strlen(token)+1)*sizeof(char));
        strcpy(parsedcmd[i], token);

        // get next token
        token = strtok(NULL, " ");
        i++;
    }

    // reallocate based on number of tokens
 // printf("actual number of tokens %d\n", i);
    parsedcmd = (char **) realloc(parsedcmd, (i+1)*sizeof(char *));
    parsedcmd[i] = NULL;    // null terminating array of pointers
    return parsedcmd;
}


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
//    printf("the length of the string is %ld\n", strlen(command));

    return command;
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
    
    
