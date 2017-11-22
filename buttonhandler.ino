void buttonEvent(const char* button_name, int event)
{
  //short press on ColorSelect rotates LED strip color
  if ((button_name == "ColorSelect") && (event == 1))
  {
    switch (stripColor)
    {
       case 0: // none -> white
         fill_solid(leds, NUM_LEDS, CRGB::White);
                 Serial.println("ColorSelect button short press -> cycle to white");
         stripColor = 1;
      break;
      case 1: //white -> red
            fill_solid(leds, NUM_LEDS, CRGB::Red);
                 Serial.println("ColorSelect button short press -> cycle to red");
        stripColor = 2;
        break;
      case 2: //red -> green
  fill_solid(leds, NUM_LEDS, CRGB::Green);
     Serial.println("ColorSelect button short press -> cycle to green");

        stripColor = 3;
        break;
      case 3: //green -> blue
  fill_solid(leds, NUM_LEDS, CRGB::Blue);
     Serial.println("ColorSelect button short press -> cycle to blue");

        stripColor = 4;
        break;
      case 4: //blue -> white
  fill_solid(leds, NUM_LEDS, CRGB::White);
     Serial.println("ColorSelect button short press -> cycle to white");

        stripColor = 1;
        break;
    }
  LEDS.show();
  }
  //long press on ColorSelect resets LED strip color to white
  if ((button_name == "ColorSelect") && (event == 2))
  {

    stripColor = 1;
  fill_solid(leds, NUM_LEDS, CRGB::White);
  LEDS.show();
      Serial.println("ColorSelect button long press -> reset LED strip color to white");
  }

  //short press on LightFieldEffect does ???
  if ((button_name == "LightFieldEffect") && (event == 1))
  {
    lightColorChase(CRGB::Red,50);
    LEDS.clear();
    Serial.println("LightFieldEffect button short press"); //debug text
  }

  //long press on LightFunction exercises all lightfieldeffects functions
  if ((button_name == "LightFieldEffect") && (event == 2))
  {
    Serial.println("LightFieldEffect button long press"); //debug text
    lightTest();
  }

  //short press on OnOff turns the strip on
  if ((button_name == "OnOff") && (event == 1))
  {
    Serial.print(stripColor);
    if (stripColor == 0)
    {
      LEDS.showColor(CRGB::White);
      LEDS.show();
      stripColor = 1;
    }
    Serial.println("Encoder button short press -> turn strip on if off"); //debug text

  }

  //long press on OnOff turns the strip off
  if ((button_name == "OnOff") && (event == 2))
  {
    LEDS.clear();
    stripColor = 0;
    Serial.println("Encoder button long press -> turn strip off"); //debug text
  }
  delay(debounceLength); //debounce button
}

void resolveButtons()
{
#ifdef blinkytape
  int event2 = buttonLightFieldEffect.handle();
   buttonEvent("LightFieldEffect", event2);
  #else
      // read buttons for events

  int event1 = buttonColorSelect.handle();
  int event2 = buttonLightFieldEffect.handle();
  int event3 = buttonOnOff.handle();

  // deal with button events
    buttonEvent("ColorSelect", event1);
    buttonEvent("LightFieldEffect", event2);
  buttonEvent("OnOff", event3);
  #endif
}

void resolveRotaryEncoders()
{
  // deal with encoder rotation
  if (rotateClockWise)
  {
    if (stripBrightness<248) // light not at max brightness
    {
      stripBrightness+=8;
      LEDS.setBrightness(stripBrightness);
      LEDS.show();
      rotateClockWise = false; //reset rotation status
            Serial.print("increase brightness of current color to ");
Serial.println(stripBrightness);
    }
    else
    { Serial.println("increase not possible");
      rotateClockWise = false; //reset rotation status
    }
  }
  if (rotateCounterClockWise)
  {
    if (stripBrightness>0) // light not at minimum brighness
    {
      stripBrightness-=8;
      LEDS.setBrightness(stripBrightness);
      LEDS.show();
      rotateCounterClockWise = false; //reset rotation status
      Serial.print("decrease brightness of current color to ");
      Serial.println(stripBrightness);
    }
    else
    {
      Serial.println("decrease not possible");
      rotateCounterClockWise = false; //reset rotation status

    }
  }
}

#ifndef blinkytape
void checkRE1Pin(){
  delay(1); //debounce
  if (digitalRead(rotaryEncoder1Pin) == LOW) // pin still LOW?
  {
    if (digitalRead(rotaryEncoder2Pin) == HIGH && halfright == false)
    {
      halfright = true; // 1/2 click counter-clockwise
    }
    if (digitalRead(rotaryEncoder2Pin) == LOW && halfleft == true)
    {
      halfleft = false;
      rotateCounterClockWise = true;  // one whole click counter-clockwise
    }
  }
}

void checkRE2Pin(){
  delay(1); // debounce
  if (digitalRead(rotaryEncoder2Pin) == LOW) // pin still LOW?
  {
    if (digitalRead(rotaryEncoder1Pin) == HIGH && halfleft == false)
    {
      halfleft = true; // 1/2 click clockwise
    }
    if (digitalRead(rotaryEncoder1Pin) == LOW && halfright == true)
    {
      halfright = false;
      rotateClockWise = true;  // One whole click clockwise
    }
  }
}
#endif
