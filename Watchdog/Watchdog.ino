

#include <avr/wdt.h>

// Global Variable used for Watchdog Timer 
volatile int resetFlag;

void setup() {
  // Disable watchdog 
  wdt_disable();

  // Initialize 7-Segment Display Pins
  int DISPLAY_PINS[8] = {3, 2, 4, 5, 6, 8, 9, 7};
  for(int i = 0; i < 8; i++) {//need a loop to set all outputs
    pinMode(DISPLAY_PINS[i], OUTPUT); //setting pins 2-9 as output
  }

  watchdogSetup();  // Setup Watchdog Timer

  // Set 7-Segment Display to "-"
  //ssd(DISPLAY_PINS[3],"-");
  digitalWrite(DISPLAY_PINS[6], 1);
  resetFlag = 0;

  Serial.begin(9600); // Initialize serial comm
  while (!Serial) {
    delay(50); // Wait for connect
  }
  initialPrompt(); //sending input prompt to user
  wdt_reset();
}

void loop() { 
  int DISPLAY_PINS[8] = {3, 2, 4, 5, 6, 8, 9, 7};//must re-initialize
  
  if (resetFlag) {
    // reset after 4 seconds
    watchdogArm(); 
    blinkLED();  // Blink Decimal Point LED 
  }
  
  else {  // resetFlag false
    static String inputString;  //user input
  
    //reaction time variables
    static float reactionTime;
    static float totalTime = millis();

    // wait for user input
    while (Serial.available() > 0) {    
      inputString = Serial.readStringUntil('\n');
  
      wdt_reset();
      reactionTime = millis() - totalTime;
  
      // 7-seg display output
      ssd(DISPLAY_PINS, inputString);
      
      // Check if input is 0-15
      if (inputString.toInt()%16 == inputString.toInt()) {
        // print input and reaction time
        printInput(inputString);
        printTime(reactionTime);
      }
      else {   // If input is invalid
        Serial.println("ERROR: Invalid input. "
          "Expected values from 0 to 15.");
      }     
      initialPrompt(); // Send prompt again
      wdt_reset();  // Reset WDT
      totalTime = millis();  
    }
  }
}
// 4 second interrupts with no reset
void watchdogSetup() {
  WDTCSR |= 0b00011000;
  WDTCSR = (1<<WDIE) | (0<<WDE) | (1<<WDP3);
}

void watchdogArm() {
  // Watchdog Config
  WDTCSR |= 0b00011000;
  // no reset
  WDTCSR = (1<<WDIE) | (1<<WDE);
  // Timeout 4 seconds
  wdt_enable(WDTO_4S);
}

//WDT ISR 
ISR(WDT_vect) {
  if (resetFlag == 0) {
    resetFlag = 1;
    WDTCSR |= (1<<WDIE);
  } 
}

void blinkLED() {
  int DISPLAY_PINS[8] = {3, 2, 4, 5, 6, 8, 9, 7};//must re-initialize
  
  Serial.println(" TIME OUT OCCURRED ");  // Interrupt occurred
  Serial.println("More than 4 seconds passed without "
    "user input.");
  Serial.println(" RESETTING BOARD ");
  
  // Clear 7-Segment Display
  ssd(DISPLAY_PINS, "LOW");
  
  delay(50);
  // Blink Decimal Point on/off for 5 seconds
  for(int i=0; i<5; i++){
    digitalWrite(DISPLAY_PINS[7],1);
    delay(500);
    digitalWrite(DISPLAY_PINS[7],0);
    delay(500);
  }
  //wait for timer reset
  delay(2000);
}


int ssd(int *Pins, String count) {//7-segment display function to index num/letter
  
  //Digits is a matrix used to index the correct number to be displayed on the 7-segment
  //each array in th ematrix corresponds to numbers 0-9 and letters A-F
  int Digits[18][8] =  {{1, 1, 1, 1, 1, 1, 0, 1},  //0 
      {0, 1, 1, 0, 0, 0, 0, 1},  {1, 1, 0, 1, 1, 0, 1, 1},   {1, 1, 1, 1, 0, 0, 1, 1}, //1,2,3 
      {0, 1, 1, 0, 0, 1, 1, 1},  {1, 0, 1, 1, 0, 1, 1, 1},   {1, 0, 1, 1, 1, 1, 1, 1}, //4,5,6
      {1, 1, 1, 0, 0, 0, 0, 1},  {1, 1, 1, 1, 1, 1, 1, 1},   {1, 1, 1, 0, 0, 1, 1, 1}, //7,8,9
      {1, 1, 1, 0, 1, 1, 1, 0},  {1, 1, 1, 1, 1, 1, 1, 0},   {1, 0, 0, 1, 1, 1, 0, 0}, //A,B,C
      {1, 1, 1, 1, 1, 1, 0, 0},  {1, 0, 0, 1, 1, 1, 1, 0},   {1, 0, 0, 0, 1, 1, 1, 0}, //D,E,F
      {1, 0, 0, 1, 0, 0, 1, 0},  {0, 0, 0, 0, 0, 0, 0, 0}}; // "-", "."

    
  if (count == "-") {
    for (int j = 0; j < 8; j++) {
      digitalWrite(Pins[6], Digits[17][j]);    
    }
  }
  else if (count == "LOW") {
    for (int p = 0; p < 8; p++) {
      digitalWrite(Pins[p], Digits[17][p]);    
    }
  }
  else if (count.toInt() > 15) {
    for (int j = 0; j < 8; j++) {
      digitalWrite(Pins[j], Digits[16][j]);
    }
  } 
  //loops through all 8 elements of the selected num/letter and writes the corresponding
  //0 or 1 to the output display
  else if (count.toInt() <= 15){
    for(int i = 0; i < 8; i++) {
    digitalWrite(Pins[i], Digits[count.toInt()][i]);
  }
  }
  return 0;
}

void initialPrompt() {
  Serial.println("\nPlease enter a number between 0 and 15:");
}

void printTime(float reactionTime) {
  Serial.print("reaction time = ");
  Serial.print((reactionTime/1000), 3);
  Serial.print("\n");
}


void printInput(String num) {
  Serial.print("Your input = (hex) ");
  Serial.print(num.toInt(), HEX);
  Serial.print("\n");
}
