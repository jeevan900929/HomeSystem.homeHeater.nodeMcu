/*
  nodeMcu.homeHeater.HomeSystem

  The server will set GPIO pins depending on the request
  http://192.168.1.16/FrontRoomsHeater/Off will set the GPIO12 low,
  http://192.168.1.16/FrontRoomsHeater/On will set the GPIO12 high,
  http://192.168.1.16/RearRoomsHeater/Off will set the GPIO15 low,
  http://192.168.1.16/RearRoomsHeater/On will set the GPIO15 high

  The circuit:
  * list the components attached to each input
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
//mqtt

void setup() {
  //ESP.wdtDisable();

  initHardware();

#ifdef WATCHDOG
  secondTick.attach(1, ISRwatchdog);
#endif // WATCHDOG

  byte eepromSavedValueFrontRoomsHeater, eepromSavedValueRearRoomsHeater;
  getSavedSettings(eepromSavedValueFrontRoomsHeater, eepromSavedValueRearRoomsHeater);

  initRelays((int)eepromSavedValueFrontRoomsHeater, (int)eepromSavedValueRearRoomsHeater);

  startWiFi();

#ifdef WATCHDOG
  //initWdt();
  ESP.wdtEnable(WDTO_8S);
#endif // WATCHDOG
}

void loop()
{

  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client)
  {
    return;
  }

  // Wait until the client sends some data
  //delay(1);//Serial.println("new client");
  while (!client.available())
  {
    delay(1);
#ifdef SERIAL_DEBUG
    Serial.print(".");
#endif // SERIAL_DEBUG
  }

  int val;
  parseHttpRequest(val, client);

  switchRelay(val);

  sendHttpResponse(val, client);


#ifdef SERIAL_DEBUG
  Serial.println("Client disonnected");
  // The client will actually be disconnected
  // when the loop function returns and 'client' object is detroyed
#endif // SERIAL_DEBUG

#ifdef WATCHDOG
  //feedWatchDog();
  ESP.wdtFeed();
  Serial.print("Watchdog pat at ");
  Serial.println(watchdogCount);
  watchdogCount = 0;
#endif // WATCHDOG
}

