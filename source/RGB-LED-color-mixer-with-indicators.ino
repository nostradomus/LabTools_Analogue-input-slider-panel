//***************************************************
//**            RGB LED color mixer                **
//**            with indicators                    **
//**                                               **
//**   author : Carl - (c)2017                     **
//**   version : 1.0                               **
//**   date : 01/06/2017                           **
//**                                               **
//**   https://github.com/nostradomus/             **
//**          LabTools_Analogue-input-slider-panel **
//**                                               **
//***************************************************
// In this example the 12 WS2812b RGB-LED's have been connected in series
// For testing purposes, the RGB LED interface board from project : 
// https://github.com/nostradomus/Base3-clock has been used
// Any number of these LED's can be used with this example
// Just adapt the first parameter in the "strip"'s constructor call (and foresee a strong enough power-supply ;-)
// You might need to modify the third parameter as well, when using other LED-types 

// This version of the application also uses the LED's on the slider
// buttons to indicate which channel was used for the most recent changes.
// The respective LED (or LED's) will stay ON, for the time as configured in LEDupTime.

//***************************************************
//**             library section                   **
//***************************************************
#include <Adafruit_NeoPixel.h>     // library required to control the WS2812B LED's, 
                                   // see : https://github.com/adafruit/Adafruit_NeoPixel
//***************************************************
//**            constants section                  **
//***************************************************
#define dataPIN 6                               // output pin for the neopixel dataline. On Arduino NANO, in this example we used 'D6'
                                                // to be adapted to your setup
#define channelCount 4                          // number of connected channels
#define LEDupTime 4000                          // time the LED will stay ON after the last change
#define serialUpdateFilter 4                    // filter with minimum change before sending new values to the serial monitor
#define buttonUpdateFilter 2                    // filter with minimum change to update the button LED's

const int sliderLEDpins[] = {12,8,7,4};         // an array of general purpose I/O pins to attach the LED's
                                                // on the sliders. The LED's will switch ON to indicate which color channel has been updated
static const uint8_t ADCpins[] = {A0,A1,A2,A3}; // the 4 ADC inputs to be used for the slider potentiometers   

//***************************************************
//**         global variables section              **
//***************************************************

int bLevel = 127;                             // brightness level 0..255 (initial value at 50%)
int checksum = 0;                             // RGB checksum, to only print to serial in case of changes

byte newChannelValue[] = {0,0,0,0};           // new RGB and brightness values from the analogue inputs
byte previousChannelValue[] = {0,0,0,0};      // previous RGB and brightness values stored from the previous measuremnts 
long LEDstartTime[] = {0,0,0,0};              // Variables to check the time-out to switch the button LED's to OFF

Adafruit_NeoPixel strip = Adafruit_NeoPixel(12, dataPIN, NEO_GRB + NEO_KHZ800);
// Parameter 1 = number of pixels connected on the dataline
// Parameter 2 = Arduino pin number 
// Parameter 3 = LED-pixel type settings, add add-up in case of combinations:
//   NEO_KHZ800  800 KHz bitstream -> most common (WS2812 LEDs)
//   NEO_KHZ400  400 KHz -> classic types ('v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels with GRB bitstream (most NeoPixel products), or NEO_GRBW
//   NEO_RGB     Pixels with RGB bitstream (v1 FLORA pixels, not v2), or NEO_RGBW
// (for more info, please check out the documentation on Adafruit's github : 
//  https://github.com/adafruit/Adafruit_NeoPixel)

//presets for fixed LED colors
uint32_t dark = strip.Color(0, 0, 0);         // LED pixel -> OFF            
uint32_t green = strip.Color(0, 255, 0);      // LED pixel -> green
uint32_t red = strip.Color(255, 0, 0);        // LED pixel -> red
uint32_t blue = strip.Color(0, 0, 255);       // LED pixel -> blue
uint32_t white = strip.Color(255, 255, 255);  // LED pixel -> white

