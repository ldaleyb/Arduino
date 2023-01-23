

#include <avr/wdt.h>

void setup() {
  Serial.begin(9600);
  wdt_disable();
  Serial.println("Board was reset");
  Serial.println();
  Serial.print("Select a type of conversion to perform ('a' for ");
  Serial.println("built-in analog read, 'b' for register config)");
}
void loop() {
  String userInput;
  static long prevTime = millis();//keep track of prevTime
  int analogVal;
  //while loop so that main loop is not executed each time
  while(1){//wait for serial monitor to become available
    if(Serial.available() > 0){
      //wait for user input
      userInput = Serial.readStringUntil('\n');
      if(userInput == "a"){
        //user function for option a 
        optionA(prevTime, analogVal);
      }
      if(userInput == "b"){
        //user function for option b
        optionB(prevTime, analogVal);     
      }   
      else{
        prevTime = millis();//restarts timer 
        Serial.print("ERROR: Invalid user input - ");
        Serial.println("please select 'a' or 'b'");
      }
    }
    //4 second timer before resetting board
    if((millis() - prevTime) >= 4000){
      Serial.println();
      //enables the WDT to run for 120ms
      wdt_enable(WDTO_120MS);
      while(1){}
     }
  }
}
/*function to print the digital value
and format the output to 3 HEX digits */
int printDigitalVal(int x, int analogVal) {
      Serial.print("#");
      Serial.print(x+1);
      if(x<9)Serial.print(":   digital value = ");
      else Serial.print(":  digital value = ");
      //formats output to 3 HEX digits 
      if(analogVal < 256){
         Serial.print(0);
         if(analogVal < 16) {
          Serial.print(0);
         }
      }
      Serial.print(analogVal, HEX);
      
}
//function to print the average conversion time
long printAvgTime(long avgTime) {
      Serial.println();
      Serial.print("Average conversion time = ");
      Serial.print(avgTime/25.00);
      Serial.println(" usecs");
      Serial.println();
}

//function to print the conversion time
void printConversionTime(long convTime) {
      Serial.print("  Time = ");
      Serial.print(convTime);
      Serial.println(" usecs");
}

/*user function for option a uses arduino's 
built-in analogRead function and computes both 
conversion and average conversion times*/
long optionA(long prevTime, int analogVal) {
     long avgTime = 0;
     Serial.println("Starting a set of 25 conversions using AnalogRead:");
     for(int x=0; x<25; x++){
         //re-initialize convTime each iteration
         long convTime = micros();
         //read analogValue 
         analogVal = analogRead(0);
         //subtract difference for convTime of analog read
         convTime = (micros()-convTime);
         delay(250);
         printDigitalVal(x, analogVal);
         printConversionTime(convTime);
         //sum avgTime --will be divided by 25 in print function
         avgTime += convTime;
           //on last conversion return to loop & reset prevTime
           if(x==24){
             printAvgTime(avgTime);
             prevTime = millis();
             //Flush serial buffer
             Serial.readString();
             loop();
           }
     }
}
/*user function for option b configres arduino's registers 
to perform an analog read using polling and port manipulation
and computes both conversion and average conversion times*/
long optionB(long prevTime, int analogVal) {
    long avgTime = 0;
    Serial.print("Starting a set of 25 conversions using ");
    Serial.println("polling and port manipulation:");
    ADCSRA |= _BV(ADEN);//enables the ADC
    ADMUX = _BV(REFS0);//Sets the ref voltage to 5V
    //set prescaler to 128
    ADCSRA |= _BV (ADPS2) | _BV (ADPS1) | _BV (ADPS0);
      for(int x=0; x<25; x++){
          //setting ADSC starts conversions
          ADCSRA |= _BV(ADSC); //bitwise inclusive or ADCSRA = ADCSRA | ADSC
          long convTime = micros();//initialize convTime for each iteration
          while(bit_is_set(ADCSRA,ADSC)){//conversion starts if ADSC is set
            //polling loop to check if conversion complete
          }
          convTime = (micros()-convTime);//subtract starting time
          int low  = ADCL;//Stores the ADC Low 
          int high = ADCH;//Stores the ADC High 
          //Shift high value 8 bits left & OR with low for analog val
          analogVal = (high << 8) | low;
          //Delays for time to change the value
          delay(250);
          printDigitalVal(x, analogVal);
          printConversionTime(convTime);
          //sum avgTime --will be divided by 25 in print function
          avgTime += convTime;
          //on last conversion return to loop & reset prevTime
          if(x==24){
            printAvgTime(avgTime);
            prevTime = millis();
            //Flush serial buffer
            Serial.readString();
            loop();
          }
     }
}
