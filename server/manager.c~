#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>                            
#include <termios.h>
#include <pthread.h>

#include "file.h"

// Structure pour les acquitements
typedef struct bool {
	unsigned char b;
	pthread_mutex_t mutex;
} bool_t;

// Ou on va lire les infos
#define TTY "/dev/ttyAMA0"

// Variables globales
file_t* FILE_TRAME; // File des trames recues correctes
file_t* FILE_CMD; // File des commandes venant
int FD;
FILE * FICHIER;
bool_t* BOOL; // Pour savoir si j'ai bien envoye la commande serveur

bool_t * create_bool() {
	bool_t * retour = (bool_t*)malloc(sizeof(bool_t));
	retour->b = 0;
	pthread_mutex_init(&retour->mutex, NULL);
	return retour;
}

// Ouverture du port
int openPort(void) {
	struct termios options;
	int fd,i;
	fd = open(TTY, O_RDWR);
	if (fd==-1) {
		printf("### Erreur d'ouverture du port \n");
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
		
		printf("### Port ouvert\n");
	}

	return fd;	
}

// Fonction de lecture de trame
int threadLecture() {
printf("1\n");
	int i = 0, ok = 0;
	char c;
	char* trame = (char*)malloc(21 * sizeof(char));
printf("2\n");
	while(1) {
		usleep(100); // Pour la synchro
		// TODO attendre son tour pour aller lire

		ok = 0;
		i = 0;
		while(i < 20 && c!='W') { // la taille max est de 20
			if(read(FD, &c, 1) >= 1) { // Lecture du caractere
				if(ok==1 || c=='X' || c=='Y') { // debut trame TEMP ou ACK
					ok = 1;
					trame[i] = c;
					i++;
				} // if
			} // if read
		} // while
		
printf("Trame recue : %s\n", trame); // affichage de test

		// Verification et mise dans file de la trame
		if(i == 20) { // trame de temperature
			if(trame[19] == 'W') { // fin correcte
				if(trame[0]=='X' && trame[1]=='0' && trame[2]=='1') { // bon code, donc bonne trame
					trame[20] = '\0'; // Ajout du caractere de fin
					while(pthread_mutex_lock( &(FILE_TRAME->mutex) ) != 0) {} // On a attend de bloquer la file
					push_file(FILE_TRAME, trame); // Ajout de la trame dans la file
					pthread_mutex_unlock( &(FILE_TRAME->mutex) ); // Debloque la file
				} // if code
			} // if fin
		} else if (i == 7) { // trame acquitement
			if(trame[6] == 'W') { // fin correcte
				if(trame[0] == 'Y' && trame[1] == '0' && trame[2] == '0') { // bon code de trame
					while(pthread_mutex_lock( &(BOOL->mutex) ) != 0) {} // On attend de bloquer le booleen
					BOOL->b = 1; // indique qu'on a recu un ACK
					pthread_mutex_unlock( &(BOOL->mutex) ); // Debloque le booleen
				} // if
			} // if
		} // if verif
	} // while(1)
	
	// Desallocation de memoire
	delete_file(FILE_TRAME);
	free(trame);
	
	return 0;
}

// Ecriture dans le fichier et envoi des acquitements
void threadEcriture() {
	// TODO gestion du mutex de file et envoi des acquitements
	// TODO get les commandes du serveur
	// TODO gestion des trames : ecriture temperature et heure dans fichier qui sera lu en js
	// TODO gesytion du mutex du booleen

	while(1) {
		
	} // while true

}

// Fonction principale
int main(int argc, char ** argv) {
	FD = openPort(); // Ouverture du port en global
	FILE_TRAME = create_file();
	FILE_CMD = create_file();
	FICHIER = fopen("valeurs.txt", "rw");
	BOOL = create_bool();
	
	threadLecture();

  return 0;
}
