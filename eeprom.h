#ifndef eeprom.h
#define eeprom.h

#include <EEPROM.h>

void initEeprom()
{
  EEPROM.begin(512);
}

struct deviceState
{
  bool err;
};

deviceState getEepromSettings()
{
  deviceState savedEeprom;

  return savedEeprom;
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

#endif // eeprom.h
