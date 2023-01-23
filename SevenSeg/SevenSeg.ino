

#define LED_PIN 12 //external LED
#define SWITCH_PIN 10 //switch connection

void setup() {
  pinMode(SWITCH_PIN, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  
  //declare pins as array so they can be passed to functin easier
  int DISPLAY_PINS[8] = {3, 2, 4, 5, 6, 8, 9, 7};
  for(int i = 0; i < 8; i++) {//need a loop to set all outputs
    pinMode(DISPLAY_PINS[i], OUTPUT); //setting pins 2-9 as output
  }
  Serial.begin(9600);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(LED_BUILTIN, LOW);
  Serial.println("Flip the switch to blink the LED");
  Serial.println("After 15 flips, the count will reset");
  serialOut(0);//prints count 0 to serial monitor
}
 
void loop() {//this loop will only execute once
  int DISPLAY_PINS[8] = {3, 2, 4, 5, 6, 8, 9, 7};//must re-initialize
  int last_switch_position = 0;
  int count = 0;
    while (1) {//to avoid re-initializing variables
      SSDisplay(DISPLAY_PINS, count);//always display a number on 7-seg
      int switch_position = digitalRead(SWITCH_PIN);
         if (digitalRead(SWITCH_PIN) != last_switch_position) {//checks for switch flip
         count += 1;//increment count if switch change detected
            if(count == 16){
               count = 0;//reset count once it reaches max value 15 (F)
            }
         last_switch_position = switch_position;//update last_switch_position
         serialOut(count);//output the current count to serial monitor
         }
         if (switch_position == 1) { //if switch high, blink LED
            ledOn();//function to blink LED
         }
   }
}

  int SSDisplay(int *Pins, int count) {//7-segment display function to index num/letter
  int row = count % 16;//another check to make sure count <= 16
  //Digits is a matrix used to index the correct number to be displayed on the 7-segment
  //each array in th ematrix corresponds to numbers 0-9 and letters A-F
  int Digits[16][8] =  {{1, 1, 1, 1, 1, 1, 0, 1},  //0 
      {0, 1, 1, 0, 0, 0, 0, 1},  {1, 1, 0, 1, 1, 0, 1, 1},   {1, 1, 1, 1, 0, 0, 1, 1}, //1,2,3 
      {0, 1, 1, 0, 0, 1, 1, 1},  {1, 0, 1, 1, 0, 1, 1, 1},   {1, 0, 1, 1, 1, 1, 1, 1}, //4,5,6
      {1, 1, 1, 0, 0, 0, 0, 1},  {1, 1, 1, 1, 1, 1, 1, 1},   {1, 1, 1, 0, 0, 1, 1, 1}, //7,8,9
      {1, 1, 1, 0, 1, 1, 1, 0},  {1, 1, 1, 1, 1, 1, 1, 0},   {1, 0, 0, 1, 1, 1, 0, 0}, //A,B,C
      {1, 1, 1, 1, 1, 1, 0, 0},  {1, 0, 0, 1, 1, 1, 1, 0},   {1, 0, 0, 0, 1, 1, 1, 0}};//D,E,F
      
  //loops through all 8 elements of the selected num/letter and writes the corresponding
  //0 or 1 to the output display
  for(int i = 0; i < 8; i++) {
    digitalWrite(Pins[i], Digits[row][i]);
  }
  return 0;
}

void ledOn() {//blink-blink-off 2 seconds
    delay(50);
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(80);
    digitalWrite(LED_PIN, LOW);
    digitalWrite(LED_BUILTIN, LOW);
    delay(80);
    digitalWrite(LED_PIN, HIGH);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(80);
    digitalWrite(LED_PIN, LOW);
    digitalWrite(LED_BUILTIN, LOW);
    delay(2000);
}

void serialOut(int count) {//serial display count in hex and decimal
      Serial.print("count = (decimal) ");
      Serial.print(count);
      Serial.print(" (hex) ");
      Serial.print(count, HEX);
      Serial.println();
}

    
