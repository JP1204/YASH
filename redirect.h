// implementing all the features of the shell
// e.g. file redirection, piping, signals, etc.

void redirectOut(char *file);

void redirectIn(char *file);

void redirectErr(char *file);

void piping(char **leftCommand, char **rightCommand);
