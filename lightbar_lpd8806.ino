/*
Project Name : Lightbar
Developer : Eric Klein Jr. (temp2@ericklein.com)
Description : Light strip with functions for key effects and functionality
Last Revision Date : 03/02/16

Revisions
10/17/14
  -  fork from example to control LPD8806-based RGB LED Modules in a strip
  -  added multiple button support, will be augmented/replaced by rotary encoder in future version        
11/15/14
  -  prototypes for desired lighting functions (lightXxYy)
04/21/15
  -  fixed bugs in lightColorFill
 
Sources
  -  Uses LPD8806 library from Adafruit https://github.com/adafruit/LPD8806
  
Target
  -  built for Arduino Uno
  
Improvements
  -  move light functions to library
  -  4/21/15 - why do many functions pass parameters that are not used?
  
Light functions
  -  lightRainbow -> 3 cycles of all 384 colors in the wheel
  -  lightRainbowCycle(uint8_t wait) -> modifies LightRainbow to make rainbow wheel equally distributed along the chain
  -  lightColorWipe(uint32_t c, uint8_t wait) -> Fills all LEDs progressively in one color
  -  lightColorFill(uint32_t c)-> Simulataneouly changes the entire strip to one color
  -  lightColorChase(uint32_t c, uint8_t wait) -> Chase one dot down the full strip
  -  lightTheatreChase(uint32_t c, uint8_t wait) -> Theatre-style crawling lights
  -  lightTheatreChaseRainbow(uint8_t wait)-> Modifies lightTheatreChaseRainbow with rainbow effect
  
*/

#include "LPD8806.h"
#include "SPI.h"

// Number of RGB LEDs in strand
int nLEDs = 32;

// Output pins, can be any valid output pins
int dataPin  = 8; // SPI data
int clockPin = 9;

LPD8806 strip = LPD8806(nLEDs, dataPin, clockPin);

// You can optionally use hardware SPI for faster writes, just leave out
// the data and clock pin parameters.  But this does limit use to very
// specific pins on the Arduino.  For "classic" Arduinos (Uno, Duemilanove,
// etc.), data = pin 11, clock = pin 13.  For Arduino Mega, data = pin 51,
// clock = pin 52.  For 32u4 Breakout Board+ and Teensy, data = pin B2,
// clock = pin B1.  For Leonardo, this can ONLY be done on the ICSP pins.

#define BUTTON1_PIN               11  // Button 1
#define BUTTON2_PIN               12  // Button 2

#define DEFAULT_LONGPRESS_LEN    25  // Min nr of loops for a long press
#define DELAY                    20  // Delay per loop in ms

//////////////////////////////////////////////////////////////////////////////

enum { EV_NONE=0, EV_SHORTPRESS, EV_LONGPRESS };

//////////////////////////////////////////////////////////////////////////////

int intensity = 40;
int stripcolor = 0;

// Class definition

