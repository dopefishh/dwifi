#include "./creator.h"	//File it extends
#include <stdio.h>		//File io
#include <stdlib.h>		//Standard library
#include <string.h>		//Working and printing cstrings

/* Reads a single word from stdin */
char* read_word()
{
	char* wpa = (char*)malloc(4048*sizeof(char));
	int i = 0;
	char c;
	fflush(stdin);
	while((c = getchar()) != '\n') wpa[i++] = c;
	wpa[i] = '\0';
	return wpa;
}

/* Reads a wpa key from stdin */
char* read_wpa()
{
	printf("Please enter the passphrase: ");
	char* wpa;
	while(strlen(wpa = read_word())<8)
		printf("WPA code should be a minimal of 8 characters\nPlease try again: ");
	return wpa;
}

/* Creates a config for the given network */
void create_config(struct wifinet* network, char* configpath)
{
	char command[4048];
	printf("CONFIG CREATION IS STILL EXPERIMENTAL!!!\n");
	wifinet_print(network);
	switch(network->encr)
	{
		case wpa1:
		case wpa2:
			sprintf(command, "wpa_passphrase %s %s > %s/%s.conf", network->ssid, read_wpa(), configpath, network->ssid);
			printf("%s\n", command);
			break;	
	}	
	system(command);
	printf("Config written to %s/%s.conf\n", configpath, network->ssid);
}
