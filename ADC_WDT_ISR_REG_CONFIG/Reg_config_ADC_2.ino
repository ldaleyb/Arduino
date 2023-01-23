
#include <avr/wdt.h>

//declare adc so it can be used in ISR
volatile int adc;
volatile int reading;
volatile int complete=0;


void setup() {
  Serial.begin(9600);
  wdt_disable();
  Serial.println("Board was reset");
  Serial.println();
  Serial.print("Select a type of conversion to perform ADC ('a' for ");
  Serial.println("built in analog read, 'b' for polling and port manipulation, or 'c' for ISR config)");      
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
      if(userInput == "c"){
        //user function for option c
        optionC(prevTime, analogVal);     
      }  
      else{
        prevTime = millis();//restarts timer 
        Serial.print("ERROR: Invalid user input - ");
        Serial.println("please select 'a', 'b', or 'C'");
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
/*user function for option b configures arduino's registers 
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
          //setting ADSC bit starts conversions
          ADCSRA |= _BV(ADSC); //bitwise inclusive or ADCSRA = ADCSRA | ADSC
          long convTime = micros();//initialize convTime for each iteration
          while(bit_is_set(ADCSRA,ADSC)){//conversion starts if ADSC is set
            //polling loop to check if conversion complete
          }
          //read low first so new val isn't written before current one 
          analogVal = ADCL | (ADCH << 8); 
          convTime = (micros()-convTime);//subtract starting time
          
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
/*user function for option c configures arduino's registers 
to perform an analog read using interrupts
and computes both conversion and average conversion times*/
long optionC(long prevTime, int analogVal) {
    long avgTime = 0;
    Serial.print("Starting a set of 25 conversions using ");
    Serial.println("interrupts:");
    ADCSRA |= _BV(ADEN);//enables the ADC
    ADMUX = _BV(REFS0);//Sets the ref voltage to 5V
    //set prescaler to 128 & ADIE for interrupt enable (ADIF = 0)
    ADCSRA |= _BV(ADIE) | _BV (ADPS2) | _BV (ADPS1) | _BV (ADPS0);
      for(int x=0; x<25; x++){
    
          //setting ADSC starts conversions
          ADCSRA |= _BV(ADSC); //bitwise inclusive or ADCSRA = ADCSRA | ADSC
          
          //conv starts if ADSC set(ADCSRA == B11001111)
        long convTime = micros();//initialize convTime for each iteration
        while(ADCSRA & (1<<ADSC)) {}//ADCSRA & (1<<ADSC)
        
           if(complete) {
            reading = 0;
            complete = 0;
           }
           convTime = (micros()-convTime);//subtract starting time
           if (reading==0) {
            reading = 1;
            ADCSRA |= _BV(ADSC) | _BV(ADIE);
           }
             
          //analogVal = ADCL | (ADCH << 8);//store in local variable analogVal
          analogVal = adc;
         
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
            ADCSRA &= ~(ADIE);//turn off ADC ISR bit
            loop();
          }
     }
}

ISR(ADC_vect) {
  //read LOW reg first then HIGH for analog val
  //ADC can't write to either reg until ADCH read
  adc = ADCL | (ADCH << 8);
  
  //reset interrupt flag bit to 1 
  //may not be needed based on documentation
  //ADCSRA |= _BV(ADIF);
  complete = 1;
}
