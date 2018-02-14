#ifndef relay.h
#define relay.h

const int FrontRoomsHeaterRelayPin = 12;
const int RearRoomsHeaterRelayPin = 15;

void initRelays(int eepromSavedValueFrontRoomsHeater, int eepromSavedValueRearRoomsHeater)
{
  // prepare GPIO0 with eeprom setting
  pinMode(FrontRoomsHeaterRelayPin, OUTPUT);
  digitalWrite(FrontRoomsHeaterRelayPin, (int)frontstatus);
  //digitalWrite(FrontRoomsHeaterRelayPin, eepromSavedValueFrontRoomsHeater);
  // prepare GPIO2
  pinMode(RearRoomsHeaterRelayPin, OUTPUT);
  digitalWrite(RearRoomsHeaterRelayPin, (int)rearstatus);
  //digitalWrite(RearRoomsHeaterRelayPin, eepromSavedValueRearRoomsHeater);
}

void switchRelay(int val)
{
  // Set GPIO2 & GPIO0 according to the request
  if (val == 0) {
    //frontstatus
    EEPROM.write(eepromSaveAddressFrontRoomsHeater, (byte)1);
    frontstatus = true;
    digitalWrite(FrontRoomsHeaterRelayPin, 1);
    //reportToBackendServer(0);
  }
  else if (val == 1) {
    EEPROM.write(eepromSaveAddressFrontRoomsHeater, (byte)0);
    frontstatus = false;
    digitalWrite(FrontRoomsHeaterRelayPin, 0);
    //reportToBackendServer(1);
  }
  else if (val == 3) {
    //set eeprom
    EEPROM.write(eepromSaveAddressRearRoomsHeater, (byte)1);
    rearstatus = true;
    analogWrite(RearRoomsHeaterRelayPin, 1023);
    //digitalWrite(RearRoomsHeaterRelayPin, 1);
    //reportToBackendServer(2);
  }
  else if (val == 4) {
    //set eeprom
    EEPROM.write(eepromSaveAddressRearRoomsHeater, (byte)0);
    rearstatus = false;
    analogWrite(RearRoomsHeaterRelayPin, 0);
    //digitalWrite(RearRoomsHeaterRelayPin, 0);
    //reportToBackendServer(3);
  }
  EEPROM.commit();
}

#endif // relay.h
