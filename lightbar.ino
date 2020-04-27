/*
  Project Name:   Lightbar
  Developer:      Eric Klein Jr. (temp2@ericklein.com)
  Description:    Control LED strips for simple lighting installations via buttons, rotary encoder

  See README.md for target information, revision history, feature requests, etc.
*/

// Conditional code
//#define blinkytape
#define DEBUG
#define RGBWSupport

// Library initialization

#ifdef RGBWSupport
  #include <Adafruit_NeoPixel.h>
#else
  #include <FastLED.h>
#endif
#include <buttonhandler.h>
#include <Encoder.h>

// LED hardware
#ifdef blinkytape
  #define ledCount      60  // BlinkyTape has 60 LEDs
#else
  #define ledCount      17  // Number of RGB LEDs on strip
#endif
#define ledDataPin      12  // for one wire LED strips
//#define ledClockPin   12  // for two wire LED strips
#ifdef RGBWSupport
  Adafruit_NeoPixel adafruitStrip(ledCount, ledDataPin, NEO_GRBW + NEO_KHZ800);
#else
  struct CRGB strip[ledCount]; 
#endif
// LED globals
#define brightnessStepChange  2
#define stripMinBrightness    6
#define stripMaxBrightness    248
int stripColor = 0;
enum {white = 0, red, green, blue};
int stripBrightness =         10;

// Rotary encoder hardware
#ifndef blinkytape
  // these specific pins for most AVR hardware
  #define rotaryEncoder1Pin        2      // expected to be counter-clockwise
  #define rotaryEncoder2Pin        3      // expected to be clockwise
  // globals related to rotary encoder
  //boolean rotateClockWise = false;
  //boolean rotateCounterClockWise = false;
  Encoder rotaryEncoder(2, 3);
  long encoderPosition = 0; 
#endif

// button hardware
#ifndef blinkytape
    // support for extra buttons
    #define colorSelectPin          11  // push button used to select color
    #define rotaryEncoderButtonPin  7
#endif
// assumes we will use the same pin that blinkytape is hard wired for
#define lightFieldEffectPin         9  // push button used to select light field effects
#define buttonLongPressDelay        2500

// globals related to buttons
enum { BTN_NOPRESS = 0, BTN_SHORTPRESS, BTN_LONGPRESS };
// Instantiate button objects
#ifndef blinkytape
  // initialize extra buttons
  ButtonHandler buttonColorSelect(colorSelectPin, buttonLongPressDelay);
  ButtonHandler buttonOnOff(rotaryEncoderButtonPin, buttonLongPressDelay);
#endif
ButtonHandler buttonLightFieldEffect(lightFieldEffectPin, buttonLongPressDelay);

