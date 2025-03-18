// One time setup
void setup() {
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(A5, INPUT);
}

// Loop through
void loop() {
    if (digitalRead(A5) == LOW) {
        digitalWrite(6, HIGH);
        delay(1000);
        digitalWrite(6, LOW);
        delay(1000);
        digitalWrite(7, HIGH);
        delay(1000);
        digitalWrite(7, LOW);
        delay(1000);
        digitalWrite(8, HIGH);
        delay(1000);
        digitalWrite(8, LOW);
        delay(1000);
    }
}
// Jack Bonney Assignment 15
