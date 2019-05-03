#include <Arduino.h>
#include <DNSServer.h>
#include <WebServer.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>
#define VARIANT "esp32"
#include <WiFiManager.h>
#define USE_SERIAL Serial

#define CURRENT_VERSION VERSION
#define CLOUD_FUNCTION_URL "http://us-central1-aravision.cloudfunctions.net/getDownloadUrl"


/* 
 * Check if needs to update the device and returns the download url.
 */
String getDownloadUrl()
{
  HTTPClient http;
  String downloadUrl;
  USE_SERIAL.print("[HTTP] begin...\n");

  String url = CLOUD_FUNCTION_URL;
  url += String("?version=") + CURRENT_VERSION;
  url += String("&variant=") + VARIANT;
  http.begin(url);

  USE_SERIAL.print("[HTTP] GET...\n");
  // start connection and send HTTP header
  int httpCode = http.GET();

  // httpCode will be negative on error
  if (httpCode > 0)
  {
    // HTTP header has been send and Server response header has been handled
    USE_SERIAL.printf("[HTTP] GET... code: %d\n", httpCode);

    // file found at server
    if (httpCode == HTTP_CODE_OK)
    {
      String payload = http.getString();
      USE_SERIAL.println(payload);
      downloadUrl = payload;
    }
    else
    {
      USE_SERIAL.println("Device is up to date!");
    }
  }
  else
  {
    USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();

  return downloadUrl;
}

void setup()
{
  // put your setup code here, to run once:
}

void loop()
{
  // put your main code here, to run repeatedly:
}