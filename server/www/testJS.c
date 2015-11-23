#include <stdlib.h>
#include <stdio.h>

float prevTemp = 0;

// renvoie une temperature pour la trame a envoyer
float genereTemperature(int min, int max) {
	(float)((rand()/(float)RAND_MAX) * (max-min) + min);
	prevTemp = prevTemp + (rand()/(float)RAND_MAX) * 5 - 2.5;
	prevTemp = (prevTemp > 40) ? 40 : (prevTemp<-20) ? -20 : prevTemp;
	return prevTemp;
}

// Creation des trames de température
int main() {
	FILE * fichier = NULL;
	FILE * history = NULL;
	while(1) {
		char* trame_locale = (char*)malloc(21 * sizeof(char));
		float temperature = genereTemperature(-20, 40);
		int deb = (int) temperature, fin = abs((int) (temperature *100) % 100);
		time_t rawtime;
		struct tm * temps;
//		fichier = fopen("temperature", "w+");
		time(&rawtime);
		temps = localtime(&rawtime);
		snprintf(trame_locale, 21, "%+03d.%02d;HHMM;JJMMAA", deb, fin);
//		fputs(trame_locale, fichier);

		/********************************************/
		
//		system("cp history tmp; tail -n 999 tmp > history");
		history = fopen("history", "a");
//		fputs(trame_locale, history);
		fprintf(history,"%s\n", trame_locale);
		fclose(history);
		
		/********************************************/
		free(trame_locale);
//		fclose(fichier);
		usleep(100000);
	}
	return 0;
}
