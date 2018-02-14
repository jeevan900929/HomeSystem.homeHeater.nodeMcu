#ifndef physicalSwitch.h
#define physicalSwitch.h

const int frontRoomsHeaterSwitchInputPin = 14;
const int rearRoomsHeaterSwitchInputPin = 13;

bool frontSwitchState = false;
bool rearSwitchState = false;

const int eepromSaveAddressFrontRoomsHeaterSwitchState = 15;
const int eepromSaveAddressRearRoomsHeaterSwitchState = 25;

void readSwitchState()
{
  rearSwitchState = (bool)digitalRead(rearRoomsHeaterSwitchInputPin);
  //frontstatus = (bool)digitalRead(frontRoomsHeaterSwitchInputPin);

  if (rearSwitchState != rearstatus)
  {
    rearstatus = rearSwitchState;
    //set eeprom
    if (rearstatus)
    {
      EEPROM.write(eepromSaveAddressRearRoomsHeaterSwitchState, (byte)1);
      EEPROM.write(eepromSaveAddressRearRoomsHeater, (byte)1);
    }
    else
    {
      EEPROM.write(eepromSaveAddressRearRoomsHeaterSwitchState, (byte)0);
      EEPROM.write(eepromSaveAddressRearRoomsHeater, (byte)0);
    }
    //if switch on, off relay;
    updateServerState(0);
  }
}

#endif // physicalSwitch.h
