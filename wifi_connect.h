#pragma once
#include <Arduino.h>
#include <ESP8266WiFi.h>


void connect_wifi(const char* ssid, const char* password, const char* ip_host, const uint16_t port);
