#include "command.h"

char **parseString(char *str){
    if(str == NULL) return NULL;

    int numTokens = 1;
    // find number of tokens
    for(int i = 0; *(str+i) != '\0'; i++){
        if(*(str+i) == ' '){
            numTokens++;
        }
    }

    char **parsedcmd = (char **) malloc(numTokens*sizeof(char *));
    char *token = strtok(str, " ");

    char **start = parsedcmd;
    while(token != NULL){
        printf("%s\n", token);
        parsedcmd = &token;
        token = strtok(NULL, " ");
        parsedcmd++;
    }

    return start;
}


char *getCommand(){


    return NULL;

}
