/* 
  ESP_WebConfig 

  Copyright (c) 2015 John Lassen. All rights reserved.
  This is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This software is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  Latest version: 1.1.3  - 2015-07-20
  Changed the loading of the Javascript and CCS Files, so that they will successively loaded and that only one request goes to the ESP.

  -----------------------------------------------------------------------------------------------
  History

  Version: 1.1.2  - 2015-07-17
  Added URLDECODE for some input-fields (SSID, PASSWORD...)

  Version  1.1.1 - 2015-07-12
  First initial version to the public
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Ticker.h>
#include <EEPROM.h>
#include <WiFiUdp.h>
#include "helpers.h"
#include "global.h"
/*
Include the HTML, STYLE and Script "Pages"
*/
#include "pages.h"

#include "user.h"


void setup ( void ) {
	// compute the size needed for config + userConfig
	long eepromSize = 512 * (floor(((sizeof(SystemConfig) + sizeof(UserConfig)) + 512 + 1) /512));
	
	EEPROM.begin(eepromSize);
	Serial.begin(115200);
	delay(500);

	Serial.println("Starting ES8266");
	readConfig();

	if (adminEnabled) {
		WiFi.mode(WIFI_AP_STA);
		WiFi.softAP( ACCESS_POINT_NAME , ACCESS_POINT_PASSWORD);
	} else {
		WiFi.mode(WIFI_STA);
	}

	configureWifi();

	server.on( "/favicon.ico", []() { server.send(200, "text/html", ""); });
	server.on( "/admin.html", showAdminMainPage);
	server.on( "/config.html", send_network_configuration_html);
	server.on( "/info.html", sendInformationPage);
	server.on( "/ntp.html", send_NTP_configuration_html);
	server.on( "/general.html", send_general_html);
	server.on( "/style.css", sendCSS);
	server.on( "/microajax.js", sendJavascript);
	server.on( "/admin/values", send_network_configuration_values_html);
	server.on( "/admin/connectionstate", send_connection_state_values_html);
	server.on( "/admin/infovalues", send_information_values_html);
	server.on( "/admin/ntpvalues", send_NTP_configuration_values_html);
	server.on( "/admin/generalvalues", send_general_configuration_values_html);
	server.on( "/admin/devicename",     send_devicename_value_html);
 	server.onNotFound( []() { Serial.println("Page Not Found"); server.send(400, "text/html", "Page not Found"); });
 	userInit();
	server.begin();
	Serial.println( "HTTP server started" );
	tkSecond.attach(1,tickHandler);
	UDPNTPClient.begin(2390);  // Port for NTP receive
}

 
void loop ( void ) {
//	Serial.print("Config Size : ");
//	Serial.println(configSize);
	if (adminEnabled) {
		if (adminTimeOut != 0 && adminTimeOutCounter > adminTimeOut) {
			adminEnabled = false;
			Serial.println("Admin Mode disabled!");
			WiFi.mode(WIFI_STA);
		}
	}
	if (config.ntpUpdatePeriod  > 0 ) {
		if (cNTP_Update > 5 && firstStart) {
			NTPRefresh();
			cNTP_Update =0;
			firstStart = false;
		} else {
			if (cNTP_Update > (config.ntpUpdatePeriod * 60)) {
				NTPRefresh();
				cNTP_Update =0;
			}
		}
	}

	server.handleClient();
	
	userLoop();
	
/*
	if (refresh) {
		refresh = false;
		Serial.println("Refreshing...");
		Serial.printf("FreeMem:%d %d:%d:%d %d.%d.%d \n",ESP.getFreeHeap() , dateTime.hour,dateTime.minute, dateTime.second, dateTime.year, dateTime.month, dateTime.day);
	}
*/	
	
}

