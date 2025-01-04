#pragma once
#include <WebSocketsClient.h>
#include <Arduino.h>
#include "config.h"
#include "neopixel_bar.h"
#include <ArduinoJson.h>
#include "rtc.h"

// Declare the function with the correct signature
void connect_websocket(WebSocketsClient &ws, const char* ip_host, const uint16_t port);

void handle_websocket_evt(WebSocketsClient &ws);
