#makefile

yash: yash.o command.o exec.o redirect.o signal.o jobs.o
	gcc -o yash yash.o command.o exec.o redirect.o signal.o jobs.o 

yash.o: yash.c command.h redirect.h exec.h signal.h
	gcc -c yash.c 
command.o: command.c jobs.h
	gcc -c command.c $@
exec.o: exec.c command.h redirect.h jobs.h
	gcc -c exec.c $@
redirect.o: redirect.c exec.h command.h
	gcc -c redirect.c $@
signal.o: signal.c jobs.h command.h
	gcc -c signal.c $@
jobs.o: jobs.c jobs.h command.h
	gcc -c jobs.c $@
