void setup() {
  Serial.begin(9600);
}

void loop() {
  if (Serial.available()) {
    String message = Serial.readString();
    Serial.print("Received message: ");
    Serial.println(message);
  }
  delay(1000);
}