#include <Servo.h>

Servo myservo;  // create servo object to control a servo
#define SERVO_PIN 9 // Servo motor pin
#define SERVO_IN_ANGLE 70 // Angle of the dispenser servo motor when it is hidden
#define SERVO_OUT_ANGLE 180 // Angle of the dispenser servo motor when it is dispensing


void setup() {
  myservo.attach(SERVO_PIN);  // attaches the servo on GIO2 to the servo object
}

void loop() {
  int pos;

  for (pos = SERVO_IN_ANGLE; pos <= SERVO_OUT_ANGLE; pos += 1) {  // goes from SERVO_IN_ANGLE to SERVO_OUT_ANGLE 
    // in steps of 1 degree
    myservo.write(pos);  // tell servo to go to position in variable 'pos'
    delay(15);           // waits 15ms for the servo to reach the position
  }
  for (pos = SERVO_OUT_ANGLE; pos >= SERVO_IN_ANGLE; pos -= 1) {  // goes from SERVO_OUT_ANGLE to SERVO_IN_ANGLE
    myservo.write(pos);                  // tell servo to go to position in variable 'pos'
    delay(15);                           // waits 15ms for the servo to reach the position
  }

}
