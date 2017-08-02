/*
  Project Name : lightfieldeffects
  Developer : Eric Klein Jr. (temp2@ericklein.com)
  Description : LED strip animation routines
  Last Revision Date : 12/29/16

  Sources
  -  Uses LPD8806 library from Adafruit https://github.com/adafruit/LPD8806

  Target
  -  built for Arduino Uno

  Revisions
  12/29/16
  - latest version in lightbar_lpd8806 removed to create this file
  
  Feature Requests
  -   12/29/16
      - update LightTest()
      
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

void lightTest()
{
//    lightColorChase(strip.Color(127, 127, 127), 50); // White
//    lightColorChase(strip.Color(127,   0,   0), 50); // Red
//    lightColorChase(strip.Color(127, 127,   0), 50); // Yellow
//    lightColorChase(strip.Color(  0, 127,   0), 50); // Green
//    lightColorChase(strip.Color(  0, 127, 127), 50); // Cyan
//    lightColorChase(strip.Color(  0,   0, 127), 50); // Blue
//    lightColorChase(strip.Color(127,   0, 127), 50); // Violet
//  
//    lightTheatreChase(strip.Color(127, 127, 127), 50); // White
//    lightTheatreChase(strip.Color(127,   0,   0), 50); // Red
//    lightTheatreChase(strip.Color(127, 127,   0), 50); // Yellow
//    lightTheatreChase(strip.Color(  0, 127,   0), 50); // Green
//    lightTheatreChase(strip.Color(  0, 127, 127), 50); // Cyan
//    lightTheatreChase(strip.Color(  0,   0, 127), 50); // Blue
//    lightTheatreChase(strip.Color(127,   0, 127), 50); // Violet
//  
//    lightColorWipe(strip.Color(127,   0,   0), 50);  // Red
//    lightColorWipe(strip.Color(  0, 127,   0), 50);  // Green
//    lightColorWipe(strip.Color(  0,   0, 127), 50);  // Blue
//  
//    lightRainbow(10);
//    lightRainbowCycle(0);  // make it go through the cycle fairly fast
//    lightTheatreChaseRainbow(50);
}
//
//void lightRainbow(uint8_t wait)
//// 3 cycles of all 384 colors in the wheel
//{
//  int i, j;
//
//  for (j = 0; j < 384; j++)
//  {
//    for (i = 0; i < NUM_LEDs; i++) {
//      strip.setPixelColor(i, Wheel( (i + j) % 384));
//    }
//    strip.show();   // write all the pixels out
//    delay(wait);
//  }
//}
//
//void lightRainbowCycle(uint8_t wait)
//// modifies LightRainbow to make rainbow wheel equally distributed along the chain
//{
//  uint16_t i, j;
//
//  for (j = 0; j < 384 * 5; j++) {   // 5 cycles of all 384 colors in the wheel
//    for (i = 0; i < strip.numPixels(); i++) {
//      // tricky math! we use each pixel as a fraction of the full 384-color wheel
//      // (thats the i / strip.numPixels() part)
//      // Then add in j which makes the colors go around per pixel
//      // the % 384 is to make the wheel cycle around
//      strip.setPixelColor(i, Wheel( ((i * 384 / strip.numPixels()) + j) % 384) );
//    }
//    strip.show();   // write all the pixels out
//    delay(wait);
//  }
//}
//
//void lightColorWipe(uint32_t c, uint8_t wait)
//// Fills all LEDs progressively in one color
//{
//  int i;
//
//  for (i = 0; i < strip.numPixels(); i++) {
//    strip.setPixelColor(i, c);
//    strip.show();
//    delay(wait);
//  }
//}
//
void lightColorFill(uint32_t c)
// Simulataneouly changes the entire strip to one color
{
  fill_solid(leds, NUM_LEDS, c);
  FastLED.show(); 
}

//void lightColorChase(uint32_t c, uint8_t wait)
//// Chase one dot down the full strip
//{
//  int i;
//
//  // Start by turning all pixels off:
//  for (i = 0; i < strip.numPixels(); i++) strip.setPixelColor(i, 0);
//
//  // Then display one pixel at a time:
//  for (i = 0; i < strip.numPixels(); i++) {
//    strip.setPixelColor(i, c); // Set new pixel 'on'
//    strip.show();              // Refresh LED states
//    strip.setPixelColor(i, 0); // Erase pixel, but don't refresh!
//    delay(wait);
//  }
//  strip.show(); // Refresh to turn off last pixel
//}
//
//void lightTheatreChase(uint32_t c, uint8_t wait)
//// Theatre-style crawling lights
//{
//  for (int j = 0; j < 10; j++) { //do 10 cycles of chasing
//    for (int q = 0; q < 3; q++) {
//      for (int i = 0; i < strip.numPixels(); i = i + 3) {
//        strip.setPixelColor(i + q, c);  //turn every third pixel on
//      }
//      strip.show();
//
//      delay(wait);
//
//      for (int i = 0; i < strip.numPixels(); i = i + 3) {
//        strip.setPixelColor(i + q, 0);      //turn every third pixel off
//      }
//    }
//  }
//}
//
//void lightTheatreChaseRainbow(uint8_t wait)
////Modifies lightTheatreChaseRainbow with rainbow effect
//{
//  for (int j = 0; j < 384; j++) {   // cycle all 384 colors in the wheel
//    for (int q = 0; q < 3; q++) {
//      for (int i = 0; i < strip.numPixels(); i = i + 3) {
//        strip.setPixelColor(i + q, Wheel( (i + j) % 384)); //turn every third pixel on
//      }
//      strip.show();
//
//      delay(wait);
//
//      for (int i = 0; i < strip.numPixels(); i = i + 3) {
//        strip.setPixelColor(i + q, 0);      //turn every third pixel off
//      }
//    }
//  }
//}
//
//// Helper functions for lighting
//
//uint32_t Wheel(uint16_t WheelPos)
////Input a value 0 to 384 to get a color value. The colours are a transition r - g -b - back to r
//{
//  byte r, g, b;
//  switch (WheelPos / 128)
//  {
//    case 0:
//      r = 127 - WheelPos % 128;   //Red down
//      g = WheelPos % 128;      // Green up
//      b = 0;                  //blue off
//      break;
//    case 1:
//      g = 127 - WheelPos % 128;  //green down
//      b = WheelPos % 128;      //blue up
//      r = 0;                  //red off
//      break;
//    case 2:
//      b = 127 - WheelPos % 128;  //blue down
//      r = WheelPos % 128;      //red up
//      g = 0;                  //green off
//      break;
//  }
//  return (strip.Color(r, g, b));
//}
