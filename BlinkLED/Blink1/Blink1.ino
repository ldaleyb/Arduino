


#define LED_PIN 12 //external LED
#define SWITCH_PIN 4

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(SWITCH_PIN, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(LED_BUILTIN, LOW);
}
  
void loop() {
  int switchVal;
  switchVal = digitalRead(SWITCH_PIN);
  
  if (switchVal == HIGH) {
    digitalWrite(LED_PIN, HIGH);
    delay(500);
    digitalWrite(LED_PIN, LOW);
    delay(500);
  }
  
  else {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  }
}
