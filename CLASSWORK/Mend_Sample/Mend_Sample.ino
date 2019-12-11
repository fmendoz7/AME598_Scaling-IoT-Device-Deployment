/* Sweep
 by BARRAGAN <http://barraganstudio.com>
 This example code is in the public domain.
 modified 8 Nov 2013
 by Scott Fitzgerald
 modified for the ESP32 on March 2017
 by John Bennett
 see http://www.arduino.cc/en/Tutorial/Sweep for a description of the original code
 * Different servos require different pulse widths to vary servo angle, but the range is 
 * an approximately 500-2500 microsecond pulse every 20ms (50Hz). In general, hobbyist servos
 * sweep 180 degrees, so the lowest number in the published range for a particular servo
 * represents an angle of 0 degrees, the middle of the range represents 90 degrees, and the top
 * of the range represents 180 degrees. So for example, if the range is 1000us to 2000us,
 * 1000us would equal an angle of 0, 1500us would equal 90 degrees, and 2000us would equal 1800
 * degrees.
 * 
 * Circuit: (using an ESP32 Thing from Sparkfun)
 * Servo motors have three wires: power, ground, and signal. The power wire is typically red,
 * the ground wire is typically black or brown, and the signal wire is typically yellow,
 * orange or white. Since the ESP32 can supply limited current at only 3.3V, and servos draw
 * considerable power, we will connect servo power to the VBat pin of the ESP32 (located
 * near the USB connector). THIS IS ONLY APPROPRIATE FOR SMALL SERVOS. 
 * 
 * We could also connect servo power to a separate external
 * power source (as long as we connect all of the grounds (ESP32, servo, and external power).
 * In this example, we just connect ESP32 ground to servo ground. The servo signal pins
 * connect to any available GPIO pins on the ESP32 (in this example, we use pin 18.
 * 
 * In this example, we assume a Tower Pro MG995 large servo connected to an external power source.
 * The published min and max for this servo is 1000 and 2000, respectively, so the defaults are fine.
 * These values actually drive the servos a little past 0 and 180, so
 * if you are particular, adjust the min and max values to match your needs.
 */

#include <TTGO.h>
TTGOClass *ttgo;

#include <ESP32Servo.h>

Servo myservo;  // create servo object to control a servo
// 16 servo objects can be created on the ESP32
Servo myservo2;

int pos = 0;    // variable to store the servo position
// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33 
int servoPin = 21;
int servoPin2 = 22;

void pressed()
{
    uint16_t color = random(0xFFFF);
    ttgo->eTFT->fillScreen(color);
    ttgo->eTFT->setTextColor(color, TFT_WHITE);
    ttgo->eTFT->drawString("HUMIDITY",  5, 100, 4);
}

void released()
{
    uint16_t color = random(0xFFFF);
    ttgo->eTFT->fillScreen(color);
    ttgo->eTFT->setTextColor(color, TFT_WHITE);
    ttgo->eTFT->drawString("TEMPERATURE",  5, 100, 4);
}

void setup() {
  myservo.setPeriodHertz(50);    // standard 50 hz servo
  myservo.attach(servoPin, 1000, 2000); // attaches the servo on pin 18 to the servo object
  // using default min/max of 1000us and 2000us
  // different servos may require different min/max settings
  // for an accurate 0 to 180 sweep

  myservo2.setPeriodHertz(50);    // standard 50 hz servo
  myservo2.attach(servoPin2, 1000, 2000); // attaches the servo on pin 18 to the servo object

    Serial.begin(115200);
    ttgo = TTGOClass::getWatch();
    ttgo->begin();
    ttgo->openBL();

    ttgo->eTFT->fillScreen(TFT_BLACK);
    ttgo->eTFT->setTextColor(TFT_WHITE, TFT_BLACK);
    ttgo->eTFT->setTextFont(4);
    ttgo->eTFT->drawString("User Button released",  5, 100, 4);

    ttgo->button->setPressedHandler(pressed);
    ttgo->button->setReleasedHandler(released);

}

void loop() {

  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);    // tell servo to go to position in variable 'pos'
    myservo2.write(pos);
    delay(15);             // waits 15ms for the servo to reach the position
  }
  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);    // tell servo to go to position in variable 'pos'
    myservo2.write(pos);
    delay(15);             // waits 15ms for the servo to reach the position
  }

  ttgo->button->loop();
}