void setup() 
{
  Serial.begin(115200);
  while (!Serial) ;
  #ifdef DEBUG
    Serial.println("Lightbar started");
  #endif

  // setup LED strip
  #ifdef blinkytape
    FastLED.addLeds<WS2811, dataPin, GRB>(ledStrip, ledCount);
    // Setup push buttons
  #endif
  #ifdef RGBWSupport
    adafruitStrip.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
    adafruitStrip.setBrightness(stripBrightness);
    adafruitStrip.fill(adafruitStrip.Color(0,0,0,255),0,ledCount);
    adafruitStrip.show();
  #else
    FastLED.addLeds<LPD8806, dataPin, clockPin, GRB>(ledStrip, ledCount);
    FastLED.clear();
  #endif

  // Setup push buttons
  #ifndef blinkytape
    buttonColorSelect.init();
    buttonOnOff.init();
  #endif

  buttonLightFieldEffect.init();  
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

void resolveRotaryEncoders()
{
  long newEncoderPosition;
  newEncoderPosition = rotaryEncoder.read();
  #ifdef DEBUG
    if (newEncoderPosition != encoderPosition)
    {
      Serial.print("Old position ");
      Serial.print(encoderPosition);
      Serial.print(", new position ");
      Serial.println(newEncoderPosition);
    }
  #endif
  if (newEncoderPosition != encoderPosition)
  {
    stripBrightness = stripBrightness+((newEncoderPosition - encoderPosition) * brightnessStepChange);
    stripBrightness = constrain(stripBrightness, stripMinBrightness , stripMaxBrightness);
    #ifdef RGBWSupport
    adafruitStrip.setBrightness(stripBrightness);
    adafruitStrip.show();
    #else
      FastLED.setBrightness(stripBrightness);
      FastLED.show();
    #endif
    #ifdef DEBUG
      Serial.print("Brightness changed to ");
      Serial.println(stripBrightness);
    #endif
  }
  encoderPosition = newEncoderPosition;
}

void resolveButtons()
{
  // resolve lightFieldEffect button
  switch (buttonLightFieldEffect.handle())
  {
    case BTN_SHORTPRESS:
      #ifdef DEBUG
        Serial.println("LightFieldEffect button short press");
      #endif
      break;
    case BTN_LONGPRESS:
      #ifdef DEBUG
        Serial.println("LightFieldEffect button long press");
      #endif
      break;
  }

  #ifndef blinkytape
  // resolve ColorSelect button
  switch (buttonColorSelect.handle()) {
    case BTN_SHORTPRESS:
      switch (stripColor)
      {
        case white: // white -> red
          #ifdef DEBUG
            Serial.print("ColorSelect button short press -> cycle to red at brightness ");
            Serial.println(stripBrightness);
          #endif
          #ifdef RGBWSupport
            adafruitStrip.fill(adafruitStrip.Color(255,0,0,0),0,ledCount);
            adafruitStrip.show();
          #else
            fill_solid(ledStrip, ledCount, CRGB::Red);
            FastLED.show();
          #endif
          stripColor = red;
          break;
        case red: // red -> green
          #ifdef DEBUG
            Serial.print("ColorSelect button short press -> cycle to green at brightness ");
            Serial.println(stripBrightness);
          #endif
          #ifdef RGBWSupport
            adafruitStrip.fill(adafruitStrip.Color(0,255,0,0),0,ledCount);
            adafruitStrip.show();
          #else
            fill_solid(ledStrip, ledCount, CRGB::Green);
            FastLED.show();
          #endif
          stripColor = green;
          break;
        case green: // green -> blue
          #ifdef DEBUG
            Serial.print("ColorSelect button short press -> green to blue at brightness ");
            Serial.println(stripBrightness);
          #endif
          #ifdef RGBWSupport
            adafruitStrip.fill(adafruitStrip.Color(0,0,255,0),0,ledCount);
            adafruitStrip.show();
          #else
            fill_solid(ledStrip, ledCount, CRGB::BLUE);
            FastLED.show();
          #endif
          stripColor = blue;
          break;
        case blue: // blue -> white
          #ifdef DEBUG
            Serial.print("ColorSelect button short press -> blue to white at brightness ");
            Serial.println(stripBrightness);
          #endif
          #ifdef RGBWSupport
            adafruitStrip.fill(adafruitStrip.Color(255,255,255,0),0,ledCount);
            adafruitStrip.show();
          #else
            fill_solid(ledStrip, ledCount, CRGB::White);
            FastLED.show();
          #endif
          stripColor = white;
          break;
      }
    break;
    case BTN_LONGPRESS:
      switch (stripColor)
      {
        case white:
          #ifdef DEBUG
            Serial.print("ColorSelect button long press -> white + white LED at brightness ");
            Serial.println(stripBrightness);
          #endif
          #ifdef RGBWSupport
            adafruitStrip.fill(adafruitStrip.Color(255,255,255,255),0,ledCount);
            adafruitStrip.show();
          #else
            #ifdef DEBUG
              Serial.println("ColorSelect button long press -> function TBD");
            #endif
          #endif
          break;
        case red: 
          #ifdef DEBUG
            Serial.print("ColorSelect button long press -> red + white LED at brightness ");
            Serial.println(stripBrightness);
          #endif
          #ifdef RGBWSupport
            adafruitStrip.fill(adafruitStrip.Color(255,0,0,255),0,ledCount);
            adafruitStrip.show();
          #else
            #ifdef DEBUG
              Serial.println("ColorSelect button long press -> function TBD");
            #endif
          #endif
          break;
        case green: 
          #ifdef DEBUG
            Serial.print("ColorSelect button long press -> green + white LED at brightness ");
            Serial.println(stripBrightness);
          #endif
          #ifdef RGBWSupport
            adafruitStrip.fill(adafruitStrip.Color(0,255,0,255),0,ledCount);
            adafruitStrip.show();
          #else
            #ifdef DEBUG
              Serial.println("ColorSelect button long press -> function TBD");
            #endif
          #endif
          break;
        case blue:
          #ifdef DEBUG
            Serial.print("ColorSelect button long press -> blue + white LED at brightness ");
            Serial.println(stripBrightness);
          #endif
          #ifdef RGBWSupport
            adafruitStrip.fill(adafruitStrip.Color(0,0,255,255),0,ledCount);
            adafruitStrip.show();
          #else
            #ifdef DEBUG
              Serial.println("ColorSelect button long press -> function TBD");
            #endif      
          #endif
          break;
      }
    break;
  }

  // resolve buttonOnOff button
  switch (buttonOnOff.handle()) 
  {
    case BTN_SHORTPRESS:
      #ifdef DEBUG
        Serial.println("Encoder button short press -> turn strip on if off");
      #endif
      break;
    case BTN_LONGPRESS:
      #ifdef DEBUG
        Serial.println("Encoder button long press -> turn strip off");
      #endif
      break;
  }
  #endif
}