//***************************************************
//**            RGB LED color mixer                **
//**                                               **
//**   author : Carl - (c)2017                     **
//**   version : 1.0                               **
//**   date : 01/05/2017                           **
//**                                               **
//**   https://github.com/nostradomus/             **
//**          LabTools_Analogue-input-slider-panel **
//**                                               **
//***************************************************

//***************************************************
//**             library section                   **
//***************************************************
#include <Adafruit_NeoPixel.h>     // library required to control the WS2812B LED's, 
                                   // see : https://github.com/adafruit/Adafruit_NeoPixel
//***************************************************
//**            constants section                  **
//***************************************************
#define dataPIN 6                  // output pin for the neopixel dataline. On Arduino NANO, in this example we used 'D6'
                                   // to be adapted to your setup
//***************************************************
//**         global variables section              **
//***************************************************
// Parameter 1 = number of pixels connected on the dataline
// Parameter 2 = Arduino pin number 
// Parameter 3 = LED-pixel type settings, add add-up in case of combinations:
//   NEO_KHZ800  800 KHz bitstream -> most common (WS2812 LEDs)
//   NEO_KHZ400  400 KHz -> classic types ('v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels with GRB bitstream (most NeoPixel products), or NEO_GRBW
//   NEO_RGB     Pixels with RGB bitstream (v1 FLORA pixels, not v2), or NEO_RGBW
Adafruit_NeoPixel strip = Adafruit_NeoPixel(12, dataPIN, NEO_GRB + NEO_KHZ800);
// In this example the 12 WS2812b RGB-LED's have been connected in series
// For testing purposes, the RGB LED interface board from project : 
// https://github.com/nostradomus/Base3-clock has been used
// Any number of these LED's can be used with this example
// Just adapt the first parameter in the "strip"'s constructor call (and foresee a strong enough power-supply ;-)
// You might need to modify the third parameter as well, when using other LED-types 
// (for more info, please check out the documentation on Adafruit's github : 
//  https://github.com/adafruit/Adafruit_NeoPixel)

int bLevel = 127;                             // brightness level 0..255 (initial value at 50%)
int checksum = 0;                             // RGB checksum, to only print to serial in case of changes

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
  Serial.println("**                   Setup completed...                **");
  Serial.println("*********************************************************");
  Serial.println("**  make sure that the slider panel is correctly       **");
  Serial.println("**  connected to your Arduino :                        **");
  Serial.println("**  pin 1  : GND to ground                             **");
  Serial.println("**  pin 3  : P1 to A0 -> RED                           **");
  Serial.println("**  pin 5  : P2 to A1 -> GREEN                         **");
  Serial.println("**  pin 7  : P3 to A2 -> BLUE                          **");
  Serial.println("**  pin 9  : P4 to A3 -> brightness                    **");
  Serial.println("**  pin 10 : +5V to the power supply                   **");
  Serial.println("*********************************************************");
  Serial.println(" ");
  Serial.println("Your LED's will now change color a couple of times to make sure that all is well connected...");
  strip.setBrightness(63);
  for(uint16_t i=0; i<strip.numPixels(); i++) {strip.setPixelColor(i, red);}
  strip.show();
  Serial.println("RED..............");
  delay(1500);
  strip.setBrightness(127);
  for(uint16_t i=0; i<strip.numPixels(); i++) {strip.setPixelColor(i, green);}
  strip.show();
  Serial.println("...GREEN.........");
  delay(1500);
  strip.setBrightness(195);
  for(uint16_t i=0; i<strip.numPixels(); i++) {strip.setPixelColor(i, blue);}
  strip.show();
  Serial.println("........BLUE.....");
  delay(1500);
  strip.setBrightness(255);
  for(uint16_t i=0; i<strip.numPixels(); i++) {strip.setPixelColor(i, white);}
  strip.show();
  Serial.println("............WHITE");
  delay(1500);
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
      // analogue values from the potentiometers
      int Rpot, Gpot, Bpot, bLevelpot;                 
      // read the current values on the analogue inputs
      Rpot = analogRead(A0);
      int newR = map(Rpot, 0, 1023, 0, 255);
      Gpot = analogRead(A1);
      int newG = map(Gpot, 0, 1023, 0, 255);
      Bpot = analogRead(A2);
      int newB = map(Bpot, 0, 1023, 0, 255);
      bLevelpot = analogRead(A3);
      bLevel = map(bLevelpot, 0, 1023, 0, 255);
      if (abs(checksum - (newR + newG + newB + bLevel)) > 3) {
          // update the LED's if any value has been changed on the analogue inputs
          // you can change the integer value in the equation to obtain more or less updates
          // 0 will give you all changes (might cause an enormous amount of serial communication)
          // 3 will mask potentiometer jitter, but still provide close enough values for the mixed color
          checksum = newR + newG + newB + bLevel;
          // send the new values to the serial monitor
          Serial.print("Color : R-");
          Serial.print(newR);
          Serial.print(" -G ");
          Serial.print(newG);
          Serial.print(" -B ");
          Serial.print(newB);
          Serial.print(" -brightness ");
          Serial.println(bLevel);
          // set the LED-pixel brightness
          strip.setBrightness(bLevel);
          // prepare the data-frame with color-info for each LED-pixel
          uint32_t c = strip.Color(newR, newG, newB);
          for(uint16_t i=0; i<strip.numPixels(); i++) 
           {
           strip.setPixelColor(i, c);
           }
          // submit the new settings to all LED-pixels
          strip.show();  
      }
}




