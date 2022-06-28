/*
  Project Name : lightfieldeffects
  Developer : Eric Klein Jr. (temp2@ericklein.com)
  Description : LED strip animation routines
      
  Light functions
    re-write this
*/

void lightTest()
{
// Tests all light functions
  Serial.println("Beginning lightTest");
  
  int stripBrightness = 100;
  debugMessage(String("lightOne Color is Red, brightness is ")+stripBrightness);
  FastLED.clear();
  lightOneColor(CRGB::Red,stripBrightness);
  delay(5000);

  stripBrightness = 150;
  debugMessage(String("lightOneFill Color is Blue, brightness is ")+stripBrightness);
  debugMessage(" fill speed is 50ms, fill percentage is 100");
  FastLED.clear();
  lightOneColorFill(CRGB::Blue, stripBrightness, 50, 100);
  delay(5000);

  stripBrightness = 50;
  debugMessage(String("lightOneColorBlink is Red, brightness is ")+stripBrightness+" blink speed is 500ms");
  FastLED.clear();
  for (int i=0;i<10;i++)
  {
    lightOneColorBlink(CRGB::Red, stripBrightness, 500);
  }
  delay(5000);

  // Test ramp up
  debugMessage("lightOneRamp Color is Green, initial brightness is 50, max brightness is 220, ramp time is 5000ms, only ramp up");
  FastLED.clear();
  lightOneColorRamp(CRGB::Green, 30,200,5000,0);
  delay(5000);

  // Test ramp up and down
  debugMessage("lightOneRamp Color is Blue, initial brightness is 50, max brightness is 240, ramp time is 5000ms, ramp up and down");
  FastLED.clear();
  lightOneColorRamp(CRGB::Blue, 50,240,5000,1);
  delay(5000); 

   // Test lightColorChase
   stripBrightness = 75;
  debugMessage(String("lightOneColorChase is Red, brightness is ")+stripBrightness+"chase delay is 50ms");
  FastLED.clear();
  lightColorChase(CRGB::Red, stripBrightness, 50);
   delay(5000);
   
  stripBrightness = 150;
  debugMessage(String("lightTheatreChase is Green, brightness is ")+stripBrightness+"chase delay is 50ms");
  FastLED.clear();
  lightTheatreChase(CRGB::Green, stripBrightness, 50);
  delay(5000);
   
  debugMessage("Ending LightTest");
  FastLED.clear();
  FastLED.show();
}

void lightOneColor(uint32_t color, int brightness)
{
// All LEDs lit in one color

  FastLED.setBrightness(brightness);
  fill_solid(ledStrip,ledCount, color);
  FastLED.show(); 
}

void lightOneColorFill(uint32_t color, int brightness, int fillSpeed, int fillPercentage)
{
// All LEDs filled progressively in one color

  FastLED.setBrightness(brightness);
  for (int i = 0; i < (ledCount*(fillPercentage/100)); i++) 
    {
      ledStrip[i] = color;
      FastLED.show();
      delay(fillSpeed);
    }
}

void lightOneColorBlink(uint32_t color, int brightness, int blinkCycle)
{
// All LEDs blink in one color

  FastLED.setBrightness(brightness);
  fill_solid(ledStrip,ledCount, color);
  FastLED.show();
  delay(blinkCycle/2);
  fill_solid(ledStrip,ledCount, CRGB::Black);
  FastLED.show();
  delay(blinkCycle/2);
}

void lightOneColorRamp(uint32_t color, int initialBrightness, int maxBrightness, int rampTime, int cycle)
{
// All LEDs cycle from initial to max to initial brightness in one color
  fill_solid(ledStrip, ledCount,color);
  //ramp up to maxBrightness
  for (int i=initialBrightness; i<maxBrightness ;i=i+((maxBrightness-initialBrightness)/10))
  {
    FastLED.setBrightness(i);
    FastLED.show();
    delay(rampTime/10);
  }
  //ramp down to initialBrightness if parameter set
  if (cycle)
  {
    for (int i=maxBrightness; i>initialBrightness ;i=i-((maxBrightness-initialBrightness)/10))
    {
      FastLED.setBrightness(i);
      FastLED.show();
      delay(rampTime/10);
    }
  }
}

void lightColorChase(uint32_t color, int brightness, int wait)
{
// Chase one dot down the full strip
  // display one pixel at a time:
  for (int i = 0; i < ledCount; i++)
    {
      ledStrip[i] = color; // Set new pixel 'on'
      FastLED.show();              // Refresh LED states
      ledStrip[i] = CRGB:: Black; // Erase pixel, but don't refresh!
      delay(wait);
    }
  FastLED.show(); // Refresh to turn off last pixel
}

void lightTheatreChase(uint32_t c, int stripBrightness, int wait)
{
// Theatre-style crawling lights

 for (int j = 0; j < 50; j++) 
  { //do 10 cycles of chasing
    for (int q = 0; q < 3; q++)
      {
        for (int i = 0; i < ledCount; i = i + 3)
          {
            ledStrip[i + q] = c;  //turn every third pixel on
          }
        FastLED.show();
        delay(wait);
        for (int i = 0; i < ledCount; i = i + 3)
          {
            ledStrip[i + q] = CRGB::Black;      //turn every third pixel off
          }
      }
  }
}
