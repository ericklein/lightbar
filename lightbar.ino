/*
  Project Name : Lightbar
  Developer : Eric Klein Jr. (temp2@ericklein.com)
  Description : Control LED strips for simple lighting installations via buttons, rotary encoder
  See README.md for target information, revision history, feature requests, etc.
*/

// Library initialization
#include <FastLED.h>

// Conditional code
#define debug
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
int stripBrightness = 32;
int stripColor = 0; //off = 0; white = 1; red = 2; green = 3; blue = 4
struct CRGB leds[NUM_LEDS]; 

// globals related to buttons
enum { EV_NONE = 0, EV_SHORTPRESS, EV_LONGPRESS };
const byte longPressLength = 25;    // Min number of loops for a long press
const byte debounceLength = 20;          // button debounce delay ms

#ifndef blinktape
// globals related to rotary encoder
volatile boolean rotateClockWise = false;
volatile boolean rotateCounterClockWise = false;
volatile boolean halfleft = false;      // Used in both interrupt routines
volatile boolean halfright = false;
#endif

// Class definition
class ButtonHandler {
  public:
    // Constructor
    ButtonHandler(int pin, int longpress_len = longPressLength);

    // Initialization done after construction, to permit static instances
    void init();

    // Handler, to be called in the loop()
    int handle();

  protected:
    boolean was_pressed;     // previous state
    int pressed_counter;     // press running duration
    const int pin;           // pin to which button is connected
    const int longpress_len; // longpress duration
};

ButtonHandler::ButtonHandler(int p, int lp)
  : pin(p), longpress_len(lp)
{
}

void ButtonHandler::init() {
  pinMode(pin, INPUT_PULLUP);
  was_pressed = false;
  pressed_counter = 0;
}

int ButtonHandler::handle(){
  int event;
  int now_pressed = !digitalRead(pin);

  if (!now_pressed && was_pressed) {
    // handle release event
    if (pressed_counter < longpress_len)
      event = EV_SHORTPRESS;
    else
      event = EV_LONGPRESS;
  }
  else
    event = EV_NONE;

  // update press running duration
  if (now_pressed)
    ++pressed_counter;
  else
    pressed_counter = 0;

  // remember state, and we're done
  was_pressed = now_pressed;
  return event;
}

// Instantiate button objects
#ifdef blinkytape
ButtonHandler buttonLightFieldEffect(lightFieldEffectPin);
#else
ButtonHandler buttonColorSelect(colorSelectPin);
ButtonHandler buttonLightFieldEffect(lightFieldEffectPin);
ButtonHandler buttonOnOff(rotaryEncoderButtonPin);
#endif

void setup() 
{
  #ifdef debug
    Serial.begin(57600);
  #endif 

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
