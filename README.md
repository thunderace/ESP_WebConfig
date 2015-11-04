# ESP_WebConfig
fork of http://www.john-lassen.de/index.php/projects/esp-8266-arduino-ide-webconfig

## Warning
In order to build this with the Arduino IDE, you have to add this on top of ESP_Webconfig.ino (thx to  RattyDAVE) :
```
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Ticker.h>
#include <WiFiUdp.h>
```


## Changelog
23/09/2015 :
  - big memory update - you need the last ESP8266 Arduino IDE (not stable)
  - You need ArduinoJson library : `https://github.com/bblanchon/ArduinoJson`
  - You can use the ESP8266 Makefile to build with command line : `https://github.com/thunderace/Esp8266-Arduino-Makefile`
23/07/2015 
  - code and file organisation cleanup
  - new user specific code
  - potential bug fixes
  - new config support (more flexible and less dangerous for eeprom)
  - PROGMEM removed (crash on nodeMCU) 

## Installation
- Clone this repository : `git clone --recursive https://github.com/thunderace/ESP_WebConfig.git`
- build




