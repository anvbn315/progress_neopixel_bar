#include "wifi_connect.h"

void connect_wifi(const char* ssid, const char* password, const char* ip_host, const uint16_t port) {
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to Wi-Fi");
}