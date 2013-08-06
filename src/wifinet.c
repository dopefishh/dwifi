#include "./wifinet.h" //The file it extends
#include <stdio.h>	  //File io
#include <string.h>	  //Workinng and printing cstrings
#include <stdlib.h>	  //Standard lib

char wpa1id[] = "WPA Version 1";
char wpa2id[] = "IEEE 802.11i/WPA2 Version 1";

/* Prints a array of wifinetworks */
void wifinets_print(struct wifinet* w, int n)
{
	printf("# strength\tencryption\tssid (bssid)\n");
	int i;
	for(i = 0; i<n; i++)
		wifinet_print(&w[i]);
}

/* Prints a single wifinetwork */
void wifinet_print(struct wifinet* w)
{
	printf("%c. %.1fdbm\t%s\t\t%s (%s)\n", w->str, w->id, w->encr==no ? "open" : w->encr==wpa1 ? "wpa1" : "wpa2", w->ssid, w->hwaddr);
}

/* Reads a wifinet from a file */
struct wifinet* get_wifinets(int* nets, FILE* f)
{
	char* l = malloc(4048*sizeof(char));
	struct wifinet* networks = malloc(36*sizeof(struct wifinet));
	(*nets) = -1;
	while(read_line(f, l))
	{	
		char prev,  cur;
		int n = 0;
		while((cur = l[n++])==' ');		/* Skip all the spaces */
		prev = cur;
		cur = l[n];
		if(prev=='C' && cur=='e')			/* Read hardware adress */
		{
			(*nets)++;
			networks[(*nets)].id = (*nets)<10 ? (*nets)+48 : (*nets)-10+97;
			memcpy(networks[(*nets)].hwaddr, &l[n+18], 17);
		}
		if(prev=='E' && cur=='S') 			/* Read SSID */
		{
			int ssidlength = 0;
			n += 6;
			while(l[n+ssidlength]!='\0')
				ssidlength++;
			memcpy(networks[(*nets)].ssid, &l[n], ssidlength-2);
		}
		if(prev=='Q' && cur=='u') 			/* Read signal strength */
		{
			char b[3];
			memcpy(b, &l[n+27], 3);
			networks[(*nets)].str = atof(b);			
		}
		if(prev=='I' && cur=='E')			/* Read encryption kind */
		{
			char* t = strstr(&l[n+3], wpa1id);
			if(t!=NULL)
			{
				networks[(*nets)].encr = networks[(*nets)].encr==wpa2 ? wpa2 : wpa1;
			}
			if((t = strstr(&l[n+3], wpa2id))!=NULL)
			{
				networks[(*nets)].encr = wpa2;
			}		
		}
	}
	fclose(f);
	return networks;
}
