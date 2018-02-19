#ifndef webServer.h
#define webServer.h

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

#endif // webServer.h
