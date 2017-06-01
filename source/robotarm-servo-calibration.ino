//***************************************************
//**          ROBOT servo calibration              **
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

#include <Servo.h>                              // library required to control servos by PWM 
      
//***************************************************
//**            constants section                  **
//***************************************************
// This demo application has been written with Arduino Nano as target eco-system
// The sketch can easily be adapted for other systems, by simply adapting the pin numbers
// Any system running on 5V can be used with below sourcecode
// For each servo under test, a PWM output, an analogue input and a general I/O pin are required   

#define servoUpdateDelay 50                     // 15ms delay, after any servo action (default for library = 15ms)
#define DUTcount 4                              // number of Devices Under Test

const int servoPWMpins[] = {5,6,9,10};          // an array of PWM capable output pins to attach servos 1 to 4
                                                // each servo has 3 connections : ground, 5V power and a PWM signal
const int servoLEDpins[] = {12,8,7,4};          // an array of general purpose I/O pins to attach the LED's
                                                // on the sliders. The LED's will switch ON to indicate which servo is moving
static const uint8_t ADCpins[] = {A0,A1,A2,A3}; // the 4 ADC inputs which will be used for the slider potentiometers                                        
                        
//***************************************************
//**         global variables section              **
//***************************************************

Servo servos[4];                                // create 4 servo objects

boolean printUpdateToSerial = true;             // flag to indicate whether updated values should be sent to the serial monitor

//***************************************************
//**               setup section                   **
//***************************************************
void setup() {
  Serial.begin(115200);                         // initialize the serial communication 
  Serial.println(".");
  Serial.println("*********************************************************");
  Serial.println("**        ROBOT servo calibration tool - (c)2017       **");
  Serial.println("**  - Serial communication initialized                 **");
  // configure the LED pins as outputs
  for (int LEDpin = 0; LEDpin < DUTcount; LEDpin++) {
    pinMode(servoLEDpins[LEDpin], OUTPUT);
    digitalWrite(servoLEDpins[LEDpin], HIGH);
    }
  // perform a short 'lamp test'
  // all LED's will be switched ON, one at a time (and switched OFF afterwards)
  for (int LEDpin = 0; LEDpin < DUTcount; LEDpin++) {
    digitalWrite(servoLEDpins[LEDpin], LOW);
    delay(500);
    }
  for (int LEDpin = DUTcount - 1; LEDpin >= 0; LEDpin--) {
    digitalWrite(servoLEDpins[LEDpin], HIGH);
    delay(500);
    }
  Serial.println("** - LED's initialized                                 **");
  Serial.println("**                   Setup completed...                **");
  Serial.println("*********************************************************");
  Serial.println("**  make sure that the slider panel is correctly       **");
  Serial.println("**  connected to your Arduino Nano :                   **");
  Serial.println("**  pin 1  : GND to ground                             **");
  Serial.println("**  pin 2  : L1 to D12 -> LED on slider 1              **");
  Serial.println("**  pin 3  : P1 to A0 -> servo 1                       **");
  Serial.println("**  pin 4  : L2 to D8 -> LED on slider 2               **");
  Serial.println("**  pin 5  : P2 to A1 -> servo 2                       **");
  Serial.println("**  pin 6  : L3 to D7 -> LED on slider 3               **");
  Serial.println("**  pin 7  : P3 to A2 -> servo 3                       **");
  Serial.println("**  pin 8  : L4 to D4 -> LED on slider 4               **");
  Serial.println("**  pin 9  : P4 to A3 -> servo 4                       **");
  Serial.println("**  pin 10 : +5V to the power supply                   **");
  Serial.println("*********************************************************");
  Serial.println("**  connect all servos correctly to the Arduino Nano : **");
  Serial.println("**  all GND's to ground, all +5V to the power supply   **");
                 //  -> an external power supply might be needed, as
                 //     the Nano board can only provide limited current
  Serial.println("**  PWM input servo 1 -> D5                            **");
  Serial.println("**  PWM input servo 2 -> D6                            **");
  Serial.println("**  PWM input servo 3 -> D9                            **");
  Serial.println("**  PWM input servo 4 -> D10                           **");
  Serial.println("*********************************************************");
  Serial.println(" ");
  Serial.println("Your servos will now move to their initial position.");
  Serial.println("Please wait until all LED's are OFF...");
  for (int DUTid = 0; DUTid < DUTcount; DUTid++){moveServo(servos[DUTid],DUTid,90);}  
  Serial.println("...and from here on you can start playing with the sliders...");
  Serial.println(" "); 
  }

//***************************************************
//**            main application loop              **
//***************************************************
void loop() {
  //run the servo calibration procedure
  runServoCalibration(); 
  }

//***************************************************
//** application specific functions and procedures **
//***************************************************

// calibrate the servos with the slider potentiometers on analogue inputs A0 to A3
// the analogue slider and serial monitor need to be connected to use this function
// read-out the calibration result on the serial monitor 
void runServoCalibration() {
  int ADCvalue;
  int newPosition;
  float ADCvoltage;
  for (int DUTid = 0; DUTid < DUTcount; DUTid++){
    ADCvalue = analogRead(ADCpins[DUTid]);          // read the current value on the respective analogue input
    newPosition = map(ADCvalue, 0, 1023, 0, 179);   // map the ADC value to an angle between 0 and 180°
    moveServo(servos[DUTid],DUTid,newPosition);     // move the servo according to the slider input (if applicable)
    if (printUpdateToSerial) {                      // send the new values to the serial monitor, if they have been changed
      Serial.print("Servo");
      Serial.print(DUTid);
      Serial.print(" : ADC = ");
      ADCvoltage = ADCvalue * (5.0 / 1023.0);       // calculate the actual voltage on the ADC's input
      Serial.print(ADCvoltage);
      Serial.print("V, angle = ");
      Serial.print(newPosition);
      Serial.println("°");
      printUpdateToSerial = false;
    }
  }
}

// generic helper function to change a servo position
void moveServo(Servo servoToMove, int DUTid, int newPosition) {
  int stepSize = 3;                               // number of degrees to move per step
  int currentPosition = servoToMove.read();       // read the current position, to decide whether any movement is needed
  if (abs(newPosition - currentPosition) > 2) {
    printUpdateToSerial = true;
    digitalWrite(servoLEDpins[DUTid], LOW);
    servoToMove.attach(servoPWMpins[DUTid]);
    if (newPosition < currentPosition) {
      for (int i = currentPosition; i >= newPosition; i -= stepSize) {
        servoToMove.write(i);
        delay(servoUpdateDelay);
      }
    } else {
      for (int i = currentPosition; i <= newPosition; i += stepSize) {
        servoToMove.write(i);
        delay(servoUpdateDelay);
      }
    }
    servoToMove.detach();
    delay(servoUpdateDelay);
    digitalWrite(servoLEDpins[DUTid], HIGH);
  }
}
