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

// Ouverture du port pour communication RS232
int openPort(void) {
	struct termios options;
	int fd,i;
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
void *threadEnvoi(void * p_trames) {
	int fd, i;
	char * trame;
	printf("Ouverture du port ...\n");
	fd = openPort();
  	
  	while(1) {
	  	while(pthread_mutex_lock(&(((file_t*)p_trames)->mutex))) {		// on attend l'acces a la zone partagee
	  		usleep(1);
	  	}
	  	if(((file_t*)p_trames)->size) {
	  		trame = top_file((file_t*)p_trames);
	  	  	for(i = 0 ; i < strlen(trame) ; ++i) {
				write(fd, &(trame[i]),1);
				usleep(10000);			// TODO verifier le temps de pause
			}
			tcflush(fd,TCIOFLUSH);
			pop_file((file_t*)p_trames);
			free(trame);
		}
		pthread_mutex_unlock(&(((file_t*)p_trames)->mutex));
	}

	//sleep(1);
	
	// fermeture du port
	close(fd);
	
	return 0;
}

// Thread de lecture des trames
int threadLecture () { // TODO a redefinir comme il faut et ajouter les bons parametres
	int fd, i, j;
	unsigned char car='a';

	printf("Tentative d'ouverture du port\n");
	fd=openPort();

/*	read*/
	usleep(20000);
		 
	if(read(fd, &car,1)>=1){
	printf("Caractere recu %c", car);
	printf("%c", '\n');
	}
	
	tcflush(fd,TCIOFLUSH);

	sleep(1);

	close(fd);
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

/******************************************************************************/

int main(int argc, char* argv[]) {
	file_t * trames = create_file();
	pthread_t envoi;
	srand(time(NULL)); // Initialise le generateur de temperature
	
	pthread_create(&envoi, NULL, threadEnvoi, (void *) &trames);
	
	while(1) {
		char* trame = trameTemperature();
	
		printf("%s\n", trame);
		push_file(trames, trame);
		free(trame);
	}
	return EXIT_SUCCESS;
}
