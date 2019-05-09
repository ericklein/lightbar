/*
  Project Name:   Lightbar
  Developer:      Eric Klein Jr. (temp2@ericklein.com)
  Description:    Control LED strips for simple lighting installations via buttons, rotary encoder

  See README.md for target information, revision history, feature requests, etc.
*/

// Library initialization
#include <FastLED.h>
#include <buttonhandler.h>

// Conditional code
#define blinkytape

// Assign hardware specific information
#ifdef blinkytape
  #define dataPin                  13 // LED strip
  #define lightFieldEffectPin      10 // push button used to select light field effects
  #define NUM_LEDS                 60 // BlinkyTape has 60 LEDs
#else
  #define rotaryEncoder1Pin        2  // rotary encoder rotation
  #define rotaryEncoder2Pin        3  // rotary encoder rotation
  #define rotaryEncoderButtonPin   4  // rotary encoder button is used to turn LED strip on and off
  #define dataPin                  11 // LED strip
  #define clockPin                 13 // LED strip
  #define colorSelectPin           10 // push button used to select color
  #define lightFieldEffectPin      9  // push button used to select light field effects
  #define NUM_LEDS                 18 // Number of RGB LEDs on strip
#endif

// independent LED globals
int stripColor = 0; //off = 0; white = 1; red = 2; green = 3; blue = 4
struct CRGB leds[NUM_LEDS]; 

// globals related to buttons
enum { BTN_NOPRESS = 0, BTN_SHORTPRESS, BTN_LONGPRESS };
int stripBrightness = 32;

#ifndef blinkytape
// globals related to rotary encoder
volatile boolean rotateClockWise = false;
volatile boolean rotateCounterClockWise = false;
volatile boolean halfleft = false;      // Used in both interrupt routines
volatile boolean halfright = false;
#endif

// Instantiate button objects
#ifdef blinkytape
ButtonHandler buttonLightFieldEffect(lightFieldEffectPin, 20000);
#else
ButtonHandler buttonColorSelect(colorSelectPin, 20000);
ButtonHandler buttonLightFieldEffect(lightFieldEffectPin, 20000);
ButtonHandler buttonOnOff(rotaryEncoderButtonPin, 20000);
#endif

void setup() 
{
    Serial.begin(115200);
  while (!Serial) ; 

  #ifdef blinkytape
    FastLED.addLeds<WS2811, dataPin, GRB>(leds, NUM_LEDS);
    // Setup push buttons
    buttonLightFieldEffect.init();  
  #else
    FastLED.addLeds<LPD8806, dataPin, clockPin, RGB>(leds, NUM_LEDS);
    // Setup push buttons
    buttonColorSelect.init();
    buttonLightFieldEffect.init();
    buttonOnOff.init();
    // Setup rotary encoder
    pinMode(rotaryEncoder1Pin, INPUT_PULLUP);
    pinMode(rotaryEncoder2Pin, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(rotaryEncoder1Pin), checkRE1Pin, FALLING);   // Call checkRE1Pin when digital pin 2 goes LOW
    attachInterrupt(digitalPinToInterrupt(rotaryEncoder2Pin), checkRE2Pin, FALLING);   // Call checkRE2Pin when digital pin 3 goes LOW
  #endif
  FastLED.clear();
  FastLED.setBrightness(stripBrightness);
  FastLED.show();
}

void loop()
{
  #ifdef blinkytape
    resolveButtons();
  #else
    resolveButtons();
    resolveRotaryEncoders();
  #endif
}

#ifndef blinkytape
void resolveRotaryEncoders(){
  // deal with encoder rotation
  if (rotateClockWise)
  {
    if (stripBrightness<248) // light not at max brightness
    {
      stripBrightness+=8;
      LEDS.setBrightness(stripBrightness);
      LEDS.show();
      rotateClockWise = false; //reset rotation status
            Serial.print("increase brightness of current color to ");
Serial.println(stripBrightness);
    }
    else
    { Serial.println("increase not possible");
      rotateClockWise = false; //reset rotation status
    }
  }
  if (rotateCounterClockWise)
  {
    if (stripBrightness>0) // light not at minimum brighness
    {
      stripBrightness-=8;
      LEDS.setBrightness(stripBrightness);
      LEDS.show();
      rotateCounterClockWise = false; //reset rotation status
      Serial.print("decrease brightness of current color to ");
      Serial.println(stripBrightness);
    }
    else
    {
      Serial.println("decrease not possible");
      rotateCounterClockWise = false; //reset rotation status
    }
  }
}

