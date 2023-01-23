


#define LED_PIN 12 //external LED

void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  Serial.begin(9600);
  Serial.print("Press g to blink LED's and s to turn them off");
  digitalWrite(LED_PIN, LOW);
  digitalWrite(LED_BUILTIN, LOW);
}
  
void loop() {
  while (Serial.available() == 0) { //wait for input
    digitalWrite(LED_PIN, LOW);
    digitalWrite(LED_BUILTIN, LOW);
  }
  if (Serial.read()== 'g') {//read input from user
    while (Serial.read() != 's') {//while input is not s
            digitalWrite(LED_BUILTIN, HIGH);// turn the LED on                 
            digitalWrite(LED_PIN, LOW);    // turn the LED off 
            delay(2000); //hold 2 sec
            digitalWrite(LED_BUILTIN, LOW);           
            digitalWrite(LED_PIN, HIGH);    
            delay(1000); //hold 1 sec
            if (Serial.read() == 's') break; //turn off LED when user presses s
   }
  }   
}