class ButtonHandler {
  public:
    // Constructor
    ButtonHandler(int pin, int longpress_len=DEFAULT_LONGPRESS_LEN);

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
  pinMode(pin, INPUT);
  digitalWrite(pin, HIGH); // pull-up
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

//////////////////////////////////////////////////////////////////////////////

// Instantiate button objects
ButtonHandler button1(BUTTON1_PIN);
//ButtonHandler button2(BUTTON2_PIN, DEFAULT_LONGPRESS_LEN*2);
ButtonHandler button2(BUTTON2_PIN);

void print_event(const char* button_name, int event)
{
  if ((button_name=="1")&&(event==1))
  {
    lightColorFill(strip.Color(intensity,0,0));  // Red    
    Serial.print("1RG"[event]);
    stripcolor = 1;
    Serial.print(stripcolor);
  }
  
  if ((button_name=="1")&&(event==2))
    {
    lightColorFill(strip.Color(0,intensity,0));  // Green
    Serial.print("1RG"[event]);
    stripcolor = 2;
  }
  
  if ((button_name=="2")&&(event==1))
    {
    lightColorFill(strip.Color(0,0,intensity));  // Blue
    Serial.print("2BY"[event]);
    stripcolor = 3;
  }  
  
  if ((button_name=="2")&&(event==2))
    {
    lightColorFill(strip.Color(intensity,intensity,0));  // Yellow
    Serial.print("2BY"[event]);
    stripcolor = 4;
  }
}

volatile int number = 0;                // Testnumber, print it when it changes value,
                                        // used in loop and both interrupt routines
int oldnumber = number;



volatile boolean halfleft = false;      // Used in both interrupt routines
volatile boolean halfright = false;

void setup() {
  Serial.begin(9600);
  // Start up the LED strip
  strip.begin();
  // Update the strip, to start they are all 'off'
  strip.show();
  button1.init();
  button2.init();
    pinMode(2, INPUT);
  digitalWrite(2, HIGH);                // Turn on internal pullup resistor
  pinMode(3, INPUT);
  digitalWrite(3, HIGH);                // Turn on internal pullup resistor
  attachInterrupt(0, isr_2, FALLING);   // Call isr_2 when digital pin 2 goes LOW
  attachInterrupt(1, isr_3, FALLING);   // Call isr_3 when digital pin 3 goes LOW
}

void loop() {
  
    // handle button
  int event1 = button1.handle();
  int event2 = button2.handle();

  // do other things
  print_event("1", event1);
  print_event("2", event2);
  
  
    if(number > oldnumber)
      {
      Serial.print(stripcolor);
      intensity+=10;
      switch (stripcolor)
      {
        case 1: lightColorFill(strip.Color(intensity,0,0));
          break;
        case 2: lightColorFill(strip.Color(0,intensity,0));
        break;
        case 3: lightColorFill(strip.Color(0,0,intensity));
        break;
        case 4: lightColorFill(strip.Color(intensity,intensity,0));
        break;
        Serial.print(intensity);
      }
      Serial.print(intensity);
      oldnumber = number;
      }
    else if (number < oldnumber)
      {
      Serial.print(stripcolor);
      intensity-=10;    
     switch (stripcolor)
      {
        case 1: lightColorFill(strip.Color(intensity,0,0));
        break;
        case 2: lightColorFill(strip.Color(0,intensity,0));
        break;
        case 3: lightColorFill(strip.Color(0,0,intensity));
        break;
        case 4: lightColorFill(strip.Color(intensity,intensity,0));
        break;
      }
                Serial.print(intensity);
                oldnumber = number;
      }

  delay(DELAY);

//  lightColorChase(strip.Color(127, 127, 127), 50); // White
//  lightColorChase(strip.Color(127,   0,   0), 50); // Red
//  lightColorChase(strip.Color(127, 127,   0), 50); // Yellow
//  lightColorChase(strip.Color(  0, 127,   0), 50); // Green
//  lightColorChase(strip.Color(  0, 127, 127), 50); // Cyan
//  lightColorChase(strip.Color(  0,   0, 127), 50); // Blue
//  lightColorChase(strip.Color(127,   0, 127), 50); // Violet
//
//  lightTheatreChase(strip.Color(127, 127, 127), 50); // White
//  lightTheatreChase(strip.Color(127,   0,   0), 50); // Red
//  lightTheatreChase(strip.Color(127, 127,   0), 50); // Yellow
//  lightTheatreChase(strip.Color(  0, 127,   0), 50); // Green
//  lightTheatreChase(strip.Color(  0, 127, 127), 50); // Cyan
//  lightTheatreChase(strip.Color(  0,   0, 127), 50); // Blue
//  lightTheatreChase(strip.Color(127,   0, 127), 50); // Violet
//
//  lightColorWipe(strip.Color(127,   0,   0), 50);  // Red
//  lightColorWipe(strip.Color(  0, 127,   0), 50);  // Green
//  lightColorWipe(strip.Color(  0,   0, 127), 50);  // Blue
//
//  lightRainbow(10);
//  lightRainbowCycle(0);  // make it go through the cycle fairly fast
//  lightTheatreChaseRainbow(50);
}

void lightRainbow(uint8_t wait)
// 3 cycles of all 384 colors in the wheel
{
  int i, j;
   
  for (j=0; j < 384; j++)
  {
    for (i=0; i < strip.numPixels(); i++) {
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
  
  for (j=0; j < 384 * 5; j++) {     // 5 cycles of all 384 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
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

  for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

void lightColorFill(uint32_t c)
// Simulataneouly changes the entire strip to one color
{
  int i;

  for (i=0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, c);}
      strip.show();
  }

void lightColorChase(uint32_t c, uint8_t wait)
// Chase one dot down the full strip
{
  int i;

  // Start by turning all pixels off:
  for(i=0; i<strip.numPixels(); i++) strip.setPixelColor(i, 0);

  // Then display one pixel at a time:
  for(i=0; i<strip.numPixels(); i++) {
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
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, c);    //turn every third pixel on
      }
      strip.show();
     
      delay(wait);
     
      for (int i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

void lightTheatreChaseRainbow(uint8_t wait)
//Modifies lightTheatreChaseRainbow with rainbow effect
{
  for (int j=0; j < 384; j++) {     // cycle all 384 colors in the wheel
    for (int q=0; q < 3; q++) {
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, Wheel( (i+j) % 384));    //turn every third pixel on
        }
        strip.show();
       
        delay(wait);
       
        for (int i=0; i < strip.numPixels(); i=i+3) {
          strip.setPixelColor(i+q, 0);        //turn every third pixel off
        }
    }
  }
}

// Helper functions for lighting

uint32_t Wheel(uint16_t WheelPos)
//Input a value 0 to 384 to get a color value. The colours are a transition r - g -b - back to r
{
  byte r, g, b;
  switch(WheelPos / 128)
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
  return(strip.Color(r,g,b));
}

void isr_2(){                                              // Pin2 went LOW
  delay(1);                                                // Debounce time
  if(digitalRead(2) == LOW){                               // Pin2 still LOW ?
    if(digitalRead(3) == HIGH && halfright == false){      // -->
      halfright = true;                                    // One half click clockwise
    }  
    if(digitalRead(3) == LOW && halfleft == true){         // <--
      halfleft = false;                                    // One whole click counter-
      number--;                                            // clockwise
    }
  }
}
void isr_3(){                                             // Pin3 went LOW
  delay(1);                                               // Debounce time
  if(digitalRead(3) == LOW){                              // Pin3 still LOW ?
    if(digitalRead(2) == HIGH && halfleft == false){      // <--
      halfleft = true;                                    // One half  click counter-
    }                                                     // clockwise
    if(digitalRead(2) == LOW && halfright == true){       // -->
      halfright = false;                                  // One whole click clockwise
      number++;
    }
  }
}
