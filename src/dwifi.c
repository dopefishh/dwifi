#include <stdio.h>		//File io
#include <stdlib.h>		//Printing to the screen
#include <string.h>		//Working with and printing cstrings
#include "./wifinet.h"	//The wifinet tools
#include "./creator.h"	//Network creator

/*Prototyping the functions */
int read_line(FILE* f, char* l);
int yn_dialog(char* l);
void connect(struct wifinet* network);
void parse_args(int argc, char** argv);

/* The global variables for the software */
char* silent = "";
struct wifinet final; int finale = 0;
char* logfile = ".essid";
char* interface = "wlan0";
char* driver = "wext";
char* configpath = "/etc/wifi";

/* Reading one line from a file */
int read_line(FILE* f, char* l)
{
	char c;
	int i = 0;
	while((c = fgetc(f)) != EOF && c != '\n')
		if(c>=32)
			l[i++] = c;
	l[i+1] = '\0';
	return c!=EOF;	
}

/* Simple yes no dialog with the string as argument */
int yn_dialog(char* l)
{
	fflush(stdin);
	printf("%s [y]es/[n]o? ", l);
	char c;
	while((c = getchar())!='y' && c!='n' && c!='Y' && c!='N')
		fflush(stdin);
	return c=='y' || c=='Y';
}

/* The connect routine */
void connect(struct wifinet* network)
{	
	if(strlen(silent)==0) printf("trying to connect to %s\n", network->ssid);
	char conf[4048];
	sprintf(conf, "%s/%s.conf", configpath, network->ssid); 
	FILE* f;	
	if(f = fopen(conf, "r"))
	{
		fclose(f);
	}
	else if(network->encr!=no)
	{
		printf("Config file not found or in use\nI'm looking for: %s\n", conf);
		if(yn_dialog("Want to create the config for the network?"))
		{
			create_config(network, configpath);
			connect(network);
		}
		else
		{
			exit(EXIT_FAILURE);
		}
	}
	char path[4048];
	system("killall -q wpa_supplicant dhclient");
	sprintf(path, "ifconfig %s down%s", interface, silent);
	system(path);
	sprintf(path, "ifconfig %s up%s", interface, silent);
	system(path);
	sprintf(path, "iwconfig %s essid \"%s\"%s", interface, network->ssid, silent);
	system(path);
	switch(network->encr)
	{
		case no:
			sprintf(path, "echo Warning connecting to a unprotected network!");
			break;
		case wpa1:
		case wpa2:
			sprintf(path, "wpa_supplicant -i%s -c%s -D%s -B%s", interface, conf, driver, silent);
			break;			
	} 
	system(path);
	sprintf(path, "dhclient %s%s", interface, silent);
	system(path);
	sprintf(path, "ifconfig %s%s", interface, silent);
	system(path);
}

/* Command line argument parsing */
void parse_args(int argc, char** argv)
{
	int i;
	for(i = 1; i<argc; i++)
	{
		if(strcmp(argv[i], "-h") == 0)
		{
			printf("Usage:\n\t%s [options]\n", argv[0]);
			printf("Options:\n");
			printf("\t-h\tthis help\n");
			printf("\t-s \tto make the program silent(pipe to /dev/null\n");			
			printf("\t-i x\twhere x is the interface(default: wlan0)\n");
			printf("\t-d x\twhere x is the driver(default: wext)\n");
			printf("\t-c x\twhere x is the config root path(default: /etc/wifi)\n");
			printf("\t-ssid x y\twhere x is the essid and y the encryption type(open/wpa2)\n");
			printf("\nThe config file has to match the essid for example if you want to connect to ");
			printf("the \"eduroam\" network you have to have a config file /etc/wifi/eduroam.conf\n");
			printf("The config file for a simple WPA2 network can be easily created by wpa_passphrase ");
			printf("for example:  \n");
			printf("wpa_passphrase essid password >> /etc/wifi/eduroam.conf\"\n");
			exit(EXIT_SUCCESS);
		}
		if(strcmp(argv[i], "-i") == 0 && argc>i)
		{
			interface = (char*)malloc(50*sizeof(char));
			strcpy(interface, argv[i+1]);
			if(strlen(silent)>0) printf("Using manual interface: %s\n", interface);
		}
		if(strcmp(argv[i], "-d") == 0 && argc>i)
		{
			driver = (char*)malloc(50*sizeof(char));
			strcpy(driver, argv[i+1]);
			if(strlen(silent)>0) printf("Using manual driver: %s\n", driver);
		}
		if(strcmp(argv[i], "-c") == 0 && argc>i)
		{
			configpath = (char*)malloc(50*sizeof(char));
			strcpy(configpath, argv[i+1]);
			if(strlen(silent)>0) printf("Using manual configpath: %s\n", configpath);
		}
		if(strcmp(argv[i], "-ssid") == 0)
		{
			if(argc>=i+2)
			{
				printf("i: %i\nargc: %i\n", i, argc);
				strcpy(final.ssid, argv[i+1]);
				if(strcmp(argv[i+2], "wpa")==0)	final.encr = wpa2;
				else final.encr = no;				
			}
			else
			{
				printf("No encryption type given assuming an open network\n");
				final.encr = no;
			}
			if(strlen(silent)>0) printf("Using manual ssid: %s\n", argv[i+1]);
			finale = 1;
		}
		if(strcmp(argv[i], "-s") == 0)
		{
			silent = (char*)malloc(15*sizeof(char));
			strcpy(silent, " >> /dev/null");
		}
	}
}

/* Main function */
int main(int argc, char* argv[]) 
{
	parse_args(argc, argv);
	if(!finale)
	{
		/* Put the interface up and scan for wireless networks */
		char path[100];
		sprintf(path, "ifconfig %s up%s", interface, silent);
		system(path);
		sprintf(path, "iwlist %s scan > %s", interface, logfile);
		system(path);

		int nlines;
		FILE* f = fopen(logfile, "r");
		struct wifinet* networks = get_wifinets(&nlines, f);
		remove(logfile);	
	
		printf("Done scanning: %i networks found\n", nlines);
		if(nlines==0)
			exit(EXIT_SUCCESS);
		
		wifinets_print(networks, nlines);
	
		char c;
		printf("Please enter character corresponding to the network(X to quit): ");
		while(!(((c = getchar())>=48 && c<48+(nlines>10?10:nlines) ) || (nlines>10 && c>=97 && c<=97+nlines-10)))
			if(c=='X' && yn_dialog("Are you sure? ")) 
				exit(EXIT_SUCCESS);
		
		connect(&networks[c>=48 && c<=57 ? c-48 : c-97+10]);
	}
	else
	{
		connect(&final);
	}
	exit(EXIT_SUCCESS);
}
