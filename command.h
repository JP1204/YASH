
// includes all the command manipulation functions

char **parseString(char *str);

char *getCommand();

char **getSubCommand(char **parsedcmd, int i, int j);

int findNumTokens(char **parsedcmd);

void printTokens(char **parsedcmd);

void freeAll(char *command, char **parsedcmd);
