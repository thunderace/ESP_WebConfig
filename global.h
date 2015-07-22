#ifndef GLOBAL_H
#define GLOBAL_H

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Ticker.h>
#include <EEPROM.h>
#include <WiFiUdp.h>

#include "helpers.h"

extern ESP8266WebServer server;									// The Webserver
extern boolean firstStart;										// On firststart = true, NTP will try to get a valid time
extern int adminTimeOutCounter;									// Counter for Disabling the AdminMode
extern DateTime dateTime;											// Global DateTime structure, will be refreshed every Second
extern WiFiUDP UDPNTPClient;											// NTP Client
extern unsigned long unixTimestamp;								// GLOBALTIME  ( Will be set by NTP)
extern boolean refresh; // For Main Loop, to refresh things like GPIO / WS2812
extern int cNTP_Update;											// Counter for Updating the time via NTP
extern Ticker tkSecond;												// Second - Timer for Updating Datetime Structure
extern boolean adminEnabled;		// Enable Admin Mode for a given Time
extern byte minuteOld;				// Helpvariable for checking, when a new Minute comes up (for Auto Turn On / Off)



typedef struct _strConfig {
	String ssid;
	String password;
	byte  IP[4];
	byte  netmask[4];
	byte  gateway[4];
	boolean dhcp;
	String ntpServerName;
	long ntpUpdatePeriod;
	long timezone;
	boolean daylight;
	String deviceName;
	boolean autoTurnOff;
	boolean autoTurnOn;
	byte turnOffHour;
	byte turnOffMinute;
	byte turnOnHour;
	byte turnOnMinute;
	byte ledR;
	byte ledG;
	byte ledB;
} strConfig;

extern strConfig config;

// CONFIGURATION
void configureWifi();
void writeConfig();
boolean readConfig();
// !CONFIGURATION

// NTP
//byte packetBuffer[NTP_PACKET_SIZE]; 
void NTPRefresh();
void secondTick();
// !NTP
#endif