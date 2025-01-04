#pragma once
#include <Adafruit_NeoPixel.h>
#include <TimeLib.h>
#include "rtc.h"

#define PIN D6
#define NUMPIXELS 60

extern Adafruit_NeoPixel strip;

void colorWipe(uint32_t c, uint8_t wait);
void rainbow(uint8_t wait);
void rainbowCycle(uint8_t wait);
void theaterChase(uint32_t c, uint8_t wait);
void theaterChaseRainbow(uint8_t wait);
uint32_t Wheel(byte WheelPos);
void neopixel_setup();
void turn_off();
void displayHourOnNeoPixel();
void displayLedBaseOnHour(uint8_t h);
void clearLedState();
void blinkBlueEvent(uint8_t blinkCount, uint16_t delayTime);
