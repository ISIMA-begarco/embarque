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
#define FICHIER_VALEURS "valeurs.txt"

// Structure pour les acquitements
typedef struct bool {
	unsigned char b;
	pthread_mutex_t mutex;
} bool_t;

// Variables globales
file_t* FILE_TRAME; // File des trames recues correctes
file_t* FILE_CMD; // File des commandes venant
int FD;
FILE * FICHIER;
bool_t* BOOL_ACK; // Pour savoir si j'ai bien envoye la commande serveur
bool_t* BOOL_SEND_ACK; // Pour dire si je dois envoyer des ACK de trame ou pas
unsigned char BOOL_RUN; // Pour savoir s'il faut continuer les threads
char* ACK_STR; // Trame d'acquitement

bool_t * create_bool() {
	bool_t * retour = (bool_t*)malloc(sizeof(bool_t));
	retour->b = 0;
	pthread_mutex_init(&retour->mutex, NULL);
	return retour;
}

// Ouverture du port
int openPort(void) {
	struct termios options;
	int fd;
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
	int i = 0, ok = 0;
	char c = 0;
	char* trame = (char*)malloc(21 * sizeof(char));

	while(BOOL_RUN == 1) { // TODO modifier dans le prog pour stopper
		//usleep(100); // Pour la synchro
		// TODO attendre son tour pour aller lire

		// Lecture trame
		ok = 0;
		i = 0;
		c = 0;
		while(i < 20 && c!='W') { // la taille max est de 20
			if(read(FD, &c, 1) >= 1) { // Lecture du caractere
				if(ok==1 || c=='X' || c=='Y') { // debut trame TEMP ou ACK
					ok = 1;
					trame[i] = c;
					i++;
				} // if
			} // if read
		} // while

		/// Verification et mise en file
		if(i == 20) { // trame de temperature
			if(trame[19] == 'W') { // fin correcte
				if(trame[0]=='X' && trame[1]=='0' && trame[2]=='1') { // bon code, donc bonne trame
printf("Trame recue : %s\n", trame); // affichage de test
					trame[20] = '\0'; // Ajout du caractere de fin
					while(pthread_mutex_lock( &(FILE_TRAME->mutex) ) != 0) {usleep(1);} // On a attend de bloquer la file
					push_file(FILE_TRAME, trame); // Ajout de la trame dans la file
					pthread_mutex_unlock( &(FILE_TRAME->mutex) ); // Debloque la file
printf("*** Trame ajoutee dans la file (%d) \n", FILE_TRAME->size);
					
					// Dit qu'il faut envoyer un ACK
					while( pthread_mutex_lock( &(BOOL_SEND_ACK->mutex) ) != 0 ) {usleep(1);}
					BOOL_SEND_ACK->b = 1;
					pthread_mutex_unlock( &(BOOL_SEND_ACK->mutex) );

				} // if code
			} // if fin
		} else if (i == 7) { // trame acquitement
			if(trame[6] == 'W') { // fin correcte
				if(trame[0] == 'Y' && trame[1] == '0' && trame[2] == '0') { // bon code de trame
					while(pthread_mutex_lock( &(BOOL_ACK->mutex) ) != 0) {usleep(1);} // On attend de bloquer le booleen
					BOOL_ACK->b = 1; // indique qu'on a recu un ACK
					pthread_mutex_unlock( &(BOOL_ACK->mutex) ); // Debloque le booleen
				} // if
			} // if
		} // if verif
	} // while(1)
	
	// Desallocation de memoire
	free(trame);
	
	return 0;
}

// Ecriture dans le fichier et envoi des acquitements
int threadEcriture() {
    int essai, i;
    char *trame, *cmd;

	// TODO gerer le BOOL_RUN

	// TODO gestion du mutex de file et envoi des acquitements
	// TODO get les commandes du serveur
	// TODO gestion des trames : ecriture temperature et heure dans fichier qui sera lu en js
	// TODO gesytion du mutex du booleen

	while(BOOL_RUN == 1) {
		// Envoi des ACK pour les trames de temp
		if(BOOL_SEND_ACK-> b == 1) { // Je dois envoyer un ACK
			while( pthread_mutex_lock( &(BOOL_SEND_ACK->mutex) ) != 0) {usleep(1);}
			// TODO envoyer l'ACK
			BOOL_SEND_ACK->b = 0; // Je ne dois pas en envoyer d'autre pour le moment
printf("*** ACK envoye\n");
			pthread_mutex_unlock( &(BOOL_SEND_ACK->mutex) );
		}

	    // Envoi des trames de commandes
	    if(FILE_CMD->size > 0) { // On doit envoyer une commande
    	    while( pthread_mutex_lock( &(FILE_CMD->mutex) ) != 0) {usleep(1);}
    	    cmd = top_file(FILE_CMD); // Recupere la trame
	        essai = 0;
	        while(BOOL_ACK->b != 1 && essai < 3) { // Pas encore l'ACK
				i = 0;
	            while(cmd[i] != 'W' && i<10) { // TODO write la commande
	                write(FD, &cmd[i], 1); // Ecrit dans pour le pc
					i++;
	            }
printf("Envoi trame cmd, essai %d\n", essai);
	            essai++;
	            usleep(100); // Sert de timeout TODO peut etre enleve
	        } // while ACK
	        pop_file(FILE_CMD); // Enleve la trame pour aller a la suivante
	        pthread_mutex_unlock( &(FILE_CMD->mutex) );
	        free(cmd); // libere la trame
	    } // if cmd > 0
	    
        //Ecriture trames dans le fichier
	    if(FILE_TRAME->size > 0) { // On a des trames
        	while( pthread_mutex_lock( &(FILE_TRAME->mutex) ) != 0) {usleep(1);}
			trame = top_file(FILE_TRAME); // Recup la trame
			// TODO ecrire la trame dans le fichier
			
			pop_file(FILE_TRAME); // Sort de la file
printf("*** Trame %s dans le fichier et sortie\n", trame);
	    	pthread_mutex_unlock( &(FILE_TRAME->mutex) );
			free(trame); // libere la trame pour avoir les suivantes
	    } // if trame > 0

	} // while true
    
    // Liberation memoire
    free(trame);
	free(cmd);
    
    return 0;
}

// Fonction principale
int main() {
	// Initialisations des variables globales
	FD = openPort(); // Ouverture du port en global
	FILE_TRAME = create_file();
	FILE_CMD = create_file();
	FICHIER = fopen(FICHIER_VALEURS, "w");
	BOOL_ACK = create_bool();
	BOOL_SEND_ACK = create_bool();
    BOOL_RUN = 1;
	ACK_STR = "Y00ACKW";

	// Initialisation des threads
	pthread_t lecture, ecriture;

	pthread_create(&lecture, NULL, threadLecture, NULL);
	pthread_create(&ecriture, NULL, threadEcriture, NULL);

	// TODO lancer des threads, TODO gerer l'arret du capteur
	// Lancement des threads
	threadLecture();
	///threadEcriture();
	
	// Liberation memoire
	delete_file(FILE_TRAME);
	delete_file(FILE_CMD);
	fclose(FICHIER);

  return 0;
}
