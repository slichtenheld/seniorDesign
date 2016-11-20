/*
 * wifi.h
 *
 * Created: 10/26/2016 11:50:02 AM
 *  Author: Abel Mak
 */ 


#ifndef WIFI_H_
#define WIFI_H_

#include <stdlib.h>
#include "uart.h"

#define GET_DISPENSERS			"!1 1 0 0"
#define GET_DISPENSER_BY_ID		"!1 1 1 "	// attach ID

#define POST_DISPENSER			"!2 1 0 "	// attach payload
#define UPDATE_DISPENSER		"!2 1 1 "   // attach ID first, then payload

#define DELETE_DISPENSER		"!4 1 1 "   // attach ID

#define DISPENSER		1
#define SCHEDULE		2
#define NOTIFICATION	3

unsigned char *request;

char* concat(const char *s1, const char *s2)
{
	char *result = (char*)malloc(strlen(s1)+strlen(s2)+1);//+1 for the zero-terminator
	//in real code you would check for errors in malloc here
	strcpy(result, s1);
	strcat(result, s2);
	return result;
}

void HTTP_GET(int endpoint, int isID, char* ID, char* payLoad) {
	
	switch(endpoint) {
		case DISPENSER:
			if(!isID) {
				transmitUART0string((unsigned char*)GET_DISPENSERS);
			}
			else {
				request = (unsigned char*)concat(GET_DISPENSER_BY_ID, ID);
				//unsigned char k[] = "!1 1 1 57fd89b1189139bc42848d1a";
				transmitUART0string(request);
			}
			break;
	}
	
}

void HTTP_POST(int endpoint, char* ID, char* payLoad) {
	
	switch(endpoint) {
		case DISPENSER:
			request = (unsigned char*)concat(POST_DISPENSER, ID);
			request = (unsigned char*)concat((char*)request, payLoad);
			transmitUART0string(request);
		break;
	}
	
}

void HTTP_DELETE(int endpoint, char* ID) {
	
	switch(endpoint) {
		case DISPENSER:
		request = (unsigned char*)concat(DELETE_DISPENSER, ID);
		transmitUART0string(request);
		break;
	}	
	
}

#endif /* WIFI_H_ */