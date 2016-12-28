/*
  Project Name : Lightbar
  Developer : Eric Klein Jr. (temp2@ericklein.com)
  Description : Control LED strips for simple lighting installations via buttons, rotary encoder
  Last Revision Date : 12/27/16

  Revisions
  10/17/14
  -  fork from example to control LPD8806-based RGB LED Modules in a strip
  -  added multiple button support, will be augmented/replaced by rotary encoder in future version
  11/15/14
  -  prototypes for desired lighting functions (lightXxYy)
  04/21/15
  -  fixed bugs in lightColorFill
  03/02/16
  -  first version in GitHub
  -  added rotary encoder, code is extremely crude
  04/27/16
  -  rotary encoder push is captured
  -  code readability improved, still much to do
  05/22/16
  - changed rotation tracking to boolean
  - fixed wrap-around of intensity bug (03/02/16)
  - moved light test script from loop() to lightTest, no dev work done on function itself
  - hard coded button functionality, now can comment out functionality based on hardware config
  12/27/16
  - completed 05/22/16 revisions
  - switch from LED intensity (one value) to r,g,b levels

  Sources
  -  Uses LPD8806 library from Adafruit https://github.com/adafruit/LPD8806

  Target
  -  built for Arduino Uno
  -  Adafruit Rotary Encoder 377b

  Feature Requests
  -  04/21/15
       -  move light functions to library
       -  why do many functions pass parameters that are not used?
  -  03/02/16
       -  switch from intensity (one value) to r,g,b levels if I want to use colors other than RGBW [COMPLETE 12/27/16]
       -  stop int based wrap around of intensity [COMPLETE 05/22/16]
       -  on/off switch
       -  toggle light function switch
       -  can I use FastLED or other generic library
       -  use rotary encoder push as button [COMPLETE 04/27/16]
       -  code optimization
  -  04/27/16
      -  consistent use of wire color for ground, etc.
		  - int to byte conversation for variables
  -   05/22/16
      - Why do I care about halfLeft and halfRight?
      - Enumerate colors white, red, blue, green? [NOT NEEDED 12/27/16]
  -   12/27/16 (from Evernote)
      - standard connectors for LED strip
      - resistor in front of LED #1

  Light functions
  -  lightRainbow -> 3 cycles of all 384 colors in the wheel
  -  lightRainbowCycle(uint8_t wait) -> modifies LightRainbow to make rainbow wheel equally distributed along the chain
  -  lightColorWipe(uint32_t c, uint8_t wait) -> Fills all LEDs progressively in one color
  -  lightColorFill(uint32_t c)-> Simulataneouly changes the entire strip to one color
  -  lightColorChase(uint32_t c, uint8_t wait) -> Chase one dot down the full strip
  -  lightTheatreChase(uint32_t c, uint8_t wait) -> Theatre-style crawling lights
  -  lightTheatreChaseRainbow(uint8_t wait)-> Modifies lightTheatreChaseRainbow with rainbow effect
  -   lightTest() -> excercises most of these functions to quickly test strip

*/

// Library initialization
#include "LPD8806.h"
#include "SPI.h"

// Assign Arduino pins
#define rotaryEncoder1Pin        2  // rotary encoder rotation
#define rotaryEncoder2Pin        3  // rotary encoder rotation
#define rotaryEncoderButtonPin   4  // rotary encoder button is used to turn LED strip on and off
#define dataPin                  8  // LED strip
#define clockPin                 9  // LED strip
#define colorSelectPin           11  // push button used to select color
#define lightFieldEffectPin      12  // push button used to select light field effects

// Assign global variables
#define stripLength 18              // Number of RGB LEDs on strip
const byte longPressLength = 25;    // Min number of loops for a long press
const byte loopDelay = 20;          // Delay per main loop in ms
int red = 0;
int blue = 0;
int green = 0;
int stripColor = 0; //white = 0; red = 1; green = 2; blue = 3

