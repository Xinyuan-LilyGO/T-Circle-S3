#include "Led_APA102.hpp"

#include "Arduino.h"
#include "FastLED.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"

#define APA102_DATA  38
#define APA102_CLOCK 39

#define NUM_LEDS 1

static uint32_t APA102_RGB_Data[] = {CRGB::Red, CRGB::Green, CRGB::Blue,
                                     CRGB::Black};

// Define the array of leds
static CRGB leds[NUM_LEDS];

Led_APA102::Led_APA102() : ILed(-1) {
  FastLED.addLeds<APA102, APA102_DATA, APA102_CLOCK, BGR>(leds, NUM_LEDS);
  set(false);
}

void Led_APA102::set(bool state) {
  if (state) {
    leds[0] = APA102_RGB_Data[3];
  }

  FastLED.show();
}

void Led_APA102::set(Colour colour, int led_num, Brightness b) {
  switch (b) {
  case Brightness::_25:
    FastLED.setBrightness(25);
    break;
  case Brightness::_50:
    FastLED.setBrightness(50);
    break;
  case Brightness::_100:
    FastLED.setBrightness(100);
    break;
  default:
    break;
  }

  switch (colour) {
  case Colour::Red:
    leds[0] = APA102_RGB_Data[0];
    break;
  case Colour::Green:
    leds[0] = APA102_RGB_Data[1];
    break;
  case Colour::Blue:
    leds[0] = APA102_RGB_Data[2];
    break;
  case Colour::Black:
    leds[0] = APA102_RGB_Data[3];
    break;
  default:
    leds[0] = APA102_RGB_Data[0];
    break;
  }
  FastLED.show();
}

void Led_APA102::flash(ILed::Colour colour, unsigned nums) {
  const unsigned dur = 100;
  set(false);
  while (nums--) {
    set(colour);
    vTaskDelay(pdMS_TO_TICKS(dur / 2));
    set(false);
    if (nums)
      vTaskDelay(pdMS_TO_TICKS(dur));
  }
}
