#include "neopixel_bar.h"

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

uint32_t primary_color = strip.Color(152, 37, 190);
uint32_t highlight_event_color = strip.Color(7, 227, 18);

bool led_state[NUMPIXELS];

void blinkBlueEvent(uint8_t blinkCount, uint16_t delayTime) {
  uint32_t blueColor = strip.Color(0, 0, 255); // Blue color
  
  for (uint8_t i = 0; i < blinkCount; i++) {
    strip.fill(blueColor, 0, strip.numPixels()); // Turn all LEDs blue
    strip.show();
    delay(delayTime); // Wait

    strip.clear(); // Turn off all LEDs
    strip.show();
    delay(delayTime); // Wait
  }
}


void displayLedBaseOnHour(uint8_t h) {
   Serial.printf("hour: %d\n", h);
  float ratio = h * 1.0 / 24;
  uint8_t led_pos = ratio * NUMPIXELS;

  Serial.printf("led pos: %d\n", led_pos);

  // Turn on the specific LED based on the hour
  strip.setPixelColor(led_pos, highlight_event_color);
  led_state[led_pos] = true;  // Mark the LED as on
  strip.show();
}

void clearLedState() {
   for (uint8_t i = 0; i < NUMPIXELS; i++) {
    led_state[i] = false;
  }
}

void displayHourOnNeoPixel()
{
   DateTime now = get_now(); // Get the current time

  uint8_t current_hour = now.hour(); // Get the current hour
  float ceiled_led_amount = NUMPIXELS / 24.0; // Calculate how many LEDs correspond to 1 hour
  uint8_t number_of_leds_need_to_be_light_up = current_hour * ceiled_led_amount;
  uint8_t i;
  
  // Serial.printf("ss: %d| hour: %d | f: %f ", number_of_leds_need_to_be_light_up, current_hour, ceiled_led_amount);
  // Update only the necessary LEDs based on the current hour
  for (i = 0; i < NUMPIXELS; i++) {
    if (i < number_of_leds_need_to_be_light_up && !led_state[i]) {  // Only light up LEDs that aren't already on
      strip.setPixelColor(i, primary_color);  // Turn on the LED
    } else if (i >= number_of_leds_need_to_be_light_up && !led_state[i]) {  // Turn off LEDs that shouldn't be on
      strip.setPixelColor(i, 0);  
    }
  }

  strip.show();
}

void turn_off(){
  strip.clear();   // Clears all pixels
  strip.show();

   for (uint8_t i = 0; i < NUMPIXELS; i++) {
      led_state[i] = false;  // Mark the LED as off
    }
}

void neopixel_setup() {
    strip.begin();
    strip.setBrightness(50);
    strip.show();
}

void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

//Theatre-style crawling lights.
void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

//Theatre-style crawling lights with rainbow effect
void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {     // cycle all 256 colors in the wheel
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));    //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}