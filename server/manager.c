#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>                            
#include <termios.h>
#include <pthread.h>

#include "file.h"

// Ou on va lire les infos
#define TTY "/dev/ttyAMA0"
int FD;
FILE* FICHIER;

// Ouverture du port
int openPort(void) {
	struct termios options;
	int fd,i;
	fd = open(TTY, O_RDWR);
	if (fd==-1) {
		printf("Erreur ouverture port !!!\n");
	}
	else {
		fcntl(fd,F_SETFL,0);
		//    fcntl(fd, F_SETFL, FNDELAY); //ne pas bloquer sur le read
		tcgetattr(fd,&options);
		usleep(10000);
		cfsetospeed(&options,B115200);
		cfsetispeed(&options,B115200);
		options.c_cflag |= (CLOCAL | CREAD);
		options.c_cflag &= ~PARENB; /* Parite   : none */
		options.c_cflag &= ~CSTOPB; /* Stop bit : 1    */
		options.c_cflag &= ~CSIZE;  /* Bits     : 8    */
		options.c_cflag |= CS8;
		options.c_cflag &= ~CRTSCTS;
		// options.c_iflag &= ~(IXON);
		options.c_iflag &= ~(IXON | IXOFF | IXANY);
		options.c_oflag &= ~OPOST; // raw output
		options.c_lflag &= ~(ICANON | ECHO | ECHONL|IEXTEN | ISIG);
		// c_cc
		options.c_cc[VMIN]= 1;
		options.c_cc[VTIME]= 4;
		tcflush(fd,TCIOFLUSH); // flushIO buffer
		tcsetattr(fd, TCSANOW,&options); // set new configure immediately
		//    tcflush(fd,TCIOFLUSH);
		usleep(10000);
	}

	return fd;	
}

// Fonction de lecture de trame
int threadLecture() {
printf("oui");
	int i = 0, ok = 0;
	char c;
	char* trame = (char*)malloc(21 * sizeof(char));
printf("dring dring");
	file_t* file = create_file(); // File des trames recues correctes
printf("allo");	
	while(1) {
		//usleep(1); // Pour la synchro
		// TODO attendre son tour pour aller lire
printf("w principal");
		do {printf("w ");
			read(FD, &c, 1); // Lecture du caractere
			if(ok==1 || c=='X') {
				ok = 1;
				trame[i] = c;
				i++; // va chercher les autres
			}
		} while(i < 20 && c!='W');
printf("je sors du premier while");
		// Verification et mise dans file de la trame
		if(i == 20) { // taille correcte
			if(trame[19] == 'W') { // fin correcte
				if(trame[0]=='X' && trame[1]=='0' && trame[2]=='1') { // bon code, donc bonne trame
					trame[20] = '\0'; // Ajout du caractere de fin
printf("J'attends le mutex");
					while(pthread_mutex_lock(&(file->mutex)) != 0) { // Je bloque la file
						push_file(file, trame); // Ajout de la trame dans la file, pour le thread acquitement
						pthread_mutex_unlock(&(file->mutex)); // Je debloque la file
printf("Trame push : %s\n", trame); // affichage de test
					}
				}
			}
		}
		
		// RAZ des valeurs pour le reste
		ok = 0;
		i = 0;
	}
	
	// Desallocation de memoire
	delete_file(file);
	free(trame);
	
	return 0;
}

// Fonction principale
int main(int argc, char ** argv) {
	FD = openPort(); // Ouverture du port et place dans la variable globale
	FICHIER = fopen("valeurs.txt", "rw");

	threadLecture();

  return 0;
}
