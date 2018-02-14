/*
  nodeMcu.homeHeater.HomeSystem
*/
#ifndef HomeSystem.homeHeater.nodeMcu.h
#define HomeSystem.homeHeater.nodeMcu.h

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <EEPROM.h>
#include <Ticker.h>
#include "networkInfo.h"
extern "C" {
#include "user_interface.h"
}

bool frontstatus = false;
bool rearstatus = false;

const int eepromSaveAddressFrontRoomsHeater = 10;
const int eepromSaveAddressRearRoomsHeater = 20;

const char* ssid = NETWORK_SSID;
const char* password = NETWORK_PASSWORD;

#include "watchDogTimer.h"
#include "bc.h"
#include "physicalSwitch.h"
#include "relay.h"
#include "webServer.h"
#include "wifi.h"

//mqtt

#define SERIAL_BAUD 9600

void initHardware()
{
#ifdef SERIAL_DEBUG
  Serial.begin(SERIAL_BAUD);
#endif // SERIAL_DEBUG

  pinMode(rearRoomsHeaterSwitchInputPin, INPUT_PULLUP);

  EEPROM.begin(512);
}

void getSavedSettings(byte &eepromSavedValueFrontRoomsHeater, byte &eepromSavedValueRearRoomsHeater)
{
  // read a byte from the current address of the EEPROM
  eepromSavedValueFrontRoomsHeater = EEPROM.read(eepromSaveAddressFrontRoomsHeater);
  eepromSavedValueRearRoomsHeater = EEPROM.read(eepromSaveAddressRearRoomsHeater);
  frontstatus = (bool)eepromSavedValueFrontRoomsHeater;
  rearstatus = (bool)eepromSavedValueRearRoomsHeater;

#ifdef SERIAL_DEBUG
  Serial.print("Front Rooms heater saved value: ");
  Serial.println((bool)frontstatus);
  Serial.print("Rear Rooms heater saved value: ");
  Serial.println((bool)rearstatus);
#endif // SERIAL_DEBUG
}

void initClientState()
{
  byte eepromSavedValueFrontRoomsHeater, eepromSavedValueRearRoomsHeater;
  getSavedSettings(eepromSavedValueFrontRoomsHeater, eepromSavedValueRearRoomsHeater);

  rearSwitchState = (bool)EEPROM.read(eepromSaveAddressRearRoomsHeaterSwitchState);

  initRelays((int)eepromSavedValueFrontRoomsHeater, (int)eepromSavedValueRearRoomsHeater);
}

#endif // HomeSystem.homeHeater.nodeMcu.h
