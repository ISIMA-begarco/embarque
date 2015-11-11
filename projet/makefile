
CC = gcc
OBJ = claude.o file.o

capteur : $(OBJ)
	$(CC) $(OBJ) -o capteur
	
claude.o : claude.c
	$(CC) -c claude.c

file.o : file.c file.h
	$(CC) -c file.c

clean :
	rm *.o
