#include <IRremote.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

Servo myservo;  // create a servo object

#define SERVO_PIN 9 // Servo motor pin
#define SERVO_IN_ANGLE 70 // Angle of the dispenser servo motor when it is hidden
#define SERVO_OUT_ANGLE 180 // Angle of the dispenser servo motor when it is dispensing


// Pin assignments
#define CUP_DETECT_PIN 51 // IR sensor
#define IR_RX_PIN 43
#define TOP_CONTACT_PIN 49 // Contact switch
#define BOT_CONTACT_PIN 47 // Contact switch
#define GEAR_MOTOR_PIN_1 44 // Motor driver pin #1 Raise drink carrier
#define GEAR_MOTOR_PIN_2 45 // Motor driver pin #2 Lower drink carrier

#define PUMP1_PIN 50


#define PUMP2_PIN 44


#define PUMP3_PIN 38


#define PUMP4_PIN 32

#define LCD_ADDR 0x27
#define LCD_COLS 16
#define LCD_ROWS 2

// Pump duration times in milliseconds
#define drinkOnePourSize 100
#define drinkTwoPourSize 500
#define drinkThreePourSize 300
#define drinkFourPourSize 200

// Drink names
String drinks[] = { "Drink1", "Drink2", "Drink3", "Drink4" };

// IR remote setup
IRrecv irrecv(IR_RX_PIN);
decode_results results;
unsigned long button1 = 0xFF30CF;
unsigned long button2 = 0xFF18E7;
unsigned long button3 = 0xFF7A85;
unsigned long button4 = 0xFF10EF;

// LCD setup
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

int selectedDrink = -1;

int pumpPin[] = { PUMP1_PIN, PUMP2_PIN, PUMP3_PIN, PUMP4_PIN };

int drinkPourSize[] = { drinkOnePourSize, drinkTwoPourSize, drinkThreePourSize, drinkFourPourSize };


void setup() {
    // Initialize serial communication
    Serial.begin(9600);

    // Initialize IR remote
    irrecv.enableIRIn();

    myservo.attach(SERVO_PIN);  // attaches the servo on pin SERVO_PIN to the servo object

    // Initialize motor control pins
    pinMode(GEAR_MOTOR_PIN_1, OUTPUT);
    pinMode(GEAR_MOTOR_PIN_2, OUTPUT);
    for (int i = 0; i < 4; i++) {
        pinMode(pumpPin[i], OUTPUT);

    }

    // Initialize LCD
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("Ready");

    // Initialize gear motor

    digitalWrite(GEAR_MOTOR_PIN_1, HIGH);
    digitalWrite(GEAR_MOTOR_PIN_2, LOW);

    while (digitalRead(TOP_CONTACT_PIN) == HIGH) {
            delay(10);
        }

           digitalWrite(GEAR_MOTOR_PIN_1, LOW);
           digitalWrite(GEAR_MOTOR_PIN_2, LOW); 

    // Initialize limit switches as input pull-up
    pinMode(TOP_CONTACT_PIN, INPUT_PULLUP);
    pinMode(BOT_CONTACT_PIN, INPUT_PULLUP);
}
void loop() {
    // Check for glass presence
    int irValue = digitalRead(CUP_DETECT_PIN);
    if (irValue == HIGH) {
        lcd.setCursor(0, 0);
        lcd.print("Ready");
        lcd.setCursor(0, 1);
        lcd.print(" ");
        selectedDrink = -1;
    }
    else {
        lcd.setCursor(0, 0);
        lcd.print("Glass Detected");
        lcd.setCursor(0, 1);
        lcd.print("Ready to Dispense");
        delay(1000);
        // Wait for user input
        while (selectedDrink == -1) {
            if (irrecv.decode(&results)) {
                if (results.value == button1) {
                    selectedDrink = 0;
                }
                else if (results.value == button2) {
                    selectedDrink = 1;
                }
                else if (results.value == button3) {
                    selectedDrink = 2;
                }
                else if (results.value == button4) {
                    selectedDrink = 3;
                }
                 
            }
            // Check serial input for selected drink
            if (Serial.available() > 0) {
                int drink = Serial.read() - '0';
                Serial.println(drink);
                if (drink >= 1 && drink <= 4) {
                    selectedDrink = drink - 1;
                }
            }
        }

        // Dispense drink
        lcd.setCursor(0, 0);
        lcd.print("Dispensing Drink");
        lcd.setCursor(0, 1);
        lcd.print(drinks[selectedDrink]);
        delay(1000);

        // Raise glass to dispensing position
        digitalWrite(GEAR_MOTOR_PIN_1, HIGH);
        digitalWrite(GEAR_MOTOR_PIN_2, LOW);

       

        while (digitalRead(BOT_CONTACT_PIN) == HIGH) {
            delay(10);
        }
        digitalWrite(GEAR_MOTOR_PIN_1, LOW);
        digitalWrite(GEAR_MOTOR_PIN_2, LOW);

         for (int pos = SERVO_IN_ANGLE; pos <= SERVO_OUT_ANGLE; pos += 1) {  // goes from SERVO_IN_ANGLE to SERVO_OUT_ANGLE degrees
            myservo.write(pos);       // tell servo to go to position in variable 'pos'
            delay(15);                 // waits 15ms for the servo to reach the position
        }

        // Dispense ingredients
        digitalWrite(pumpPin[selectedDrink], HIGH);


        delay(drinkPourSize[selectedDrink]);

        digitalWrite(pumpPin[selectedDrink], LOW);

        for (int pos = SERVO_OUT_ANGLE; pos >= SERVO_IN_ANGLE; pos -= 1) {  // goes from SERVO_OUT_ANGLE degrees to SERVO_IN_ANGLE 
            myservo.write(pos);       // tell servo to go to position in variable 'pos'
            delay(15);                 // waits 15ms for the servo to reach the position
        }

        // Lower glass back to top position
        digitalWrite(GEAR_MOTOR_PIN_1, LOW);
        digitalWrite(GEAR_MOTOR_PIN_2, HIGH);


        while (digitalRead(TOP_CONTACT_PIN) == HIGH) {
            delay(10);
        }
        digitalWrite(GEAR_MOTOR_PIN_1, LOW);
        digitalWrite(GEAR_MOTOR_PIN_2, LOW);

        // Reset selected drink
        selectedDrink = -1;
    }
}
