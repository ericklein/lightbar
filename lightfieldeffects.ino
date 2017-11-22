/*
  Project Name : lightfieldeffects
  Developer : Eric Klein Jr. (temp2@ericklein.com)
  Description : LED strip animation routines
      
  Light functions
  -  lightColorWipe(uint32_t c, uint8_t wait) -> Fills all LEDs progressively in one color
  -  lightColorChase(uint32_t c, uint8_t wait) -> Chase one dot down the full strip
  -  lightTheatreChase(uint32_t c, uint8_t wait) -> Theatre-style crawling lights
  -  lightTheatreChaseRainbow(uint8_t wait)-> Modifies lightTheatreChaseRainbow with rainbow effect
  -  lightTest() -> excercises most of these functions to quickly test strip
*/

void lightTest()
{
   lightColorChase(CRGB::Red,50);
   lightTheatreChase(CRGB::Green, 50);
   lightColorWipe(CRGB::Blue, 50);
}

void lightColorWipe(uint32_t color, uint8_t wait) {
// Fills all LEDs progressively in one color

 for (int i = 0; i < NUM_LEDS; i++) 
  {
   leds[i] = color;
   FastLED.show();
   delay(wait);
  }
}

void lightColorChase(uint32_t color, uint8_t wait) {
// Chase one dot down the full strip
  // Start by turning all pixels off:
  FastLED.clear();
  // Then display one pixel at a time:
  for (int i = 0; i < NUM_LEDS; i++)
    {
      leds[i] = color; // Set new pixel 'on'
      FastLED.show();              // Refresh LED states
      leds[i] = CRGB:: Black; // Erase pixel, but don't refresh!
      delay(wait);
    }
  FastLED.show(); // Refresh to turn off last pixel
}

void lightTheatreChase(uint32_t c, uint8_t wait) {
// Theatre-style crawling lights

 for (int j = 0; j < 10; j++) 
  { //do 10 cycles of chasing
    for (int q = 0; q < 3; q++)
      {
        for (int i = 0; i < NUM_LEDS; i = i + 3)
          {
            leds[i + q] = c;  //turn every third pixel on
          }
        FastLED.show();
        delay(wait);
        for (int i = 0; i < NUM_LEDS; i = i + 3)
          {
            leds[i + q] = CRGB::Black;      //turn every third pixel off
          }
      }
  }
}
