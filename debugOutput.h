#ifndef debugOutput.h
#define debugOutput.h

#define SERIAL_BAUD 9600

void initDebugOutput() {

#ifdef SERIAL_DEBUG
  Serial.begin(SERIAL_BAUD);
#endif // SERIAL_DEBUG
}

#endif // debugOutput.h
