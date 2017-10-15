/*
  nodeMcu.homeHeater.HomeSystem

  The server will set GPIO pins depending on the request
  http://192.168.1.16/FrontRoomsHeater/Off will set the GPIO12 low,
  http://192.168.1.16/FrontRoomsHeater/On will set the GPIO12 high,
  http://192.168.1.16/RearRoomsHeater/Off will set the GPIO15 low,
  http://192.168.1.16/RearRoomsHeater/On will set the GPIO15 high

  The circuit:
    list the components attached to each input
  Front Rooms Heater Relay attached to GPIO12
  Rear Rooms Heater Relay attached to GPIO15

  Created 17 Jul 2017
  By jeevanAnga
  Modified 16 Oct 2017
  By jeevanAnga

  https://github.com/jeevan900929/HomeSystem.homeHeater.nodeMcu/blob/master/README.md

*/

#define ESP8266_INITIALIZATION
#undef SERIAL_DEBUG
#undef WATCHDOG

#include "HomeSystem.homeHeater.nodeMcu.h"

void setup()
{
  initWdt();

  initHardware();

  byte eepromSavedValueFrontRoomsHeater, eepromSavedValueRearRoomsHeater;
  getSavedSettings(eepromSavedValueFrontRoomsHeater, eepromSavedValueRearRoomsHeater);

  initRelays((int)eepromSavedValueFrontRoomsHeater, (int)eepromSavedValueRearRoomsHeater);

  initWiFi();
}

void loop()
{
  WiFiClient client;

  processWiFi(client);

  int val;
  parseHttpRequest(val, client);

  switchRelay(val);

  sendHttpResponse(val, client);

  feedWdt();
}