enum { EV_NONE = 0, EV_SHORTPRESS, EV_LONGPRESS };
volatile boolean rotateClockWise = false;
volatile boolean rotateCounterClockWise = false;
volatile boolean halfleft = false;      // Used in both interrupt routines
volatile boolean halfright = false;

LPD8806 strip = LPD8806(stripLength, dataPin, clockPin);

// You can optionally use hardware SPI for faster writes, just leave out
// the data and clock pin parameters.  But this does limit use to very
// specific pins on the Arduino.  For "classic" Arduinos (Uno, Duemilanove,
// etc.), data = pin 11, clock = pin 13.  For Arduino Mega, data = pin 51,
// clock = pin 52.  For 32u4 Breakout Board+ and Teensy, data = pin B2,
// clock = pin B1.  For Leonardo, this can ONLY be done on the ICSP pins.

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

void ButtonHandler::init()
{
  pinMode(pin, INPUT_PULLUP);
  was_pressed = false;
  pressed_counter = 0;
}

int ButtonHandler::handle()
{
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
ButtonHandler buttonColorSelect(colorSelectPin);
ButtonHandler buttonLightFieldEffect(lightFieldEffectPin);
ButtonHandler buttonOnOff(rotaryEncoderButtonPin);

void buttonEvent(const char* button_name, int event)
{
  //short press on ColorSelect rotates LED strip color
  if ((button_name == "ColorSelect") && (event == 1))
  {
    switch (stripColor)
    {
      case 0: //white -> red
        blue = 0;
        green = 0;
        stripColor = 1;
      break;
      case 1: //red -> green
        green = red;
        red = 0;
        blue = 0;
        stripColor = 2;
              //Serial.println(debugText);
              break;
      case 2: //green -> blue
        blue = green;
        green = 0;
        stripColor = 3;
              //Serial.println(debugText);
              break;
      case 3: //blue -> red
        red = blue;
        blue = 0;
        stripColor = 1;
              //Serial.println(debugText);
              break;
    }
      Serial.print(red);
      Serial.print(green);
      Serial.println(blue);
      lightColorFill(strip.Color(red, green, blue));
      Serial.println("ColorSelect button short press -> cycle color");
  }
  //long press on ColorSelect resets LED strip color to white
  if ((button_name == "ColorSelect") && (event == 2))
  {
   switch (stripColor)
    {
      case 1: //red -> white
        green = red;
        blue = red;
      case 2: //green -> white
        blue = green;
        red = green;
      case 3: //blue -> red
        green = red;
    }
    stripColor = 0;
      lightColorFill(strip.Color(red, green, blue));
    Serial.println("ColorSelect button long press -> reset LED strip color to white");
  }

  //short press on LightFieldEffect does ???
  if ((button_name == "LightFieldEffect") && (event == 1))
  {
        Serial.println("LightFieldEffect button short press"); //debug text
  }

  //long press on LightFunction does ???
  if ((button_name == "LightFieldEffect") && (event == 2))
  {
        lightTest();
        Serial.println("LightFieldEffect button long press"); //debug text
  }
  
  //short press on OnOff turns the strip on and off
  if ((button_name == "OnOff") && (event == 1))
  {
    Serial.println("Encoder button short press"); //debug text
  }
}

void setup() {
  Serial.begin(9600);
  // Setup LED strip
  strip.begin();
  lightColorFill(strip.Color(red, green, blue)); // initialize the LED strip
red = 8;
blue = 8;
green = 8;
  // Setup push buttons
  buttonColorSelect.init();
  buttonLightFieldEffect.init();
  buttonOnOff.init();
  // Setup rotary encoder
  pinMode(rotaryEncoder1Pin, INPUT_PULLUP);
  pinMode(rotaryEncoder2Pin, INPUT_PULLUP);
  attachInterrupt(0, checkRE1Pin, FALLING);   // Call checkRE1Pin when digital pin 2 goes LOW
  attachInterrupt(1, checkRE2Pin, FALLING);   // Call checkRE2Pin when digital pin 3 goes LOW
}

void loop()
{
  // read buttons for events
  int event1 = buttonColorSelect.handle();
  int event2 = buttonLightFieldEffect.handle();
  int event3 = buttonOnOff.handle();

  // deal with button events
  buttonEvent("ColorSelect", event1);
  buttonEvent("LightFieldEffect", event2);
  buttonEvent("OnOff", event3);

  // deal with encoder rotation
  if (rotateClockWise)
  {
    if ((red<113) && (green<113)&& (blue<113)) // light not at max brightness
    {
    switch (stripColor)
    {
      case 0:
        red +=8;
        green +=8;
        blue +=8;
        break;
      case 1: 
      red +=8;
      break;
      case 2: 
      green +=8;
      break;
      case 3:
      blue +=8;
      break;
    }
    lightColorFill(strip.Color(red, green, blue));
    rotateClockWise = false; //reset rotation status
      Serial.print(red);
      Serial.print(green);
      Serial.println(blue);    
    Serial.println("increase brightness of current color");
    }
    else
    {Serial.print(red);
      Serial.print(green);
      Serial.println(blue);   
      Serial.println("increase not possible");
          rotateClockWise = false; //reset rotation status
    }
  }
  if (rotateCounterClockWise)
  {
    if ((red>15) || (green>15)|| (blue>15)) // light not at minimum brighness
    {
    switch (stripColor)
    {
            case 0:
        red -=8;
        green -=8;
        blue -=8;
        break;
        case 1: 
      red -=8;
      break;
      case 2: 
      green -=8;
      break;
      case 3:
      blue -=8;
      break;
    }
    lightColorFill(strip.Color(red, green, blue));
    rotateCounterClockWise = false; //reset rotation status
          Serial.print(red);
      Serial.print(green);
      Serial.println(blue);    
    Serial.println("decrease brightness of current color");
    }
    else
    {
          Serial.print(red);
      Serial.print(green);
      Serial.println(blue);   
      Serial.println("decrease not possible");
          rotateCounterClockWise = false; //reset rotation status

    }
  }
  delay(loopDelay);
}

void lightTest()
{
    lightColorChase(strip.Color(127, 127, 127), 50); // White
    lightColorChase(strip.Color(127,   0,   0), 50); // Red
    lightColorChase(strip.Color(127, 127,   0), 50); // Yellow
    lightColorChase(strip.Color(  0, 127,   0), 50); // Green
    lightColorChase(strip.Color(  0, 127, 127), 50); // Cyan
    lightColorChase(strip.Color(  0,   0, 127), 50); // Blue
    lightColorChase(strip.Color(127,   0, 127), 50); // Violet
  
    lightTheatreChase(strip.Color(127, 127, 127), 50); // White
    lightTheatreChase(strip.Color(127,   0,   0), 50); // Red
    lightTheatreChase(strip.Color(127, 127,   0), 50); // Yellow
    lightTheatreChase(strip.Color(  0, 127,   0), 50); // Green
    lightTheatreChase(strip.Color(  0, 127, 127), 50); // Cyan
    lightTheatreChase(strip.Color(  0,   0, 127), 50); // Blue
    lightTheatreChase(strip.Color(127,   0, 127), 50); // Violet
  
    lightColorWipe(strip.Color(127,   0,   0), 50);  // Red
    lightColorWipe(strip.Color(  0, 127,   0), 50);  // Green
    lightColorWipe(strip.Color(  0,   0, 127), 50);  // Blue
  
    lightRainbow(10);
    lightRainbowCycle(0);  // make it go through the cycle fairly fast
    lightTheatreChaseRainbow(50);
}

void lightRainbow(uint8_t wait)
// 3 cycles of all 384 colors in the wheel
{
  int i, j;

  for (j = 0; j < 384; j++)
  {
    for (i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel( (i + j) % 384));
    }
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

void lightRainbowCycle(uint8_t wait)
// modifies LightRainbow to make rainbow wheel equally distributed along the chain
{
  uint16_t i, j;

  for (j = 0; j < 384 * 5; j++) {   // 5 cycles of all 384 colors in the wheel
    for (i = 0; i < strip.numPixels(); i++) {
      // tricky math! we use each pixel as a fraction of the full 384-color wheel
      // (thats the i / strip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 384 is to make the wheel cycle around
      strip.setPixelColor(i, Wheel( ((i * 384 / strip.numPixels()) + j) % 384) );
    }
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

void lightColorWipe(uint32_t c, uint8_t wait)
// Fills all LEDs progressively in one color
{
  int i;

  for (i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void lightColorFill(uint32_t c)
// Simulataneouly changes the entire strip to one color
{
  int i;

  for (i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
  }
  strip.show();
}

void lightColorChase(uint32_t c, uint8_t wait)
// Chase one dot down the full strip
{
  int i;

  // Start by turning all pixels off:
  for (i = 0; i < strip.numPixels(); i++) strip.setPixelColor(i, 0);

  // Then display one pixel at a time:
  for (i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c); // Set new pixel 'on'
    strip.show();              // Refresh LED states
    strip.setPixelColor(i, 0); // Erase pixel, but don't refresh!
    delay(wait);
  }
  strip.show(); // Refresh to turn off last pixel
}

void lightTheatreChase(uint32_t c, uint8_t wait)
// Theatre-style crawling lights
{
  for (int j = 0; j < 10; j++) { //do 10 cycles of chasing
    for (int q = 0; q < 3; q++) {
      for (int i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, c);  //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}

void lightTheatreChaseRainbow(uint8_t wait)
//Modifies lightTheatreChaseRainbow with rainbow effect
{
  for (int j = 0; j < 384; j++) {   // cycle all 384 colors in the wheel
    for (int q = 0; q < 3; q++) {
      for (int i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, Wheel( (i + j) % 384)); //turn every third pixel on
      }
      strip.show();

      delay(wait);

      for (int i = 0; i < strip.numPixels(); i = i + 3) {
        strip.setPixelColor(i + q, 0);      //turn every third pixel off
      }
    }
  }
}

// Helper functions for lighting

uint32_t Wheel(uint16_t WheelPos)
//Input a value 0 to 384 to get a color value. The colours are a transition r - g -b - back to r
{
  byte r, g, b;
  switch (WheelPos / 128)
  {
    case 0:
      r = 127 - WheelPos % 128;   //Red down
      g = WheelPos % 128;      // Green up
      b = 0;                  //blue off
      break;
    case 1:
      g = 127 - WheelPos % 128;  //green down
      b = WheelPos % 128;      //blue up
      r = 0;                  //red off
      break;
    case 2:
      b = 127 - WheelPos % 128;  //blue down
      r = WheelPos % 128;      //red up
      g = 0;                  //green off
      break;
  }
  return (strip.Color(r, g, b));
}

void checkRE1Pin() {                                             // Pin2 went LOW
  delay(1);                                                // Debounce time
  if (digitalRead(rotaryEncoder1Pin) == LOW) {                             // Pin2 still LOW ?
    if (digitalRead(rotaryEncoder2Pin) == HIGH && halfright == false) {    // -->
      halfright = true;                                    // One half click clockwise
    }
    if (digitalRead(rotaryEncoder2Pin) == LOW && halfleft == true) {       // <--
      halfleft = false;                                    // One whole click counter-
      rotateCounterClockWise = true;                                            // clockwise
    }
  }
}
void checkRE2Pin() {                                            // Pin3 went LOW
  delay(1);                                               // Debounce time
  if (digitalRead(rotaryEncoder2Pin) == LOW) {                            // Pin3 still LOW ?
    if (digitalRead(rotaryEncoder1Pin) == HIGH && halfleft == false) {    // <--
      halfleft = true;                                    // One half  click counter-
    }                                                     // clockwise
    if (digitalRead(rotaryEncoder1Pin) == LOW && halfright == true) {     // -->
      halfright = false;                                  // One whole click clockwise
      rotateClockWise = true;
    }
  }
}
