

#include <avr/wdt.h>
#define analogPin A0

void setup () {
pinMode(analogPin, INPUT);
wdt_disable();//disable the WDT
Serial.begin (9600);
Serial.println("Resetting board");
Serial.println("Press 'c' to begin conversions");
while (!Serial);//wait for serial monitor 
wdt_enable(WDTO_4S);//enable WDT to reset after 4 s
wdt_reset();//reset WDT
}

void loop () {

  static int analogVal = 0;//value of analog read
  static float conversionTime = 0;//time for each conversion
  static float avgTime = 0;//avg conversion time
  static float totalTime;//set to usecs
  static String userInput;//reads input to serial monitor
  String letter = "c";//to compare to userInput
  
   if(Serial.available() > 0){ //if serial monitor ready
      wdt_enable(WDTO_4S);//start WDT
      userInput = Serial.readStringUntil('\n');//take input
      wdt_disable();//disable WDT after input detected
      if(userInput == letter) {//if user input == 'c'
        Serial.println("A series of 25 conversions is about to begin");
        for (int count = 0; count < 25; count++) {//start conversions
          totalTime = micros();//set total time to usecs
          analogVal = analogRead (analogPin);//read pin A0
          conversionTime = micros()-totalTime;//time passed
          avgTime+=conversionTime;//get total time 
          delay(250);
          
          Serial.print("#");
          Serial.print(count+1);
          Serial.print(": ");
     
          Serial.print(" Digital value = ");
          Serial.print(analogVal, HEX);
     
          Serial.print("  Time = ");
          Serial.print(conversionTime);
          Serial.println(" usecs");
          
          if (count == 24) {//final count val
            Serial.print("Average Conversion Time = ");
            Serial.print(avgTime/25, 2);//divide total time by 25
            Serial.println(" usecs");
            avgTime = 0;//reset avg time
          }
       }
    }
    else {//if invalid input
      Serial.println("ERROR: Invalid Input.");
    }
    wdt_reset();//reset WDT
    wdt_enable(WDTO_4S);//enable WDT before loop starts again
  }
 }
 
