#include "global.h"

ESP8266WebServer server(80);									// The Webserver
boolean firstStart = true;										// On firststart = true, NTP will try to get a valid time
int adminTimeOutCounter = 0;									// Counter for Disabling the AdminMode
DateTime dateTime;											// Global DateTime structure, will be refreshed every Second
WiFiUDP UDPNTPClient;											// NTP Client
unsigned long unixTimestamp = 0;								// GLOBALTIME  ( Will be set by NTP)
boolean refresh = false; // For Main Loop, to refresh things like GPIO / WS2812
int cNTP_Update = 0;											// Counter for Updating the time via NTP
Ticker tkSecond;												// Second - Timer for Updating Datetime Structure
boolean adminEnabled = false;		// Enable Admin Mode for a given Time
const int adminTimeOut  = 60;  // Defines the Time in Seconds, when the Admin-Mode will be diabled : 0 = disable


//strConfig config;
SystemConfig config;

void configureWifi() {
	Serial.println("Configuring Wifi");
	WiFi.begin (config.ssid, config.wifiPassword);
	if (!config.dhcp) {
		WiFi.config(IPAddress(config.IP[0],config.IP[1],config.IP[2],config.IP[3]),  
								IPAddress(config.gateway[0],config.gateway[1],config.gateway[2],config.gateway[3]), 
								IPAddress(config.netmask[0],config.netmask[1],config.netmask[2],config.netmask[3]));
	}
}


void copyConfigString(char *dest, const char *src, int length) {
	strncpy(dest, src, length);
	dest[length] = '\0';
}


void initConfig() {
	Serial.println("Default config");
	copyConfigString(config.ssid, "SSID", SSID_MAX_SIZE);
	copyConfigString(config.wifiPassword, "WIFIPASSWORD", WIFI_PASSWORD_MAX_SIZE);
	config.dhcp = true;
	config.IP[0] = 192;config.IP[1] = 168;config.IP[2] = 1;config.IP[3] = 100;
	config.netmask[0] = 255;config.netmask[1] = 255;config.netmask[2] = 255;config.netmask[3] = 0;
	config.gateway[0] = 192;config.gateway[1] = 168;config.gateway[2] = 1;config.gateway[3] = 1;
	copyConfigString(config.ntpServerName, "0.fr.pool.ntp.org", NTP_SERVERNAME_MAX_SIZE);
	config.ntpUpdatePeriod =  0;
	config.timezone = -10;
	config.daylight = true;
	copyConfigString(config.deviceName, "Unnamed", DEVICE_NAME_MAX_SIZE);
	writeConfig();
	adminEnabled = true;
}

void writeConfig() {
	Serial.println("Writing System Config");
	config.header[0] = 'D';
	config.header[1] = 'F';
	config.header[2] = 'G';
	
	EEPROMWritelong(0, sizeof(config));

	char *ptr = (char *)&config;
	for (int t=0; t<sizeof(config); t++) {
		EEPROM.write(t+4, ptr[t]);
	}
	EEPROM.commit();
}

void readConfig() {
	Serial.println("Reading System Configuration");
	// read size 
	if (EEPROMReadlong(0) != sizeof(SystemConfig)) {
			Serial.println("System Configuration Version Mismatch : ignore");
			initConfig();
			return;
	}
	if (EEPROM.read(4) == 'D' && EEPROM.read(5) == 'F' && EEPROM.read(6) == 'G' ) {
		char *ptr = (char *)&config;
		for (int t=0; t<sizeof(SystemConfig); t++) {
			ptr[t] = EEPROM.read(t+4);
		}
		return;
	}
	Serial.println("No Configuration Found!");
	initConfig();
	return;
}

#define NTP_PACKET_SIZE 48 
byte packetBuffer[NTP_PACKET_SIZE]; 
void NTPRefresh() {
	if (WiFi.status() == WL_CONNECTED) {
		IPAddress timeServerIP; 
		WiFi.hostByName(config.ntpServerName, timeServerIP); 
		//sendNTPpacket(timeServerIP); // send an NTP packet to a time server
		Serial.println("sending NTP packet...");
		memset(packetBuffer, 0, NTP_PACKET_SIZE);
		packetBuffer[0] = 0b11100011;   // LI, Version, Mode
		packetBuffer[1] = 0;     // Stratum, or type of clock
		packetBuffer[2] = 6;     // Polling Interval
		packetBuffer[3] = 0xEC;  // Peer Clock Precision
		packetBuffer[12]  = 49;
		packetBuffer[13]  = 0x4E;
		packetBuffer[14]  = 49;
		packetBuffer[15]  = 52;
		UDPNTPClient.beginPacket(timeServerIP, 123); 
		UDPNTPClient.write(packetBuffer, NTP_PACKET_SIZE);
		UDPNTPClient.endPacket();

		delay(1000);
  
		int cb = UDPNTPClient.parsePacket();
		if (!cb) {
			Serial.println("NTP no packet yet");
		} else {
			Serial.print("NTP packet received, length=");
			Serial.println(cb);
			UDPNTPClient.read(packetBuffer, NTP_PACKET_SIZE); // read the packet into the buffer
			unsigned long highWord = word(packetBuffer[40], packetBuffer[41]);
			unsigned long lowWord = word(packetBuffer[42], packetBuffer[43]);
			unsigned long secsSince1900 = highWord << 16 | lowWord;
			const unsigned long seventyYears = 2208988800UL;
			unsigned long epoch = secsSince1900 - seventyYears;
			unixTimestamp = epoch;
		}
	}
}

void tickHandler() {
	DateTime tempDateTime;
	adminTimeOutCounter++;
	cNTP_Update++;
	unixTimestamp++;
	convertUnixTimeStamp(unixTimestamp +  (config.timezone *  360) , &tempDateTime);
	if (config.daylight) { 
		if (summertime(tempDateTime.year,tempDateTime.month,tempDateTime.day,tempDateTime.hour,0)) {
			convertUnixTimeStamp(unixTimestamp +  (config.timezone *  360) + 3600, &dateTime);
		} else {
			dateTime = tempDateTime;
		}
	} else {
		dateTime = tempDateTime;
	}
	refresh = true;
}
