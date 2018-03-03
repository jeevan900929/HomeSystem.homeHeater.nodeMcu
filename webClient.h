#ifndef webClient.h
#define webClient.h

#include "networkInfo.h"

#ifdef ESP8266_INITIALIZATION

#include <ESP8266WiFi.h>

WiFiServer server(80);  // Create an instance of the server, specify the port to listen on as an argument

#undef CUSTOM_MAC
uint8_t Mac[] = { 0x5c, 0xcf, 0x7f, 0x99, 0x11, 0x02 };

#define STATIC_IP
IPAddress ip(192, 168, 1, 16); // where xx is the desired IP Address
IPAddress gateway(192, 168, 1, 1); // set gateway to match your network
IPAddress subnet(255, 255, 255, 0); // set subnet mask to match your network

#endif // ESP8266_INITIALIZATION

const char* ssid = NETWORK_SSID;
const char* password = NETWORK_PASSWORD;

void startServer()
{
  // Start the server
  server.begin();

#ifdef SERIAL_DEBUG
  Serial.print("Client started at IP: ");

  // Print the IP address
  Serial.println(WiFi.localIP());
#endif // SERIAL_DEBUG
}

void initWebClient()
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

WiFiClient client;

void processWiFi()
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

void parseHttpRequest(int &val)
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

void sendHttpResponse(int val)
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

#endif // webClient.h
