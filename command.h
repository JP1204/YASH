// includes all the command manipulation functions

#define MAX_COMMAND_SIZE 2000
#define MAX_TOKEN_SIZE 30

char **parseString(char *str);

char *getCommand();

char **getSubCommand(char **parsedcmd, int i, int j);

int findNumTokens(char **parsedcmd);

void printTokens(char **parsedcmd);

void getInfo();

void freeAll(char *command, char **parsedcmd);
