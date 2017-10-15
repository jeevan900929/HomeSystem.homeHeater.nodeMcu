/*
    This sketch demonstrates how to set up a simple HTTP-like server.
    The server will set a GPIO pin depending on the request
      http://server_ip/RearRoomHeater/Off will set the GPIO2 low,
      http://server_ip/RearRoomHeater/On will set the GPIO2 high
      http://server_ip/FrontRoomHeater/Off will set the GPIO0 low,
      http://server_ip/FrontRoomHeater/On will set the GPIO0 high
    server_ip is the IP address of the ESP8266 module, will be
    printed to Serial when the module is connected.
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

