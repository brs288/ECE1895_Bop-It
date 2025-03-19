// One time setup
void setup() {
  pinMode(8, OUTPUT);
  pinMode(A5, INPUT);
}

// Loop through
void loop() {
    if (digitalRead(A5) == LOW) {
        digitalWrite(8, HIGH);
        delay(1000);
        digitalWrite(8, LOW);
        delay(1000);
    }
}
// Jack Bonney, Assignment 15
// Connor Murray was here!