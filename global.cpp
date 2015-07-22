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
const int adminTimeOut  = 0;  // Defines the Time in Seconds, when the Admin-Mode will be diabled : 0 = disable

byte minuteOld = 100;				// Helpvariable for checking, when a new Minute comes up (for Auto Turn On / Off)



strConfig config;


void configureWifi() {
	Serial.println("Configuring Wifi");
	WiFi.begin (config.ssid.c_str(), config.password.c_str());
	if (!config.dhcp) {
		WiFi.config(IPAddress(config.IP[0],config.IP[1],config.IP[2],config.IP[3]),  
								IPAddress(config.gateway[0],config.gateway[1],config.gateway[2],config.gateway[3]), 
								IPAddress(config.netmask[0],config.netmask[1],config.netmask[2],config.netmask[3]));
	}
}

void writeConfig() {
	Serial.println("Writing Config");
	EEPROM.write(0,'C');
	EEPROM.write(1,'F');
	EEPROM.write(2,'G');
	EEPROM.write(16,config.dhcp);
	EEPROM.write(17,config.daylight);
	
	EEPROMWritelong(18,config.ntpUpdatePeriod); // 4 Byte
	EEPROMWritelong(22,config.timezone);  // 4 Byte

	EEPROM.write(26,config.ledR);
	EEPROM.write(27,config.ledG);
	EEPROM.write(28,config.ledB);

	EEPROM.write(32,config.IP[0]);
	EEPROM.write(33,config.IP[1]);
	EEPROM.write(34,config.IP[2]);
	EEPROM.write(35,config.IP[3]);

	EEPROM.write(36,config.netmask[0]);
	EEPROM.write(37,config.netmask[1]);
	EEPROM.write(38,config.netmask[2]);
	EEPROM.write(39,config.netmask[3]);

	EEPROM.write(40,config.gateway[0]);
	EEPROM.write(41,config.gateway[1]);
	EEPROM.write(42,config.gateway[2]);
	EEPROM.write(43,config.gateway[3]);

	EEPROMWriteString(64,config.ssid);
	EEPROMWriteString(96,config.password);
	EEPROMWriteString(128,config.ntpServerName);

	EEPROM.write(300,config.autoTurnOn);
	EEPROM.write(301,config.autoTurnOff);
	EEPROM.write(302,config.turnOnHour);
	EEPROM.write(303,config.turnOnMinute);
	EEPROM.write(304,config.turnOffHour);
	EEPROM.write(305,config.turnOffMinute);
	EEPROMWriteString(306,config.deviceName);
	
	EEPROM.commit();
}

boolean readConfig() {
	Serial.println("Reading Configuration");
	if (EEPROM.read(0) == 'C' && EEPROM.read(1) == 'F' && EEPROM.read(2) == 'G' ) {
		Serial.println("Configurarion Found!");
		config.dhcp = 	EEPROM.read(16);
		config.daylight = EEPROM.read(17);
		config.ntpUpdatePeriod = EEPROMReadlong(18); // 4 Byte
		config.timezone = EEPROMReadlong(22); // 4 Byte
		config.ledR = EEPROM.read(26);
		config.ledG = EEPROM.read(27);
		config.ledB = EEPROM.read(28);

		config.IP[0] = EEPROM.read(32);
		config.IP[1] = EEPROM.read(33);
		config.IP[2] = EEPROM.read(34);
		config.IP[3] = EEPROM.read(35);
		config.netmask[0] = EEPROM.read(36);
		config.netmask[1] = EEPROM.read(37);
		config.netmask[2] = EEPROM.read(38);
		config.netmask[3] = EEPROM.read(39);
		config.gateway[0] = EEPROM.read(40);
		config.gateway[1] = EEPROM.read(41);
		config.gateway[2] = EEPROM.read(42);
		config.gateway[3] = EEPROM.read(43);
		config.ssid = EEPROMReadString(64);
		config.password = EEPROMReadString(96);
		config.ntpServerName = EEPROMReadString(128);
		
		config.autoTurnOn = EEPROM.read(300);
		config.autoTurnOff = EEPROM.read(301);
		config.turnOnHour = EEPROM.read(302);
		config.turnOnMinute = EEPROM.read(303);
		config.turnOffHour = EEPROM.read(304);
		config.turnOffMinute = EEPROM.read(305);
		config.deviceName= EEPROMReadString(306);
		return true;
	} else {
		Serial.println("Configuration NOT FOUND!!!!");
		return false;
	}
}


#define NTP_PACKET_SIZE 48 
byte packetBuffer[NTP_PACKET_SIZE]; 
void NTPRefresh() {
	if (WiFi.status() == WL_CONNECTED) {
		IPAddress timeServerIP; 
		WiFi.hostByName(config.ntpServerName.c_str(), timeServerIP); 
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

void secondTick() {
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
