/*
  nodeMcu.homeHeater.HomeSystem
*/
#ifndef HomeSystem.homeHeater.nodeMcu.h
#define HomeSystem.homeHeater.nodeMcu.h

#include <ESP8266HTTPClient.h>
extern "C" {
#include "user_interface.h"
}

bool frontstatus = false;
bool rearstatus = false;

const int eepromSaveAddressFrontRoomsHeater = 10;
const int eepromSaveAddressRearRoomsHeater = 20;

#include "watchDogTimer.h"
#include "eeprom.h"
#include "debugOutput.h"
#include "backEndConnection.h"
#include "physicalSwitch.h"
#include "relay.h"
#include "webServer.h"
#include "webClient.h"

//mqtt

void initHardware()
{
  initWatchDogTimer();

  initEeprom();

  initDebugOutput();

  initPhysicalSwitches();

  initWebClient();

}

void initClientState()
{
  getEepromSettings();

  setRelays();

  byte eepromSavedValueFrontRoomsHeater, eepromSavedValueRearRoomsHeater;
  getSavedSettings(eepromSavedValueFrontRoomsHeater, eepromSavedValueRearRoomsHeater);

  rearSwitchState = (bool)EEPROM.read(eepromSaveAddressRearRoomsHeaterSwitchState);

  initRelays((int)eepromSavedValueFrontRoomsHeater, (int)eepromSavedValueRearRoomsHeater);
}

boolean clientStateIsChanged()
{

}

boolean serverStateIsChanged()
{

}

void updateClientState()
{

}

void updateServerState()
{

}

#endif // HomeSystem.homeHeater.nodeMcu.h
