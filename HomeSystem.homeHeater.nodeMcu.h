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

//mqtt

#define SERIAL_BAUD 9600

#ifdef ESP8266_INITIALIZATION

#undef CUSTOM_MAC
uint8_t Mac[] = { 0x5c, 0xcf, 0x7f, 0x99, 0x11, 0x02 };

#define STATIC_IP
IPAddress ip(192, 168, 1, 16); // where xx is the desired IP Address
IPAddress gateway(192, 168, 1, 1); // set gateway to match your network
IPAddress subnet(255, 255, 255, 0); // set subnet mask to match your network

#endif // ESP8266_INITIALIZATION

const char* ssid = NETWORK_SSID;
const char* password = NETWORK_PASSWORD;

WiFiServer server(80);  // Create an instance of the server, specify the port to listen on as an argument

const int eepromSaveAddressFrontRoomsHeater = 10;
const int eepromSaveAddressRearRoomsHeater = 20;

const int FrontRoomsHeaterRelayPin = 12;
const int RearRoomsHeaterRelayPin = 15;

const int rearRoomsHeaterSwitchInputPin = 13;

bool frontstatus = false;
bool rearstatus = false;

#ifdef WATCHDOG

Ticker secondTick;
volatile int watchdogCount = 0;

void ISRwatchdog()
{
  watchdogCount++;
  if (watchdogCount == 5)
  {
    Serial.println("the watchdog bit");
    ESP.reset();
  }
}
#endif // WATCHDOG

void initWdt()
{

#ifdef WATCHDOG
  ESP.wdtDisable();
  secondTick.attach(1, ISRwatchdog);
  ESP.wdtEnable(WDTO_8S);
#endif // WATCHDOG

}

void feedWdt()
{
#ifdef WATCHDOG
  ESP.wdtFeed();
  Serial.print("Watchdog pat at ");
  Serial.println(watchdogCount);
  watchdogCount = 0;
  yield();
#endif // WATCHDOG
}

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

void initClientState()
{
  byte eepromSavedValueFrontRoomsHeater, eepromSavedValueRearRoomsHeater;
  getSavedSettings(eepromSavedValueFrontRoomsHeater, eepromSavedValueRearRoomsHeater);

  initRelays((int)eepromSavedValueFrontRoomsHeater, (int)eepromSavedValueRearRoomsHeater);
}

void startServer()
{
  // Start the server
  server.begin();

#ifdef SERIAL_DEBUG
  Serial.print("Server started at IP: ");

  // Print the IP address
  Serial.println(WiFi.localIP());
#endif // SERIAL_DEBUG
}

void reportToBackendServer(int event)
{
  const char http_site[] = "www.example.com";
  const int http_port = 80;

  WiFiClient client;

  if ( client.connect(http_site, http_port) ) {
    // Make an HTTP GET request
    switch (event)
    {
      case -1:
        client.println("GET /bootup.html HTTP/1.1");
        break;
      case 0:
        client.println("GET /frontOn.html HTTP/1.1");
        break;
      case 1:
        client.println("GET /frontOff.html HTTP/1.1");
        break;
      case 2:
        client.println("GET /rearOn.html HTTP/1.1");
        break;
      case 3:
        client.println("GET /rearOff.html HTTP/1.1");
        break;
    }
    client.print("Host: ");
    client.println(http_site);
    client.println("Connection: close");
    client.println();
  }

}

void initWiFi()
{
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

#ifdef SERIAL_DEBUG
  Serial.println();  Serial.println();  Serial.print("Connecting to ");  Serial.println(ssid);
#endif // SERIAL_DEBUG

  //  delay(10);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);

#ifdef SERIAL_DEBUG
    Serial.print(".");
#endif // SERIAL_DEBUG

    feedWdt();
  }

#ifdef SERIAL_DEBUG
  Serial.println("");
  Serial.println("WiFi connected");
#endif // SERIAL_DEBUG

  startServer();

  //reportToBackendServer(-1);
}

void updateServerState(int serverStateMode) //reportToBackendServer
{
  HTTPClient http;

  Serial.print("[HTTP] begin...\n");
  // configure traged server and url
  //http.begin("https://192.168.1.12/test.html", "7a 9c f4 db 40 d3 62 5a 6e 21 bc 5c cc 66 c8 3e a1 45 59 38"); //HTTPS
  http.begin("http://192.168.1.11:3100/heaters/23"); //HTTP

  Serial.print("[HTTP] GET...\n");
  // start connection and send HTTP header
  int httpCode = http.GET();

  // httpCode will be negative on error
  if (httpCode > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);

    // file found at server
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}

void readSwitchState()
{
  bool rearSwitchState = (bool)digitalRead(rearRoomsHeaterSwitchInputPin);
  //frontstatus = (bool)digitalRead(frontRoomsHeaterSwitchInputPin);

  if (rearSwitchState != rearstatus)
  {
    rearstatus = rearSwitchState;
    //if switch on, off relay;
    updateServerState(0);
  }
}

void processWiFi(WiFiClient &client)
{
  // Check if a client has connected
  client = server.available();
  if (!client)
  {
    return;
  }

  // Wait until the client sends some data
  delay(1);//Serial.println("new client");
  while (!client.available())
  {
    //Fix bug that stalls program running here...
    delay(1);
#ifdef SERIAL_DEBUG
    Serial.print(".");
#endif // SERIAL_DEBUG
  }
}

void parseHttpRequest(int &val, WiFiClient &client)
{
  // Read the first line of the request
  String req = client.readStringUntil('\r');

#ifdef SERIAL_DEBUG
  //if (req != "\r")
  //Serial.println(req);
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
    val = 5; /*
  else if (req.indexOf("favicon.ico") != -1) {
    s = "HTTP/1.1 404 No Favicon\r\n";
    val = -2;
  } */
  else {
    val = -1;
#ifdef SERIAL_DEBUG
    //Serial.println("No valid request");
#endif // SERIAL_DEBUG
    client.stop();//
    return;
  }
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

void sendHttpResponse(int val, WiFiClient &client)
{
  client.flush();

  // Prepare the response
  String s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\n";

  if (val == 0)
    s += "Front Rooms Heater turned on";
  else if (val == 1)
    s += "Front Rooms Heater turned off";
  else if (val == 2)
    //s += (int)EEPROM.read(eepromSaveAddressFrontRoomsHeater);
    s += (int)frontstatus;
  else if (val == 3)
    s += "Rear Rooms Heater turned on";
  else if (val == 4)
    s += "Rear Rooms Heater turned off";
  else if (val == 5)
    //s += (int)EEPROM.read(eepromSaveAddressRearRoomsHeater);
    s += (int)rearstatus;

  //s += "&nbsp;&nbsp;<input type=button onClick=""parent.location='http://alienwareX51/'"" value='Return'>";

  s += "</html>\n";

  // Send the response to the client
  client.print(s);
  delay(1);//
}

#endif // HomeSystem.homeHeater.nodeMcu.h
