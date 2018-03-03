#ifndef backEndConnection.h
#define backEndConnection.h


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

void updateServerState(int serverStateMode) //reportToBackendServer
{
  HTTPClient http;

  Serial.print("[HTTP] begin...\n");
  // configure traged server and url
  //http.begin("https://192.168.1.12/test.html", "7a 9c f4 db 40 d3 62 5a 6e 21 bc 5c cc 66 c8 3e a1 45 59 38"); //HTTPS
  String url0 = "http://192.168.1.11:3100/heaters/rear/5a97848ea83c1337a010c4d5";
  String url1 = "http://192.168.1.11:3100/heaters/rear/heater9/false";
  String url2 = "http://192.168.1.11:3100/heaters/rear/heater9/true";

  //http.begin(url0); //HTTP

  if (rearstatus)
    http.begin(url1);
  else
    http.begin(url2);


  Serial.print("[HTTP] GET...\n");
  // start connection and send HTTP header
  int httpCode = http.GET();

  /*

    Serial.print("[HTTP] PUT...\n");
    // start connection and send HTTP header
    //int httpCode = http.GET();
    int httpCode0;

    http.addHeader("content-type", "application/json", false, false);
    //http.addHeader("authorization", "basic YWRtaW5pc3RyYXRvcjpwYXNzd29yZA==", false, false);

    String payload0;

    if (rearstatus)
    {
      //payload0 = "{\"state\": \"true\"}\r\n";
      payload0 = "{\"heater\":{\"state\":{\"on1\":true}}}";
    }
    else
    {
      //payload0 = "{\"state\": \"false\"}\r\n";
      payload0 = "{\"heater\":{\"state\":{\"on1\":false}}}";
    }

    httpCode0 = http.PUT(payload0);
    Serial.print("Putting: ");
    Serial.println(payload0);

    // httpCode will be negative on error
    if (httpCode0 > 0) {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] GET... code: %d\n", httpCode0);

    // file found at server
    if (httpCode0 == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    }
    } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode0).c_str());
    }
  */

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

#endif // backEndConnection.h
