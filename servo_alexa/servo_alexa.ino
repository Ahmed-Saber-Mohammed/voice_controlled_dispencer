#include <IRremote.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

Servo myservo;  // create a servo object

#define initial_degree 90
#define final_degree 120


// Pin assignments
#define IR_SENSOR 51
#define IR_RECEIVER 53
#define UPPER_LIMIT_SWITCH 49
#define LOWER_LIMIT_SWITCH 47
#define GEAR_MOTOR_IN1 37
#define GEAR_MOTOR_IN2 35
#define PUMP1_ENABLE 8
#define PUMP1_IN3 50
#define PUMP1_IN4 48
#define PUMP2_ENABLE 9
#define PUMP2_IN1 44
#define PUMP2_IN2 42
#define PUMP3_ENABLE 10
#define PUMP3_IN3 38
#define PUMP3_IN4 36
#define PUMP4_ENABLE 11
#define PUMP4_IN1 32
#define PUMP4_IN2 30
#define LCD_ADDR 0x27
#define LCD_COLS 16
#define LCD_ROWS 2

// Pump duration times in milliseconds
#define PUMP1_DURATION 100
#define PUMP2_DURATION 500
#define PUMP3_DURATION 300
#define PUMP4_DURATION 200

// Drink names
String drinks[] = {"Drink1", "Drink2", "Drink3", "Drink4"};

// IR remote setup
IRrecv irrecv(IR_RECEIVER);
decode_results results;
unsigned long button1 = 0xFF30CF;
unsigned long button2 = 0xFF18E7;
unsigned long button3 = 0xFF7A85;
unsigned long button4 = 0xFF10EF;

// LCD setup
LiquidCrystal_I2C lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

int selectedDrink = -1;
int pumpPin[] = {PUMP1_ENABLE, PUMP2_ENABLE, PUMP3_ENABLE, PUMP4_ENABLE};
int pumpIn1Pin[] = {PUMP1_IN3, PUMP2_IN1, PUMP3_IN3, PUMP4_IN1};
int pumpIn2Pin[] = {PUMP1_IN4, PUMP2_IN2, PUMP3_IN4, PUMP4_IN2};
int PUMP_DURATION[] = {PUMP1_DURATION, PUMP2_DURATION, PUMP3_DURATION, PUMP4_DURATION};


void setup() {
  // Initialize serial communication
  Serial.begin(9600);

  // Initialize IR remote
  irrecv.enableIRIn();

 myservo.attach(13);  // attaches the servo on pin 13 to the servo object
 
  // Initialize motor control pins
  pinMode(GEAR_MOTOR_IN1, OUTPUT);
  pinMode(GEAR_MOTOR_IN2, OUTPUT);
  for (int i=0; i<4; i++) {
    pinMode(pumpPin[i], OUTPUT);
    pinMode(pumpIn1Pin[i], OUTPUT);
    pinMode(pumpIn2Pin[i], OUTPUT);
  }

  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Ready");

  // Initialize gear motor
  digitalWrite(GEAR_MOTOR_IN1, LOW);
  digitalWrite(GEAR_MOTOR_IN2, HIGH);

  // Initialize limit switches as input pull-up
  pinMode(UPPER_LIMIT_SWITCH, INPUT_PULLUP);
  pinMode(LOWER_LIMIT_SWITCH, INPUT_PULLUP);
}
void loop() {
  // Check for glass presence
  int irValue = digitalRead(IR_SENSOR);
  if (irValue == HIGH) {
    lcd.setCursor(0, 0);
    lcd.print("Ready");
    lcd.setCursor(0, 1);
    lcd.print(" ");
    selectedDrink = -1;
  } else {
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
        } else if (results.value == button2) {
          selectedDrink = 1;
        } else if (results.value == button3) {
          selectedDrink = 2;
        } else if (results.value == button4) {
          selectedDrink = 3;
        }
        irrecv.resume();
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

    // Lower glass to dispensing position
    digitalWrite(GEAR_MOTOR_IN1, HIGH);
    digitalWrite(GEAR_MOTOR_IN2, LOW);
    
   for (int pos = initial_degree; pos <= final_degree; pos += 1) {  // goes from initial_degree to final_degree degrees
    myservo.write(pos);       // tell servo to go to position in variable 'pos'
    delay(15);                 // waits 15ms for the servo to reach the position
  }
  
    while (digitalRead(LOWER_LIMIT_SWITCH) == HIGH) {
      delay(10);
    }
    digitalWrite(GEAR_MOTOR_IN1, LOW);
    digitalWrite(GEAR_MOTOR_IN2, HIGH);

    // Dispense ingredients
    digitalWrite(pumpIn1Pin[selectedDrink], HIGH);
    digitalWrite(pumpIn2Pin[selectedDrink], LOW);
    digitalWrite(pumpPin[selectedDrink], HIGH);
    delay(PUMP_DURATION[selectedDrink]);
    digitalWrite(pumpPin[selectedDrink], LOW);
    digitalWrite(pumpIn1Pin[selectedDrink], LOW);
    digitalWrite(pumpIn2Pin[selectedDrink], LOW);

    // Raise glass back to top position
    digitalWrite(GEAR_MOTOR_IN1, HIGH);
    digitalWrite(GEAR_MOTOR_IN2, LOW);
    
for (int pos = final_degree; pos >= initial_degree ; pos -= 1) {  // goes from final_degree degrees to initial_degree 
    myservo.write(pos);       // tell servo to go to position in variable 'pos'
    delay(15);                 // waits 15ms for the servo to reach the position
  }
  
    while (digitalRead(UPPER_LIMIT_SWITCH) == HIGH) {
      delay(10);
    }
    digitalWrite(GEAR_MOTOR_IN1, LOW);
    digitalWrite(GEAR_MOTOR_IN2, HIGH);

    // Reset selected drink
    selectedDrink = -1;
  }
}
