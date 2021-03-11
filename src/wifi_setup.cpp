
#include "WiFi.h"

void setupWifi(const char* ssid, const char* pwd){
  WiFi.begin(ssid, pwd);
  int cnt = 0;
  while (WiFi.status() != WL_CONNECTED && cnt < 5) {
    delay(1000);
    Serial.println(F("Connecting to WiFi..."));
    cnt++;
  }
  if(WiFi.status() == WL_CONNECTED){
    Serial.println(F("Connected to the WiFi network"));
  }
  else{
    Serial.println(F("Connection to WIFI failed:" + WiFi.status()));
  }
}