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

    int numTokens = 1;
    // find number of tokens
    for(int i = 0; *(str+i) != '\0'; i++){
        if(*(str+i) == ' '){
            numTokens++;
        }
    }

  printf("num of tokens = %d\n", numTokens);

    // create array of char * or strings
    char **parsedcmd = (char **) calloc(numTokens+1, sizeof(char *)); 
  printf("here too\n");
    char *token = strtok(str, " ");
   
    // allocate memory for the token and copy over the contents
    parsedcmd[0] = (char *) malloc((strlen(token)+1)*sizeof(char));
    strcpy(parsedcmd[0], token);

 printf("wow still here\n");
    for(int i = 1; i < numTokens; i++){
        token = strtok(NULL, " ");

        if(token == NULL){
            // indicates excessive spaces (empty string used)
            token = "\0";
        }

        parsedcmd[i] = (char *) malloc((strlen(token)+1)*sizeof(char));
        strcpy(parsedcmd[i], token);
      printf("the token is %s with length %ld\n", parsedcmd[i], strlen(token));
    }

    parsedcmd[numTokens] = NULL;    // null terminating pointer array
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
    printf("the length of the string is %ld\n", strlen(command));

    return command;
}


// free everything to take in more commands
void freeAll(char *command, char**parsedcmd){
    free(command);

    while(*parsedcmd != NULL){
        free(*parsedcmd);
        parsedcmd++;
    }

    free(parsedcmd);
}
    
    
