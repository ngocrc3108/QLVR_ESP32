#include <Arduino.h>
#include <HTTPClient.h>
#include "main.h"

#define ssid "MSI"
#define password "31082003"

String webAppUrl = "https://htn-server.onrender.com/";

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200);
  initWiFi();
}

void loop() {
  HTTPClient http;
  http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
  String tempUrl = webAppUrl + "main?cmd=read&id=12345";
  http.begin(tempUrl);
  int responseCode = http.GET();
  Serial.printf("response code: %d\n", responseCode);
  String data = http.getString();
  Serial.println(data);
  Serial2.print(data);
  http.end();
  delay(5000);
}

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi ");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.printf("\nIp adrress: %s\n", WiFi.localIP().toString());
}