//***************************************************
//**               setup section                   **
//***************************************************
void setup() {
  Serial.begin(115200);                            // initialize the serial communication 
  Serial.println(".");
  Serial.println(" ");
  Serial.println("*********************************************************");
  Serial.println("**             RGB LED color mixer - (c)2017           **");
  Serial.println("**  - Serial communication initialized                 **");
  strip.begin();

  strip.setBrightness(bLevel);                    // set the LED-pixel brightness 
  strip.show();                                   // Initialize all pixels to 'OFF'
  Serial.print("** - LED brightness set to : ");
  Serial.println(bLevel);
  Serial.println("** - LED strip initialized                             **");
  // configure the LED pins as outputs
  for (int LEDpin = 0; LEDpin < channelCount; LEDpin++) {
    pinMode(sliderLEDpins[LEDpin], OUTPUT);
    digitalWrite(sliderLEDpins[LEDpin], HIGH);
    }
  // perform a short 'lamp test'
  // all LED's will be switched ON, one at a time (and switched OFF afterwards)
  for (int LEDpin = 0; LEDpin < channelCount; LEDpin++) {
    digitalWrite(sliderLEDpins[LEDpin], LOW);
    delay(500);
    }
  for (int LEDpin = channelCount - 1; LEDpin >= 0; LEDpin--) {
    digitalWrite(sliderLEDpins[LEDpin], HIGH);
    delay(500);
    }
  Serial.println("** - slider LED's initialized                          **");  
  Serial.println("**                   Setup completed...                **");
  Serial.println("*********************************************************");
  Serial.println("**  make sure that the slider panel is correctly       **");
  Serial.println("**  connected to your Arduino :                        **");
  Serial.println("**  pin 1  : GND to ground                             **");
  Serial.println("**  pin 2  : L1 to D12 -> LED on slider 1              **");
  Serial.println("**  pin 3  : P1 to A0 -> RED                           **");
  Serial.println("**  pin 4  : L1 to D8 -> LED on slider 1               **");
  Serial.println("**  pin 5  : P2 to A1 -> GREEN                         **");
  Serial.println("**  pin 6  : L1 to D7 -> LED on slider 1               **");
  Serial.println("**  pin 7  : P3 to A2 -> BLUE                          **");
  Serial.println("**  pin 8  : L1 to D4 -> LED on slider 1               **");
  Serial.println("**  pin 9  : P4 to A3 -> brightness                    **");
  Serial.println("**  pin 10 : +5V to the power supply                   **");
  Serial.println("*********************************************************");
  Serial.println(" ");
  Serial.println("Your LED's will now change color a couple of times to make sure that all is well connected...");
  strip.setBrightness(63);
  for(uint16_t i=0; i<strip.numPixels(); i++) {strip.setPixelColor(i, red);}
  strip.show();
  Serial.println("RED..............");
  delay(1000);
  strip.setBrightness(127);
  for(uint16_t i=0; i<strip.numPixels(); i++) {strip.setPixelColor(i, green);}
  strip.show();
  Serial.println("...GREEN.........");
  delay(1000);
  strip.setBrightness(195);
  for(uint16_t i=0; i<strip.numPixels(); i++) {strip.setPixelColor(i, blue);}
  strip.show();
  Serial.println("........BLUE.....");
  delay(1000);
  strip.setBrightness(255);
  for(uint16_t i=0; i<strip.numPixels(); i++) {strip.setPixelColor(i, white);}
  strip.show();
  Serial.println("............WHITE");
  delay(1000);
  Serial.println("...and from here on you can start playing with the sliders...");
  Serial.println(" ");  
  }

//***************************************************
//**            main application loop              **
//***************************************************
void loop() {
  //run the color calibration procedure
  runColorCalibrationProcedure(); 
  }

//***************************************************
//** application specific functions and procedures **
//***************************************************

// calibrate the colors with the slider potentiometers on analogue inputs A0 to A3
// the analogue slider and serial monitor need to be connected to use this function
// read-out the shown color on the serial monitor 
void runColorCalibrationProcedure() {            
      // read the current values on the analogue inputs and process them
      for (int i = 0; i < channelCount; i++) {
          newChannelValue[i] = map(analogRead(ADCpins[i]), 0, 1023, 0, 255);
          if (abs(newChannelValue[i] - previousChannelValue[i]) > buttonUpdateFilter) {
            previousChannelValue[i] = newChannelValue[i];
            LEDstartTime[i] = millis();
            digitalWrite(sliderLEDpins[i], LOW);
            } else {
              if ((millis() - LEDstartTime[i]) > LEDupTime) {
                digitalWrite(sliderLEDpins[i], HIGH);}
              }
          }
      bLevel = newChannelValue[3];
      int newChecksum = newChannelValue[0] + newChannelValue[1] + newChannelValue[2] + bLevel;
      if (abs(checksum - newChecksum) > serialUpdateFilter) {
          // update the LED's if any value has been changed on the analogue inputs
          // you can change the <serialUpdateFilter> value in the equation to obtain more or less updates
          // 0 will give you all changes (might cause an enormous amount of serial communication)
          // 3 will mask potentiometer jitter, but still provide close enough values for the mixed color
          // The <serialUpdateFilter> constant can set in the 'constants section'
          checksum = newChecksum;
          // send the new values to the serial monitor
          Serial.print("Color : R-");
          Serial.print(newChannelValue[0]);
          Serial.print(" -G ");
          Serial.print(newChannelValue[1]);
          Serial.print(" -B ");
          Serial.print(newChannelValue[2]);
          Serial.print(" -brightness ");
          Serial.println(bLevel);
          // set the LED-pixel brightness
          strip.setBrightness(bLevel);
          // prepare the data-frame with color-info for each LED-pixel
          uint32_t c = strip.Color(newChannelValue[0], newChannelValue[1], newChannelValue[2]);
          for(uint16_t i=0; i<strip.numPixels(); i++) 
           {
           strip.setPixelColor(i, c);
           }
          // submit the new settings to all LED-pixels
          strip.show();  
      }
}





