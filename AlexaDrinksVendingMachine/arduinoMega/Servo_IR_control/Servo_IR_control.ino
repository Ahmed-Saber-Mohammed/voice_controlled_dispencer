#include <Servo.h>
#include <IRremote.h>

Servo myservo;  // create servo object to control a servo
#define SERVO_PIN 9 // Servo motor pin
#define SERVO_IN_ANGLE 70 // Angle of the dispenser servo motor when it is hidden
#define SERVO_OUT_ANGLE 180 // Angle of the dispenser servo motor when it is dispensing

#define IR_RX_PIN 43

IRrecv irrecv(IR_RX_PIN);
decode_results results;
unsigned long button1 = 0xFF30CF;
unsigned long button2 = 0xFF18E7;

void setup() {
  myservo.attach(SERVO_PIN);  // attaches the servo on GIO2 to the servo object.

    // Initialize IR remote
    irrecv.enableIRIn();
}

void loop() {
  int pos;
if (irrecv.decode(&results)) {
                if (results.value == button1) {
                    for (pos = SERVO_IN_ANGLE; pos <= SERVO_OUT_ANGLE; pos += 1) {  // goes from SERVO_IN_ANGLE to SERVO_OUT_ANGLE in steps of 1 degree
                          myservo.write(pos);  // tell servo to go to position in variable 'pos'
                          delay(15);           // waits 15ms for the servo to reach the position
                       }
                }
                else if (results.value == button2) {
                     
                    for (pos = SERVO_OUT_ANGLE; pos >= SERVO_IN_ANGLE; pos -= 1) {  // goes from SERVO_OUT_ANGLE to SERVO_IN_ANGLE
                           myservo.write(pos);                  // tell servo to go to position in variable 'pos'
                           delay(15);                           // waits 15ms for the servo to reach the position
                  }   }        
   }
                
 irrecv.resume();

}