void checkRE1Pin(){
  delay(1); //debounce
  if (digitalRead(rotaryEncoder1Pin) == LOW) // pin still LOW?
  {
    if (digitalRead(rotaryEncoder2Pin) == HIGH && halfright == false)
    {
      halfright = true; // 1/2 click counter-clockwise
    }
    if (digitalRead(rotaryEncoder2Pin) == LOW && halfleft == true)
    {
      halfleft = false;
      rotateCounterClockWise = true;  // one whole click counter-clockwise
    }
  }
}

void checkRE2Pin(){
  delay(1); // debounce
  if (digitalRead(rotaryEncoder2Pin) == LOW) // pin still LOW?
  {
    if (digitalRead(rotaryEncoder1Pin) == HIGH && halfleft == false)
    {
      halfleft = true; // 1/2 click clockwise
    }
    if (digitalRead(rotaryEncoder1Pin) == LOW && halfright == true)
    {
      halfright = false;
      rotateClockWise = true;  // One whole click clockwise
    }
  }
}
#endif

void resolveButtons(){
#ifdef blinkytape
   // resolve ColorSelect button
   switch (buttonLightFieldEffect.handle()) {
  case BTN_SHORTPRESS:
    Serial.println("LightFieldEffect button short press"); //debug text
    FastLED.clear();
    stripBrightness = 100;
    lightColorChase(CRGB::Red,stripBrightness,50);
    break;
  case BTN_LONGPRESS:
    Serial.println("LightFieldEffect button long press"); //debug text
    lightTest();
    break;
  }

  #else

  // resolve ColorSelect button
  switch (buttonColorSelect.handle()) {
    case BTN_SHORTPRESS:
      switch (stripColor) {
        case 0: // none -> white
          Serial.println("ColorSelect button short press -> cycle to white");
          fill_solid(leds, NUM_LEDS, CRGB::White);
          stripColor = 1;
          break;
        case 1: //white -> red
          Serial.println("ColorSelect button short press -> cycle to red");
          fill_solid(leds, NUM_LEDS, CRGB::Red);
          stripColor = 2;
          break;
        case 2: //red -> green
          fill_solid(leds, NUM_LEDS, CRGB::Green);
          Serial.println("ColorSelect button short press -> cycle to green");
          stripColor = 3;
          break;
        case 3: //green -> blue
          fill_solid(leds, NUM_LEDS, CRGB::Blue);
          Serial.println("ColorSelect button short press -> cycle to blue");
          stripColor = 4;
          break;
        case 4: //blue -> white
          fill_solid(leds, NUM_LEDS, CRGB::White);
          Serial.println("ColorSelect button short press -> cycle to white");
          stripColor = 1;
          break;
      }
      LEDS.show();
    break;
    case BTN_LONGPRESS:
      Serial.println("ColorSelect button long press -> reset LED strip color to white");
      stripColor = 1;
      fill_solid(leds, NUM_LEDS, CRGB::White);
      LEDS.show();
      break;
  }

  // resolve LightFieldEffect button
  switch (buttonLightFieldEffect.handle()) {
    case BTN_SHORTPRESS:
      Serial.println("LightFieldEffect button short press -> Red color chase then clear");
      lightColorChase(CRGB::Red,50);
      LEDS.clear();
      break;
    case BTN_LONGPRESS:
      Serial.println("LightFieldEffect button long press -> LightTest then clear");
      lightTest();
      LEDS.clear();
      break;
  }

    // resolve buttonOnOff button
  switch (buttonOnOff.handle()) {
    case BTN_SHORTPRESS:
      Serial.println("Encoder button short press -> turn strip on if off");
      if (stripColor == 0)
        {
          LEDS.showColor(CRGB::White);
          LEDS.show();
          stripColor = 1;
        }
      break;
    case BTN_LONGPRESS:
      Serial.println("Encoder button long press -> turn strip off");
      LEDS.clear();
      stripColor = 0;
      break;
  }

  #endif
}
