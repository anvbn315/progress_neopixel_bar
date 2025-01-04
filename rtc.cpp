#include "rtc.h"

#include <RTClib.h>

RTC_DS1307 rtc;

char daysOfTheWeek[7][12] = {
  "Sunday",
  "Monday",
  "Tuesday",
  "Wednesday",
  "Thursday",
  "Friday",
  "Saturday"
};

void setup_rtc() {
    if (!rtc.begin()) {
        Serial.println("RTC not found! Check wiring.");
        while (1) { delay(1000); } // Halt execution if RTC is not detected
    }
    if (!rtc.isrunning()) {
        Serial.println("RTC not running. Setting default time.");
        // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
        // January 21, 2014 at 3am you would call:
        rtc.adjust(DateTime(2024, 12, 18, 7, 0, 0));
    }
}

void print_now(){
  DateTime now = rtc.now();
  Serial.print("Date & Time: ");
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.println(now.second(), DEC);

  delay(1000);  // delay 1 seconds
}

DateTime get_now() {
  return rtc.now();
}