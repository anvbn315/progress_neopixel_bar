#pragma once
#include "RTClib.h"

extern RTC_DS1307 rtc;

void setup_rtc();
void print_now();
DateTime get_now();
