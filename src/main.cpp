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

/* 
 * Show current device version
 */
void handleRoot()
{
  server.send(200, "text/plain", "v" + String(CURRENT_VERSION));
}

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  pinMode(LED_BUILTIN, OUTPUT);

  delay(3000);
  Serial.println("\n Starting");
  // Setup Wifi Manager
  String version = String("<p>Current Version - v") + String(CURRENT_VERSION) + String("</p>");
  USE_SERIAL.println(version);

  WiFiManager wm;
  WiFiManagerParameter versionText(version.c_str());
  wm.addParameter(&versionText);
  
  if (!wm.autoConnect("Robotics", "nopebook"))
  {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.restart();
    delay(1000);
  }

  // Check if we need to download a new version
  String downloadUrl = getDownloadUrl();
  if (downloadUrl.length() > 0)
  {
    bool success = downloadUpdate(downloadUrl);
    if (!success)
    {
      USE_SERIAL.println("Error updating device");
    }
  }

  server.on("/", handleRoot);
  server.begin();
  USE_SERIAL.println("HTTP server started");

  USE_SERIAL.print("IP address: ");
  USE_SERIAL.println(WiFi.localIP());
}

int ledState = LOW;
const long interval = 1000;
unsigned long previousMillis = 0;

void loop()
{

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    ledState = ledState == LOW ? HIGH : LOW;
    digitalWrite(BUILTIN_LED, ledState);
  }

  // Just chill
  server.handleClient();
}