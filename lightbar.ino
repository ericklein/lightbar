/*
  Project Name:   lightbox
  Developer:      Eric Klein Jr. (temp2@ericklein.com)
  Description:    Control LED strips for easyGrade card photo capture box

  See README.md for target information, revision history, feature requests, etc.
*/

// hardware and internet configuration parameters
#include "config.h"

// Library initialization
#ifdef RGBWSupport
  #include <Adafruit_NeoPixel.h>
#else  // in every other case use FastLED
  #include <FastLED.h>
#endif

#include <buttonhandler.h>
#include <Encoder.h>

#ifdef RGBWSupport
  Adafruit_NeoPixel strip(ledCount, ledDataPin, NEO_GRBW + NEO_KHZ800);
  #else
  struct CRGB ledStrip[ledCount];
#endif

// LED globals
int ledStripBrightness = 10;
int activeLEDBank = 0;
// bank states associated with activeLEDBank
enum { noLEDs = 0,
       backLEDs,
       rightLEDs,
       frontLEDs,
       leftLEDs,
       allLEDs };

// Rotary encoder
#ifndef blinkytape
// initalization
Encoder rotaryEncoderOne(rotaryEncoderOnePin1, rotaryEncoderOnePin2);
// global variables
long encoderOnePosition = 0;
#endif

// Instantiate button objects
// common button to all configurations
#define buttonLongPressDelay 2500
ButtonHandler buttonOne(buttonOnePin, buttonLongPressDelay);
// button states
enum { BTN_NOPRESS = 0,
       BTN_SHORTPRESS,
       BTN_LONGPRESS };

void setup() {
#ifdef DEBUG
  Serial.begin(115200);
  // wait for serial port connection
  while (!Serial)
    ;

  debugMessage("Lightbox started");
#endif

// setup LED strip
#ifdef blinkytape
  FastLED.addLeds<WS2811, ledDataPin, GRB>(ledStrip, ledCount);
#else
#ifdef RGBWSupport
  strip.begin();
  strip.setBrightness(ledStripMinBrightness);
  strip.show();  // Initialize all pixels to 'off'
#else
  // LPD8806
  //FastLED.addLeds<LPD8806, ledDataPin, ledClockPin, GRB>(ledStrip, ledCount);
  // WS2812b
  FastLED.addLeds<NEOPIXEL, ledDataPin>(ledStrip, ledCount);  // GRB ordering is assumed
  FastLED.setBrightness(ledStripMinBrightness);
  FastLED.clear();
#endif
#endif

  // Setup push button(s)
  buttonOne.init();
}

void loop() {
  resolveButtons();
#ifndef blinkytape
  resolveRotaryEncoderOne();
#endif
}

void resolveRotaryEncoderOne()
{
  long newEncoderPosition;
  newEncoderPosition = rotaryEncoderOne.read() >> 1;
  if (newEncoderPosition != encoderOnePosition) 
  {
    debugMessage(String("encoder 1 old position ") + encoderOnePosition + ",new " + newEncoderPosition);
    //debugMessage(String("encoder 1 new position ") + newEncoderPosition);
    ledStripBrightness = ledStripBrightness + ((newEncoderPosition - encoderOnePosition) * ledStripBrightnessStepChange);
    ledStripBrightness = constrain(ledStripBrightness, ledStripMinBrightness, ledStripMaxBrightness);
    changeLEDStripBrightness();
    encoderOnePosition = newEncoderPosition;
  }
}

void changeLEDStripBrightness()
{
  #ifdef RGBWSupport
    strip.setBrightness(ledStripBrightness);
    strip.show();
  #else
    FastLED.setBrightness(ledStripBrightness);
    FastLED.show();
    #endif
  debugMessage(String("Brightness changed to ") + ledStripBrightness);
}

void resolveButtons() 
{
  // turn on and off LED banks
  switch (buttonOne.handle()) 
  {
    case BTN_SHORTPRESS:
      debugMessage("button one short press");
      changeLEDBank();
      break;
    case BTN_LONGPRESS:
      debugMessage("LightFieldEffect button long press");
      break;
  }
}

void changeLEDBank()
{
  switch (activeLEDBank) 
  {
    case noLEDs:
      // turn on back bank
      debugMessage("turn on back bank of LEDs");
      for (int range = (activeLEDBank * ledsPerBank); range < ((activeLEDBank * ledsPerBank) + ledsPerBank); range++) {
        debugMessage(String("white LED # ") + range);
        ledStrip[range] = CRGB::White;
      }
      FastLED.show();
      break;
    case backLEDs:
    case rightLEDs:
    case frontLEDs:
      // turn off previous LED bank
      debugMessage("turn off previous LED bank");
      for (int range = ((activeLEDBank - 1) * ledsPerBank); range < (((activeLEDBank - 1) * ledsPerBank) + ledsPerBank); range++) {
        debugMessage(String("turn off LED # ") + range);
        ledStrip[range] = CRGB::Black;
      }
      // turn on next LED bank
      debugMessage("turn on next LED bank");
      for (int range = (activeLEDBank * ledsPerBank); range < ((activeLEDBank * ledsPerBank) + ledsPerBank); range++) {
        debugMessage(String("white LED # ") + range);
        ledStrip[range] = CRGB::White;
      }
      FastLED.show();
      break;
    case leftLEDs:
      // turn on all LEDs
      debugMessage("turn on all LED banks");
      fill_solid(ledStrip, ledCount, CRGB::White);
      FastLED.show();
      break;
    case allLEDs:
      // turn off LEDs
      debugMessage("turn off all LED banks");
      FastLED.clear();
      FastLED.show();
      break;
  }
  if (activeLEDBank == allLEDs)
    activeLEDBank = noLEDs;
  else
    activeLEDBank++;
  debugMessage(String("Current LED bank is now ") + activeLEDBank);
}

void debugMessage(String messageText)
// wraps Serial.println as #define conditional
{
#ifdef DEBUG
  Serial.println(messageText);
  Serial.flush();  // Make sure the message gets output (before any sleeping...)
#endif
}