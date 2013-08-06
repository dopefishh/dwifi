#ifndef WIFINET_H
#define WIFINET_H
#include <stdio.h>

enum encr{no, wpa1, wpa2};

struct wifinet{
	char id;
	char ssid[50];
	char hwaddr[17];
	float str;
	enum encr encr;
};

void wifinet_print(struct wifinet* w);
struct wifinet* get_wifinets(int* nets, FILE* f);

#endif
