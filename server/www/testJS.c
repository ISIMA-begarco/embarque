#include <stdlib.h>
#include <stdio.h>

// renvoie une temperature pour la trame a envoyer
float genereTemperature(int min, int max) {
	return (float)((rand()/(float)RAND_MAX) * (max-min) + min);
	}
	
	// Creation des trames de température
	int main() {
		FILE * fichier = NULL;
			while(1) {
					char* trame_locale = (char*)malloc(21 * sizeof(char));
							float temperature = genereTemperature(-20, 40);
									int deb = (int) temperature, fin = abs((int) (temperature *100) % 100);
											time_t rawtime;
													struct tm * temps;
															fichier = fopen("temperature", "w+");
																	time(&rawtime);
																			temps = localtime(&rawtime);
																					snprintf(trame_locale, 21, "%+03d.%02d", deb, fin);
																							fputs(trame_locale, fichier);
																									free(trame_locale);
																											fclose(fichier);
																													sleep(1);
																														}
																															
																																return 0;
																																}
																																
