#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>                            
#include <termios.h>                             

#define ATTENTE for(i=0;i<100000;i++)

int open_port(void)
{
  struct termios options;
  int fd,i;
//  fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY | O_NDELAY);
  fd = open("/dev/ttyAMA0", O_RDWR); // open device for read&write
  if (fd==-1)
    printf("Erreur ouverture port !!!\n");
  else
  {
    fcntl(fd,F_SETFL,0);
    fcntl(fd, F_SETFL, FNDELAY); //ne pas bloquer sur le read
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

int main(int argc, char ** argv)
{
  int fd, i, j;char x = 0;
  unsigned char car;
  unsigned char TxChar[]="BONJOUR BONJOUR ZZF1\n";
  char RxChar[]="ABCDEFGHIJKLMNOPQRST\n";
  printf("Start open port\n");
  fd=open_port();

  while(1) {
	car=getchar();
//	scanf("%c", &x);
 	write(fd, &car, 1);
	ATTENTE;
  }

  printf("Attente le caractère 9\n");
  
  for (j=0; j<5; j++) {
	ATTENTE;
	printf("Bonjour\n");
	ATTENTE;
        for (i=0; i<21; i++)
	{
	 write(fd, &TxChar[i],1);
	 usleep(20000);
	 }
  }	 
 	if(read(fd, &car,1)>=1){
	printf("Caractere recu %c", car);
	printf("%c", '\n');
	}
//	RxChar[i+1]='\n';
//	printf("%s",&RxChar[0]);
//	 fflush((FILE*)fd);
    	tcflush(fd,TCIOFLUSH);

	sleep(1);

	//if (read(fd,&c,1)>=1) 
	//if ((c&0x7F)==0x39)
	//	break;
  close(fd);
  return 0;
}
