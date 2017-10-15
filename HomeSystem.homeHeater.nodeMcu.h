/*
  nodeMcu.homeHeater.HomeSystem
*/
#ifndef HomeSystem.homeHeater.nodeMcu.h
#define HomeSystem.homeHeater.nodeMcu.h

extern "C" {
#include "user_interface.h"
}

#include <ESP8266WiFi.h>
#include <EEPROM.h>

#include <Ticker.h>

#define SERIAL_BAUD 9600

Ticker secondTick;

volatile int watchdogCount = 0;

#ifdef ESP8266_INITIALIZATION

#undef CUSTOM_MAC
uint8_t Mac[] = { 0x5c, 0xcf, 0x7f, 0x99, 0x11, 0x02 };

#define STATIC_IP
IPAddress ip(192, 168, 1, 16); // where xx is the desired IP Address
IPAddress gateway(192, 168, 1, 1); // set gateway to match your network
IPAddress subnet(255, 255, 255, 0); // set subnet mask to match your network

#endif // ESP8266_INITIALIZATION

const char* ssid = "x";
const char* password = "x";

// Create an instance of the server
// specify the port to listen on as an argument
WiFiServer server(80);

const int eepromSaveAddressFrontRoomsHeater = 10;
const int eepromSaveAddressRearRoomsHeater = 20;

const int FrontRoomsHeaterRelayPin = 12;
const int RearRoomsHeaterRelayPin = 15;

//frontstatus
//rearstatus

#ifdef WATCHDOG
void ISRwatchdog()
{
  watchdogCount++;
  if (watchdogCount == 15)
  {
    Serial.println("the watchdog bit");
    ESP.reset();
  }
}
#endif // WATCHDOG

void parseHttpRequest(int &val, WiFiClient &client) {

  // Read the first line of the request
  String req = client.readStringUntil('\r');

#ifdef SERIAL_DEBUG
  Serial.println(req);
#endif // SERIAL_DEBUG
  client.flush();

  // Match the request
  if (req.indexOf("/FrontRoomsHeater/On") != -1)
    val = 0;
  else if (req.indexOf("/FrontRoomsHeater/Off") != -1)
    val = 1;
  else if (req.indexOf("/FrontRoomsHeater/Status") != -1)
    val = 2;
  else if (req.indexOf("/RearRoomsHeater/On") != -1)
    val = 3;
  else if (req.indexOf("/RearRoomsHeater/Off") != -1)
    val = 4;
  else if (req.indexOf("/RearRoomsHeater/Status") != -1)
    val = 5; /*/*
  else if (req.indexOf("favicon.ico") != -1) {
    s = "HTTP/1.1 404 No Favicon\r\n";
    val = -2;
  } */
  else {
    val = -1;
#ifdef SERIAL_DEBUG
    Serial.println("invalid request");
#endif // SERIAL_DEBUG
    client.stop();//
    return;
  }

}

void switchRelay(int val) {

  // Set GPIO2 & GPIO0 according to the request
  if (val == 0) {
    //frontstatus
    EEPROM.write(eepromSaveAddressFrontRoomsHeater, (byte)1);
    digitalWrite(FrontRoomsHeaterRelayPin, 1);
  }
  else if (val == 1) {
    EEPROM.write(eepromSaveAddressFrontRoomsHeater, (byte)0);
    digitalWrite(FrontRoomsHeaterRelayPin, 0);
  }
  else if (val == 3) {
    //set eeprom
    EEPROM.write(eepromSaveAddressRearRoomsHeater, (byte)1);
    analogWrite(RearRoomsHeaterRelayPin, 1023);
    //digitalWrite(RearRoomsHeaterRelayPin, 1);
  }
  else if (val == 4) {
    //set eeprom
    EEPROM.write(eepromSaveAddressRearRoomsHeater, (byte)0);
    analogWrite(RearRoomsHeaterRelayPin, 0);
    //digitalWrite(RearRoomsHeaterRelayPin, 0);
  }
  EEPROM.commit();

}

