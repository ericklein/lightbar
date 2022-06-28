/*
  Project Name:   lightbox
  Developer:      Eric Klein Jr. (temp2@ericklein.com)
  Description:    Control LED strips for easyGrade card photo capture box

  See README.md for target information, revision history, feature requests, etc.
*/

// conditional compile flags
#define DEBUG 	// Output to serial port

// LED strip type
// #define blinkytape	// fixed LED count, one button only
// #define RGBWSupport 	// uses different library as FastLED doesn't support RGBW

// hardware configuration
#ifdef blinkytape
	#define ledCount	60  // BlinkyTape has 60 LEDs
	#define buttonOnePin	9	// fixed PIN
#else
	#define ledCount      20  // Number of RGB LEDs on strip
	#define buttonOnePin    9  // push button
  	#define rotaryEncoderOnePin1    11  // counter-clockwise, used to change brighness
  	#define rotaryEncoderOnePin2    10  // clockwise, used to change brightness
	#define ledDataPin      12  // for one wire LED strips
	//#define ledClockPin   12  // for two wire LED strips
#endif

// LED configuration
#define ledsPerBank		5
#define stripMinBrightness 6
#define stripMaxBrightness 248
#define brightnessStepChange 2

// button configuration
//#define buttonLongPressDelay 2500