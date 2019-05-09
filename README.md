# lightbar
LED light strip control

Description : Library of light patterns for LED strips for stand-alone application or as library function for other applications

Sources
  - uses FastLED https://github.com/FastLED/FastLED
  - uses button https://github.com/ericklein/button 

Targets
  - tested on Arduino Uno with LPD8806
  - tested on Blinkytape (Arduino Leonardo with 60 WS2811)
  - Adafruit Rotary Encoder 377b

Information Sources


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
  -  03/02/16 - rotary encoder push is captured
  -  code readability improved, still much to do
  05/22/16
  - changed rotation tracking to boolean
  - 03/02/16 - fixed wrap-around of intensity bug
  - moved light test script from loop() to lightTest, no dev work done on function itself
  - hard coded button functionality, now can comment out functionality based on hardware config
  12/27/16
  - completed 05/22/16 revisions
  - 03/02/16 - switch from LED intensity (one value) to r,g,b levels
  12/29/16
  - fixed bug in blue to white transition
  - 04/21/15 - move light functions to library
  - 03/02/16 - added off (0) state to stripColor to control On/Off
  - 03/02/16 - added "powered" On/Off state with control from rotary encoder; Arduino and LED are powered, LEDs at 0,0,0
  07/19/17
  	- Moved revision history, feature requests, build targets, etc. to README.md
  	- refactored code in loop()
  07/29/17
    - 03/02/16 - can I use FastLED or other generic library
  08/01/17
    - 04/27/16 - consistent use of wire color for ground, etc.
    - 12/27/16 - standard connectors for LED strip
    - 07/29/17 - FastLED 3.1 upgrade
    - 07/29/17 - move LED pins to SPI
    - 07/29/17 - how do I checkin code with new filename
  08/06/17
    - 07/29/17 - fix lightfieldeffects.ino after FastLED transition
  11/21/17
    - completed lightfieldeffects.ino fixes after FastLED transition
    - conditional code branching for blinkytape and LPD8806
  11/26/17
    - 12/29/16 - button support to library
    - 11/20/17 - button globals probably don't need to be global
  05/05/19
    - [FR 05/14/14] Implemented lightOneColor, lightOneColorFill, lightOneColorBlink, lightOneColorRamp
    - [FR 04/21/15] why do many functions pass parameters that are not used?
    - [FR 08/06//17] should I run a FastLED.clear() before each light function?
      - Yes, functions don't clear themselves so higher logic can control this
    - [FR 08/06/17] switch delay() to FastLED.delay function?
      - No, FastLED.delay() only for platforms without native delay() function
    - added brightness parameter to lightColorChase, lightTheatreChase
    - updated button initialization based on changes to button library
    
Feature Requests
  - 05/14/14
    - lightCylon(color, brightness, speed)
    - lightPacMan(colorone, colortwo, speed, distancebetween)
    - lightRainbow()
    - lightWaterFlow()
    - lightVolcano()
    - lightFireplace()
  - 03/02/16
    -  on/off switch [software implementation 12/29/16]
    -  toggle light function switch
    -  memory optimization
      - int to byte variable conversion
  - 05/22/16
    - Why do I care about halfLeft and halfRight?
  - 12/27/16
      - resistor in front of LED #1 for LPD8086
  - 12/29/16
      - improve code readability
  - 07/29/17        - 
      - CRGB::Red is green and CRGB::Green is Red? [LPD8086]
  - 11/20/17
      - enum stripColor for readability
  - 11/24/17
      - rotary encoder to to class/library/file separated functions
      - stripColor should not also embed on/off status (see 03/02/16), impacts resolveButtons
      - implement DEBUG flag for print and println
    05/07/19
      - Alexa support
      - remote logging
      - error handling
      - not sure stripBrightness needs to be a global, each function should receive it