void sendHttpResponse(int val, WiFiClient &client) {

  client.flush();

  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\n";

  if (val == 0)
    s += "Front Rooms Heater turned on";
  else if (val == 1)
    s += "Front Rooms Heater turned off";
  else if (val == 2)
    s += (int)EEPROM.read(eepromSaveAddressFrontRoomsHeater); //frontstatus
  else if (val == 3)
    s += "Rear Rooms Heater turned on";
  else if (val == 4)
    s += "Rear Rooms Heater turned off";
  else if (val == 5)
    s += (int)EEPROM.read(eepromSaveAddressRearRoomsHeater); //rearstatus

  //s += "&nbsp;&nbsp;<input type=button onClick=""parent.location='http://alienwareX51/'"" value='Return'>";

  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);//
}

void initHardware()
{

#ifdef SERIAL_DEBUG
  Serial.begin(SERIAL_BAUD);
#endif // SERIAL_DEBUG

  EEPROM.begin(512);

}

void getSavedSettings(byte &eepromSavedValueFrontRoomsHeater, byte &eepromSavedValueRearRoomsHeater)
{
  // read a byte from the current address of the EEPROM
  eepromSavedValueFrontRoomsHeater = EEPROM.read(eepromSaveAddressFrontRoomsHeater);
  eepromSavedValueRearRoomsHeater = EEPROM.read(eepromSaveAddressRearRoomsHeater);
  //frontstatus = (int)eepromSavedValueFrontRoomsHeater)
  //frontstatus =

#ifdef SERIAL_DEBUG

  Serial.print("Front Rooms heater saved value: ");
  Serial.println((int)eepromSavedValueFrontRoomsHeater);
  Serial.print("Rear Rooms heater saved value: ");
  Serial.println((int)eepromSavedValueRearRoomsHeater);

#endif // SERIAL_DEBUG

}

void initRelays(int eepromSavedValueFrontRoomsHeater, int eepromSavedValueRearRoomsHeater)
{
  // prepare GPIO0 with eeprom setting
  pinMode(FrontRoomsHeaterRelayPin, OUTPUT);
  digitalWrite(FrontRoomsHeaterRelayPin, eepromSavedValueFrontRoomsHeater); //fonrtstatus
  // prepare GPIO2
  pinMode(RearRoomsHeaterRelayPin, OUTPUT);
  digitalWrite(RearRoomsHeaterRelayPin, eepromSavedValueRearRoomsHeater);

}

void initWiFi()
{

}

void startWiFi()
{

  // Connect to WiFi network

#ifdef SERIAL_DEBUG

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

#endif // SERIAL_DEBUG

  //  delay(10);

#ifdef ESP8266_INITIALIZATION

#ifdef CUSTOM_MAC
  //Set MAC Address
  wifi_set_macaddr(STATION_IF, Mac);
#endif // CUSTOM_MAC

#ifdef STATIC_IP
  //Set Static IP Address
  WiFi.config(ip, gateway, subnet);
#endif // STATIC_IP

  //Set mode
  WiFi.mode(WIFI_STA);
  /* WiFi.mode(WIFI_AP);


    //if ap_sta

    //Set AP Name String
    String AP_NameString = "ESP8266 Device "; //+ Mac;

    char AP_NameChar[AP_NameString.length() + 1];
    memset(AP_NameChar, 0, AP_NameString.length() + 1);

    for (int i = 0; i < AP_NameString.length(); i++)
     AP_NameChar[i] = AP_NameString.charAt(i);

    WiFi.softAP(AP_NameChar, WiFiAPPSK);
  */

#endif // ESP8266_INITIALIZATION

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);

#ifdef SERIAL_DEBUG
    Serial.print(".");
#endif // SERIAL_DEBUG

    ESP.wdtFeed();
    watchdogCount = 0;
    yield();

  }

#ifdef SERIAL_DEBUG
  Serial.println("");
  Serial.println("WiFi connected");
#endif // SERIAL_DEBUG

  // Start the server
  server.begin();

#ifdef SERIAL_DEBUG
  Serial.print("Server started at IP: ");

  // Print the IP address
  Serial.println(WiFi.localIP());
#endif // SERIAL_DEBUG
}

void initWdt()
{

}

#endif // HomeSystem.homeHeater.nodeMcu.h
