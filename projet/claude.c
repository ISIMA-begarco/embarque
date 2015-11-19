/*****************************************************************
 *                                                               *
 * FILENAME : claude.c                                           *
 * DESCRIPTION : Main program launching threads and managing the *
 *              the pc side                                      *
 *                                                               *
 * AUTHORS : Benjamin BARBESANGE                                 *
 *           Benoit GARCON                                       *
 *                                                               *
 *                                                               *
 *****************************************************************/

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

#include "file.h"

// Constantes
#define TTYUSB "/dev/ttyUSB0"

/*****************************************************************
						TODO list

	- liste de trames 			!DONE!
	- gestion acquittement
	- threads²

******************************************************************/

typedef struct bool {
    unsigned char   b;
    pthread_mutex_t mutex;
} bool_t;

// Variables globales
int fd;
file_t * GL_trames;
file_t * GL_recues;
bool_t * BOOL_ACK_RECEPTION;
bool_t * BOOL_ACK_ENVOI;
bool_t * BOOL_RUN;
int FREQ;
char ACK_STR[] = "Y00ACKW";

// Ouverture du port pour communication RS232
int openPort(void) {
	struct termios options;
	int fd;
	fd = open(TTYUSB, O_RDWR); // open device for read&write

	if (fd==-1)
		printf("Erreur ouverture port !!!\n");
	else {
		fcntl(fd,F_SETFL,0);
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
		options.c_iflag &= ~(IXON | IXOFF | IXANY);
		options.c_oflag &= ~OPOST; // raw output
		options.c_lflag &= ~(ICANON | ECHO | ECHONL|IEXTEN | ISIG);
		options.c_cc[VMIN]= 1;
		options.c_cc[VTIME]= 4;
		tcflush(fd,TCIOFLUSH); // flushIO buffer
		tcsetattr(fd, TCSANOW,&options); // set new configure immediately
		usleep(10000);
	}

	return fd;
}

// Thread d'ecriture de trames
void *threadEnvoi(void *) {
	int i, nbEssais, msgEnvoye;
	char * trame;

  	while(BOOL_RUN->b) {

  		/// envoi d'ACK
  		if(BOOL_ACK_ENVOI->b) {
		  	while(pthread_mutex_lock(&(BOOL_ACK_ENVOI->mutex))) {		// on attend l'acces a la zone partagee
		  		usleep(1);
		  	}
	  	  	for(i = 0 ; i < strlen(ACK_STR) ; ++i) {
				write(fd, &(ACK_STR[i]),1);
	            printf("%c", ACK_STR[i]);
				usleep(0);
			}
			tcflush(fd,TCIOFLUSH);
	        printf("\tEnvoi fini\n");
			BOOL_ACK_ENVOI->b = 0;
			pthread_mutex_unlock(&(BOOL_ACK_ENVOI->mutex));
  		} else { // envoi des temperatures
			msgEnvoye=0, nbEssais=3;
			while(!msgEnvoye && nbEssais) {
				while(pthread_mutex_lock(&(GL_trames->mutex))) {	// on attend l'acces a la zone partagee
					usleep(1);
				}
				if(GL_trames->size) {
					trame = top_file(GL_trames);
					for(i = 0 ; i < strlen(trame) ; ++i) {
						write(fd, &(trame[i]),1);
						printf("%c", trame[i]);
						usleep(0);			// TODO verifier le temps de pause
					}
					tcflush(fd,TCIOFLUSH);
					printf("\tEnvoi fini\n");
					free(trame);

                    // attente de l'ack
                    int attente = 100;
                    while(BOOL_ACK_RECEPTION->b == 0 && attente != 0) {
                        usleep(1);
                        attente--;
                    }
                    if(attente==0) {
                        nbEssais--;
                    } else {
                        pop_file(GL_trames);
                        msgEnvoye = 1;
                        while(pthread_mutex_lock(&(BOOL_ACK_RECEPTION->mutex))) {		// on attend l'acces a la zone partagee
                            usleep(1);
                        }
                        BOOL_ACK_RECEPTION->b = 0;
                        pthread_mutex_unlock(&(BOOL_ACK_RECEPTION->mutex));
                    }
                    if(!nbEssais) {
                        pop_file(GL_trames);
                    }
				}
                pthread_mutex_unlock(&(GL_trames->mutex));
			}
		}
	}

	return 0;
}

