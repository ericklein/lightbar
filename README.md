N# lightbar
## What is lightbar? 
Simple packet communication between 900mhz radios. reciever is half of the solution, the other half is in the transmitter application.

### Purpose
Starting point for custom LED strip installs

### Contributors


### Software Dependencies
  - FastLED https://github.com/FastLED/FastLED
  - buttonhandler https://github.com/ericklein/buttonhandler
  - encoder https://github.com/PaulStoffregen/Encoder
  - Adafruit NeoPixel https://github.com/adafruit/Adafruit_NeoPixel

### BOM
  - 1 or more: Adafruit Rotary Encoder 377b
  - 1 or more: simple buttons
  - LED strip; any supported by FastLED or Adafruit NeoPixel (for RGBW)
  - optional: Blinkytape (Arduino Leonardo with 60 WS2811)
  - 1X: MCU if not Blinkytape
  - 1 protoboard
  - wire

### Pinouts
  - buttons to digital pin, ground
  - rotary encoder to interupt enabled pins, ground
  - LED strip to data pin(s), independent 5v, power GND + Arduino GND

### Information Sources
- See Evernote note on wiring for LED strips.

### Issues
- 111820 - [P1] Code does not compile when lightfieldeffects.ino is included. Code needs to be transitioned to David's LEDControl library

### Questions

### Learnings

### Feature Requests

 
### Revisions
- 10/17/14
  -  fork from example to control LPD8806-based RGB LED Modules in a strip
  -  added multiple button support, will be augmented/replaced by rotary encoder in future version
- 11/15/14
  -  prototypes for desired lighting functions (lightXxYy)
- 04/21/15
  -  fixed bugs in lightColorFill
- 03/02/16
  -  first version in GitHub
  -  added rotary encoder, code is extremely crude
- 04/27/16
  -  03/02/16 - rotary encoder push is captured
  -  code readability improved, still much to do
- 05/22/16
  - changed rotation tracking to boolean
  - 03/02/16 - fixed wrap-around of intensity bug
  - moved light test script from loop() to lightTest, no dev work done on function itself
  - hard coded button functionality, now can comment out functionality based on hardware config
- 12/27/16
  - completed 05/22/16 revisions
  - 03/02/16 - switch from LED intensity (one value) to r,g,b levels
- 12/29/16
  - fixed bug in blue to white transition
  - 04/21/15 - move light functions to library
  - 03/02/16 - added off (0) state to stripColor to control On/Off
  - 03/02/16 - added "powered" On/Off state with control from rotary encoder; Arduino and LED are powered, LEDs at 0,0,0
- 07/19/17
	- Moved revision history, feature requests, build targets, etc. to README.md
	- refactored code in loop()
  07/29/17
  - 03/02/16 - can I use FastLED or other generic library
- 08/01/17
  - 04/27/16 - consistent use of wire color for ground, etc.
  - 12/27/16 - standard connectors for LED strip
  - 07/29/17 - FastLED 3.1 upgrade
  - 07/29/17 - move LED pins to SPI
  - 07/29/17 - how do I checkin code with new filename
- 08/06/17
  - 07/29/17 - fix lightfieldeffects.ino after FastLED transition
- 11/21/17
  - completed lightfieldeffects.ino fixes after FastLED transition
  - conditional code branching for blinkytape and LPD8806
  11/26/17
  - 12/29/16 - button support to library
  - 11/20/17 - button globals probably don't need to be global
- 05/05/19
  - [FR 08/06//17] should I run a FastLED.clear() before each light function?
    - Yes, functions don't clear themselves so higher logic can control this
  - [FR 08/06/17] switch delay() to FastLED.delay function?
    - No, FastLED.delay() only for platforms without native delay() function
  - added brightness parameter to lightColorChase, lightTheatreChase
  - updated button initialization based on changes to button library
- 06/02/19
  - variable name changes to improve code readability
  - function name alignment missed during 05/05/19 changes
  - [FR 05/10/19] ARM compilation
  - [FR 03/02/16] CRGB::Red is green and CRGB::Green is Red [LPD8086]
- 04/25/20
  - [FR 11/24/17] transition to encoder library for rotary encoder support
  - [FR 05/22/16] why do I care about halfLeft and halfRight? -> CLOSED with encoder.h
  - [FR 11/20/17] enum stripColor for readability
  - [FR 11/24/17] stripColor should not also embed on/off status (see 03/02/16), impacts resolveButtons
  - [FR 11/24/17] implement DEBUG flag for print and println and put !Serial under that debug flag
  - RGBW strip support via Adafruit NeoPixel library
- 111820
  - Added more control for RGBW strips