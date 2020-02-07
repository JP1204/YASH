// implementing all the features of the shell
// e.g. file redirection, piping, signals, etc.

void redirectOut(char **command, char *file);

void redirectIn(char **command, char *file);

void redirectErr(char **command, char *file);

void piping(char **leftCommand, char **rightCommand);
