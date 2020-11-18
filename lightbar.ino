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
#else   // in every other case we're going to use FastLED
  #include <FastLED.h>
#endif

#include <buttonhandler.h>
#include <Encoder.h>

// LED

#define ledCount      17  // Number of RGB LEDs on strip

#ifdef blinkytape
  #define ledCount      60  // BlinkyTape has 60 LEDs
#endif
#define ledDataPin      12  // for one wire LED strips
//#define ledClockPin   12  // for two wire LED strips

#ifdef RGBWSupport
  Adafruit_NeoPixel strip(ledCount, ledDataPin, NEO_GRBW + NEO_KHZ800);
#else
  struct CRGB strip[ledCount]; 
#endif

#define brightnessStepChange  2
#define stripMinBrightness    6
#define stripMaxBrightness    248
int stripColor = 0;
enum {white = 0, red, green, blue};
int stripBrightness =         10;
int whiteValue = 2;
bool stripPower = true;

// Rotary encoder
#ifndef blinkytape
  // these specific pins for most AVR hardware
  #define rotaryEncoderOnePin1    2      // expected to be counter-clockwise
  #define rotaryEncoderOnePin2    3      // expected to be clockwise
  #define rotaryEncoderTwoPin1    4
  #define rotaryEncoderTwoPin2    5
  Encoder rotaryEncoderOne(2, 3);
  Encoder rotaryEncoderTwo(4,5);
  long encoderOnePosition = 0;
  long encoderTwoPosition = 0;
#endif

// button hardware
#ifndef blinkytape
  // support for extra buttons
  #define colorSelectPin          11  // push button used to select color
  #define rotaryEncoderButtonPin  7
#endif
// assumes we will use the same pin that blinkytape is hard wired for
//#define lightFieldEffectPin         9  // push button used to select light field effects
#define buttonLongPressDelay        2500

// globals related to buttons
enum { BTN_NOPRESS = 0, BTN_SHORTPRESS, BTN_LONGPRESS };
// Instantiate button objects
#ifndef blinkytape
  // initialize extra buttons
  ButtonHandler buttonColorSelect(rotaryEncoderButtonPin, buttonLongPressDelay);
  ButtonHandler buttonOnOff(colorSelectPin, buttonLongPressDelay);
#endif
//ButtonHandler buttonLightFieldEffect(lightFieldEffectPin, buttonLongPressDelay);

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
    strip.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
    strip.setBrightness(stripBrightness);
    strip.fill(strip.Color(0,0,0,255),0,ledCount);
    strip.show();
  #else
    FastLED.addLeds<LPD8806, dataPin, clockPin, GRB>(ledStrip, ledCount);
    FastLED.clear();
  #endif

  // Setup push buttons
  #ifndef blinkytape
    buttonColorSelect.init();
    buttonOnOff.init();
  #endif

  //buttonLightFieldEffect.init();  
}

void loop()
{
  #ifdef blinkytape
    resolveButtons();
  #else
    resolveButtons();
    resolveRotaryEncoderOne();
    resolveRotaryEncoderTwo();
  #endif
}

void resolveRotaryEncoderOne()
{
  long newEncoderPosition;
  newEncoderPosition = rotaryEncoderOne.read();
  #ifdef DEBUG
    if (newEncoderPosition != encoderOnePosition)
    {
      Serial.print("Encoder One old position ");
      Serial.print(encoderOnePosition);
      Serial.print(", new position ");
      Serial.println(newEncoderPosition);
    }
  #endif
  if (newEncoderPosition != encoderOnePosition)
  {
    stripBrightness = stripBrightness+((newEncoderPosition - encoderOnePosition) * brightnessStepChange);
    stripBrightness = constrain(stripBrightness, stripMinBrightness , stripMaxBrightness);
    #ifdef RGBWSupport
    strip.setBrightness(stripBrightness);
    strip.show();
    #else
      FastLED.setBrightness(stripBrightness);
      FastLED.show();
    #endif
    #ifdef DEBUG
      Serial.print("Brightness changed to ");
      Serial.println(stripBrightness);
    #endif
  }
  encoderOnePosition = newEncoderPosition;
}

void resolveRotaryEncoderTwo()
{
  long newEncoderPosition;
  newEncoderPosition = rotaryEncoderTwo.read();
  #ifdef DEBUG
    if (newEncoderPosition != encoderTwoPosition)
    {
      Serial.print("Encoder Two old position ");
      Serial.print(encoderTwoPosition);
      Serial.print(", new position ");
      Serial.println(newEncoderPosition);
    }
  #endif
  if (newEncoderPosition != encoderTwoPosition)
  {
    whiteValue = whiteValue+((newEncoderPosition - encoderTwoPosition) * brightnessStepChange);
    whiteValue = constrain(whiteValue, stripMinBrightness , stripMaxBrightness);
    #ifdef RGBWSupport
      switch (stripColor)
      {
        case white:
            strip.fill(strip.Color(255,255,255,whiteValue),0,ledCount);
          break;
        case red:
          strip.fill(strip.Color(255,0,0,whiteValue),0,ledCount);
          break;
        case green:
          strip.fill(strip.Color(0,255,0,whiteValue),0,ledCount);
          break;
        case blue:
          strip.fill(strip.Color(0,0,255,whiteValue),0,ledCount);
          break;
      }
      strip.show();
    #else
      //;
    #endif
    #ifdef DEBUG
      Serial.print("W LED value = ");
      Serial.println(whiteValue);
    #endif
  }
  encoderTwoPosition = newEncoderPosition;
}

void resolveButtons()
{
  // resolve lightFieldEffect button
  // switch (buttonLightFieldEffect.handle())
  // {
  //   case BTN_SHORTPRESS:
  //     #ifdef DEBUG
  //       Serial.println("LightFieldEffect button short press");
  //     #endif
  //     break;
  //   case BTN_LONGPRESS:
  //     #ifdef DEBUG
  //       Serial.println("LightFieldEffect button long press");
  //     #endif
  //     break;
  // }

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
            strip.fill(strip.Color(255,0,0,0),0,ledCount);
            strip.show();
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
            strip.fill(strip.Color(0,255,0,0),0,ledCount);
            strip.show();
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
            strip.fill(strip.Color(0,0,255,0),0,ledCount);
            strip.show();
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
            strip.fill(strip.Color(255,255,255,0),0,ledCount);
            strip.show();
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
            strip.fill(strip.Color(255,255,255,255),0,ledCount);
            strip.show();
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
            strip.fill(strip.Color(255,0,0,255),0,ledCount);
            strip.show();
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
            strip.fill(strip.Color(0,255,0,255),0,ledCount);
            strip.show();
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
            strip.fill(strip.Color(0,0,255,255),0,ledCount);
            strip.show();
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
        Serial.println("Encoder button long press -> ???");
      #endif
      if (stripPower == true)
      {
        #ifdef RGBWSupport
        strip.setBrightness(0);
        strip.show();
        stripPower = false;
        #endif
      }
      else
      {
        #ifdef RGBWSupport
        strip.setBrightness(stripBrightness);
        strip.show();
        stripPower = true;
        #endif
      }
      break;
  }
  #endif
}