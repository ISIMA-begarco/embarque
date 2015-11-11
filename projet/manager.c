#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>                            
#include <termios.h>                             

// Ou on va lire les infos
#define TTY "/dev/ttyAMA0"

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
	int fd, i = 0, ok = 0;
	char c;
	// Pour stocker la récupération
	char* trame = (char*)malloc(21 * sizeof(char));
	
	fd = openPort();

	// TODO attendre son tour pour aller lire

	do {
	// TODO attendre un X pour commencer a lire
		read(fd, &c, 1); // Lecture du caractere
//		usleep(20000);
		trame[i] = c;
		i++; // carctere suivant
	} while(i < 20 && c!='W');

	// TODO verifier la trame
	// TODO ACK si le trame est bonne
	
	trame[20] = '\0';
	
	printf("Trame : %s\n", trame); // affichage de test
	
	free(trame);
	
	return 0;
}

// Fonction principale
int main(int argc, char ** argv) {
/*
  int fd, i, j;
  unsigned char car='a';
  unsigned char TxChar[]="BONJOUR BONJOUR ZZF1\n";
  char RxChar[]="ABCDEFGHIJKLMNOPQRST\n";
  printf("Start open port\n");
*/

	while(1) {
		printf("----- Nouvelle lecture -----\n");
		threadLecture();
	}

/*
	fd=openPort();

	

  
  while(1) {
	if(read(fd, &car,1)>=1)
	 	printf("%c", car); 
  }
  
  for (j=0; j<5; j++) {
	 for (i=0; i<21; i++) {
 		if(read(fd, &car,1)>=1){
//		read(fd, &car,1);
	 	RxChar[i]=car;
	 	printf("%c\n", car); 
	 	}
	}
	printf("\n Receive message \n");
	printf("%s\n",RxChar);
    	tcflush(fd,TCIOFLUSH);
  }*/
  return 0;
}
