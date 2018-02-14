#ifndef bc.h
#define bc.h

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

#endif // bc.h