// renvoie une temperature pour la trame a envoyer
float genereTemperature(int min, int max) {
	return (float)((rand()/(float)RAND_MAX) * (max-min) + min);
}

// Creation des trames de température
char* trameTemperature() {	/* TODO Desallouer la trame apres envoi */
	char* trame = (char*)malloc(20 * sizeof(char));
	float temperature = genereTemperature(-20, 40);
	int deb = (int) temperature, fin = abs((int) (temperature *100) % 100);
	time_t rawtime;
	struct tm * temps;
	time(&rawtime);
	temps = localtime(&rawtime);
	sprintf(trame, "X01%+03d.%02d%02d%02d%02d%02d%02dW", deb, fin, temps->tm_hour, temps->tm_min, temps->tm_mday, temps->tm_mon+1, temps->tm_year%100);
	return trame;
}

// thread de capture de la temperature
void* threadThermometre(void *) {
    while(BOOL_RUN->b) {
        while(pthread_mutex_lock(&(GL_trames->mutex))) {	// on attend l'acces a la zone partagee
            usleep(1);
        }
		char* trame = trameTemperature();
		push_file(GL_trames, trame);
		free(trame);
        pthread_mutex_unlock(&(GL_trames->mutex));
		sleep(FREQ);
	}
}


/******************************************************************************/

int main(int argc, char* argv[]) {
	GL_trames = create_file();
	GL_recues = create_file();
	BOOL_RUN = (bool_t*)malloc(sizeof(bool_t));
	BOOL_RUN->b = 1;
	pthread_mutex_init(&(BOOL_RUN->mutex), NULL);
	BOOL_ACK_RECEPTION = (bool_t*)malloc(sizeof(bool_t));
	BOOL_ACK_RECEPTION->b = 0;
	pthread_mutex_init(&(BOOL_ACK_RECEPTION->mutex), NULL);
	BOOL_ACK_ENVOI = (bool_t*)malloc(sizeof(bool_t));
	BOOL_ACK_ENVOI->b = 0;
	pthread_mutex_init(&(BOOL_ACK_ENVOI->mutex), NULL);
	FREQ = 1;
	pthread_mutex_init(&(BOOL_RUN->mutex), NULL);

    // threads
	pthread_t envoi, reception, thermometre;

	srand(time(NULL)); // Initialise le generateur de temperature
	printf("Ouverture du port ...\n");
	fd = openPort();

    // lancement des threads
	pthread_create(&envoi, NULL, threadEnvoi, NULL);
	pthread_create(&reception, NULL, threadReception, NULL);
	pthread_create(&thermometre, NULL, threadThermometre, NULL);

    // traitement des donnees recues
	while(BOOL_RUN->b) {
		while(pthread_mutex_lock(&(GL_recues->mutex))) {	// on attend l'acces a la zone partagee
            usleep(1);
        }
        if(GL_recues->size) {
            char * ordre = top_file(GL_recues);
            if(ordre[0]=='Z'&&ordre[1]=='0'&&ordre[2]=='2') {   // chgt de frequence
                int j = 0;
                FREQ = 0;
                for(j = 3 ; j < 9 ; j++) {
                    FREQ *= 10;
                    FREQ += (ordre[j]-'0');
                }
                printf("Changement de periode d'echantillonage a %d secondes.\n", FREQ);
            } else if(ordre[0]=='Q'&&ordre[1]=='0'&&ordre[2]=='3') {    // on a un stop
                while(pthread_mutex_lock(&(BOOL_RUN->mutex))) {	// on attend l'acces a la zone partagee
                    usleep(1);
                }
                BOOL_RUN->b = 0;
                pthread_mutex_unlock(&(BOOL_RUN->mutex));
                printf("Arret demande ...\n")
            }
            free(ordre);
            pop_file(GL_recues);
            pthread_mutex_unlock(&(GL_recues->mutex));
            usleep(1);
        }
	}
    printf("Arret en cours ....\n");
    // arret des threads
	pthread_join(envoi, NULL);
	pthread_join(reception, NULL);
	pthread_join(thermometre, NULL);

    // liberation de memoire
	free(BOOL_RUN);
	free(BOOL_ACK_RECEPTION);
	free(BOOL_ACK_ENVOI);
    // fermeture du port
	close(fd);
	// destruction des files
	delete_file(GL_recues);
	delete_file(GL_trames);

	return EXIT_SUCCESS;
}

