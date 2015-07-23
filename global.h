#ifndef GLOBAL_H
#define GLOBAL_H

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Ticker.h>
#include <EEPROM.h>
#include <WiFiUdp.h>

#include "user.h"
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
extern const int adminTimeOut;  // Defines the Time in Seconds, when the Admin-Mode will be diabled : 0 = disable

#define ACCESS_POINT_NAME  "ESP"				
#define ACCESS_POINT_PASSWORD  "12345678" 


#define NTP_SERVERNAME_MAX_SIZE	172
#define DEVICE_NAME_MAX_SIZE		20
#define SSID_MAX_SIZE						32
#define WIFI_PASSWORD_MAX_SIZE		64
#define DEVICE_NAME_MAX_SIZE		20

typedef struct _SystemConfig {
	char header[3];
	char ssid[SSID_MAX_SIZE + 1];  // max size : 32
	char wifiPassword[WIFI_PASSWORD_MAX_SIZE + 1]; // max size : 64
	byte  IP[4];
	byte  netmask[4];
	byte  gateway[4];
	boolean dhcp;
	char ntpServerName[NTP_SERVERNAME_MAX_SIZE + 1]; // max size 172
	long ntpUpdatePeriod;
	long timezone;
	boolean daylight;
	char deviceName[DEVICE_NAME_MAX_SIZE + 1];   // max size 20
	UserConfig userConfig;
} SystemConfig;


//extern strConfig config;
extern SystemConfig config;

// CONFIGURATION
void configureWifi();
void initConfig();
void writeConfig();
void readConfig();
void copyConfigString(char *dest, const char *src, int length);
// !CONFIGURATION

// NTP
//byte packetBuffer[NTP_PACKET_SIZE]; 
void NTPRefresh();
void tickHandler();
// !NTP
#endif