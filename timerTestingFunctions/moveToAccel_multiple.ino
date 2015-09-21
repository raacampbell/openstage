// TESTING FUNCTION:
// Purpose Move multiple motors to a position using the timeers

//The counter for steps taken is a long (2^32) and will allow us
//to travel up to about 8.4 meters in one direction even with a 125 
//mics/rev micrometer, 1000 steps/rev stepper, 32 microsteps. So no
//wrapping problems. 

//motor settings (in counts)
const int nSteppers=3; //how many steppers to cycle through
volatile long stepsToGo[4]  = {0,0,0,0};
volatile long targetPos[4]  = {19600,9600,9600,3600};//one rev at 16 mic steps is 3200 pulses
volatile long currentPos[4] = {0,0,0,0};
volatile char direction[4]  = {1,1,1,1}; 
float spd=17000;
volatile float maxSpeed[4]  = {spd,spd,spd,spd}; //Counts/s
float acc=500000;
volatile float accel[4]     = {acc,acc,acc,acc} ; //Counts/s/t [Unsure of the scale]




void setup()
{
  //Set up outputs
   pinMode(13, OUTPUT);
   pinMode(12, OUTPUT);
   pinMode(11, OUTPUT);
   pinMode(10, OUTPUT);
   pinMode(53, OUTPUT);
   pinMode(51, OUTPUT);

    //set up microstepping [this is just one motor]
    int MS1=7;
    int MS2=6;
    int MS3=5;
    pinMode(MS1,OUTPUT);
    pinMode(MS2,OUTPUT);
    pinMode(MS3,OUTPUT);
    
    digitalWrite(MS1,HIGH);
    digitalWrite(MS2,HIGH);
    digitalWrite(MS3,HIGH);
    

    cli();          // disable global interrupts

    //Zero registers
    TCCR1A = 0; TCCR1B = 0;     
    TCCR3A = 0; TCCR3B = 0;     
    TCCR4A = 0; TCCR4B = 0;     
    TCCR5A = 0; TCCR5B = 0;     

    // initialize CTC mode for each timer
    TCCR1B |= (1 << WGM12); // Timer1
    TCCR3B |= (1 << WGM32); // Timer3
    TCCR4B |= (1 << WGM42); // Timer4
    TCCR5B |= (1 << WGM52); // Timer5

    sei();  //enable global interrupts
    Serial.begin(9600);
}



// *****************************
void loop(){
 runToPosition();
 delay(1500);
}




long stepsLeft(){
   for (int ii=0; ii<nSteppers; ii++){
    stepsToGo[ii]=targetPos[ii]-currentPos[ii];
   }
}


//Blocking motion     
void runToPosition(){
  bool verbose=0;

  stepsLeft();    

  float cur=500;
  float currentSpeed[4]={cur,cur,cur,cur}; //starting speed


  //Set timers to this speed but don't enable them
  for (int ii=0; ii<nSteppers; ii++){
    setTimerFreq(currentSpeed[ii],ii);
  }
  if (verbose){
      Serial.println("Timers set");
  }
    
  //How many steps needed to reach desired speed
  long stepsToSpeed[nSteppers];
  if (verbose){
    Serial.print("Steps to speed: ");
  }

  for (int ii=0; ii<nSteppers; ii++){    
    stepsToSpeed[ii]= (maxSpeed[ii]*maxSpeed[ii])/(2.0 * accel[ii]);
    if (verbose){
      Serial.print(stepsToSpeed[ii]);
      Serial.print(" ");
    }
  }
  if (verbose){
    Serial.println("");
  }

  //Steps to reach this speed
  float changeSpeedSize[4];
  //long changeEvery=3; //Every so many steps we update the speed
  //We update speed by this much
  if (verbose)
    Serial.print("changeSpeedSize set: ");
  for (int ii=0; ii<nSteppers; ii++){
      changeSpeedSize[ii] = round( (maxSpeed[ii]/stepsToSpeed[ii]));
      if (verbose){
        Serial.print(changeSpeedSize[ii]);
        Serial.print(" ");
      }
  }
  if (verbose)
     Serial.println("");



  //enable timers
  if (stepsToGo[0]!=0)
    TIMSK1 |= (1 << OCIE1A);
  if (stepsToGo[1]!=0)
    TIMSK3 |= (1 << OCIE3A);
  if (stepsToGo[2]!=0)
    TIMSK4 |= (1 << OCIE4A);
  if (stepsToGo[3]!=0)
    TIMSK5 |= (1 << OCIE5A);

  //At this point, the values in stepsToGo are incrementing because the
  //ISRs are chipping away at them
 

  bool reachedSpeed[4]={0,0,0,0};
  
  //WHILE LOOP FOR RUNNING TO POSITION
  int updateStyle=1;

  //Set up timers for wait without delay
  long updateDelay=1;
  long previousUpdate=0;


  while (!areWeThereYet()){
      
/*
      switch(updateStyle){
      case 0:
      //Speed up
      for (int ii=0; ii<nSteppers; ii++){        
       if (stepsToGo[ii] % changeEvery == 0 && 
           currentSpeed[ii]<maxSpeed[ii] &&
           !reachedSpeed[ii]) {
         currentSpeed[ii]+=changeSpeedSize[ii];
         setTimerFreq(currentSpeed[ii],ii);
         if (verbose){
          Serial.print("Speeding up: ");  Serial.print(ii);
          Serial.print(" at: ");  Serial.println(currentSpeed[ii]);
          }//verbose
        }
        if (currentSpeed[ii]>=maxSpeed[ii]){
          reachedSpeed[ii]=1;
        }
      } //Accel for loop
      
      //Slow down
      for (int ii=0; ii<nSteppers; ii++){
       if (stepsToGo[ii] % changeEvery == 0 && 
           currentSpeed[ii]>50 && 
           stepsToGo[ii]<=stepsToSpeed[ii] &&
           reachedSpeed[ii]) {
          if (verbose){
          Serial.print("Slowing: ");   Serial.print(ii);
          Serial.print(" to: ");   Serial.println(currentSpeed[ii]);
          }//verbose 
         currentSpeed[ii]-=changeSpeedSize[ii];
         setTimerFreq(currentSpeed[ii],ii);
        } //if statement
      } //Decel for loop
     
      
      if (stepsToGo[0] % 5 == 0){
        if (verbose){
          Serial.print("Steps to go 0");
          Serial.println(stepsToGo[0]);
         } //verbose
      }
    
    break;
   case 1:
   */
    unsigned long currentMillis=millis();

    if (currentMillis-previousUpdate>updateDelay){
      previousUpdate = currentMillis;

    PORTB  |= bit(0);//switch on pin 53   
    for (int ii=0; ii<nSteppers; ii++){        
       if (currentSpeed[ii]<maxSpeed[ii] &&
           !reachedSpeed[ii]) {
         currentSpeed[ii]+=changeSpeedSize[ii];
         PORTB  |= bit(2);//switch on pin 51
         setTimerFreq(currentSpeed[ii],ii);
         PORTB  &= ~bit(2);//switch off pin 51  
        }
        
        if (currentSpeed[ii]>=maxSpeed[ii]){
          reachedSpeed[ii]=1;
        }

        if (currentSpeed[ii]>50 && 
           stepsToGo[ii]<=stepsToSpeed[ii] &&
           reachedSpeed[ii]) {
         currentSpeed[ii]-=changeSpeedSize[ii];
         PORTB  |= bit(2);//switch on pin 51
         setTimerFreq(currentSpeed[ii],ii);
         PORTB  &= ~bit(2);//switch off pin 51  

        } //if statement

    } 
      PORTB  &= ~bit(0);//switch off pin 53     

      previousUpdate=millis(); 
      } //DELAY IF
  // break;
 // } //Switch


  } //while are we there yet 

  if (verbose){
    Serial.println("GOT THERE!");
  }

} //runToPosition()
        
        

bool areWeThereYet(){
  long distance=0;
  for (int ii=0; ii<nSteppers; ii++){
    distance=distance+abs(stepsToGo[ii]);
  }
  //Serial.println(distance);
  bool out=distance;
  return !out;
}


        
//Set the timer frequency
void setTimerFreq(float frequency, int timer){
  bool verbose=0;
  if (frequency<0.25){
     frequency=0.25;
  }

  uint8_t prescalarbits = 0b001;//default to no prescaler    
  // two choices for the 16 bit timers: ck/1 or ck/64
  uint32_t ocr = F_CPU / (frequency*0.5) / 2 - 1;
  
  int ps=1;
  
  if (ocr > 65535) //If it over-flows go to 64 prescaler
  {
    ps=64;
    ocr = F_CPU / (frequency*0.5) / 2 / ps - 1;
    prescalarbits = 0b011;
  }
  
  if (ocr > 65535) //If it over-flows go to 64 prescaler
  {
    ps=1024;
    ocr = F_CPU / (frequency*0.5) / 2 / ps - 1;
    prescalarbits = 0b101;
  }
  
  if (verbose){
    Serial.print("f:");  Serial.print(frequency);
    Serial.print(" ");   Serial.print(ps);
    Serial.print(" ");   Serial.println(ocr);
    }

  //If it's running only proceed with change when counter register is at zero
  //  signal choppy without this line
  switch (timer){
   case 0:      
      if (TIMSK1 == bit(OCIE1A))
         while(TCNT1>round(ocr/3.0)){  }
      TCCR1B = (TCCR1B & 0b11111000) | prescalarbits; 
      OCR1A = ocr; 
      break;
   case 1:
      if (TIMSK3 == bit(OCIE3A))
         while(TCNT3>round(ocr/3.0)){  }
      TCCR3B = (TCCR3B & 0b11111000) | prescalarbits; 
      OCR3A = ocr; 
      break;
   case 2:
      if (TIMSK4 == bit(OCIE4A))
         while(TCNT4>round(ocr/3.0)){  }
      TCCR4B = (TCCR4B & 0b11111000) | prescalarbits; 
      OCR4A = ocr; 
      break;
   case 3:
      if (TIMSK5 == bit(OCIE5A))
         //while(TCNT5>0){  }
      TCCR5B = (TCCR5B & 0b11111000) | prescalarbits; 
      OCR5A = ocr; 
      break;

  }//switch (timer)

 
}  






//Interrupt Service Request 1
ISR(TIMER1_COMPA_vect)
{
    PORTB  |= bit(7);//switch on pin 13
    stepsToGo[0]--;
    if (stepsToGo[0]<=0){
          TIMSK1 &= ~(1 << OCIE1A);
    }  
    PORTB  &= ~bit(7);//switch off pin 13     
}

//Interrupt Service Request 2
ISR(TIMER3_COMPA_vect)
{
    PORTB  |= bit(6);//switch on pin 12
    stepsToGo[1]--;
    if (stepsToGo[1]<=0){
          TIMSK3 &= ~(1 << OCIE3A);
    }  
    PORTB  &= ~bit(6);//switch off pin 12
}



//Interrupt Service Request 3
ISR(TIMER4_COMPA_vect)
{
    PORTB  |= bit(5);//switch on pin 11
    stepsToGo[2]--;
    if (stepsToGo[2]<=0){
          TIMSK4 &= ~(1 << OCIE4A);
    }  
    PORTB  &= ~bit(5);//switch off pin 11
}


//Interrupt Service Request 4
ISR(TIMER5_COMPA_vect)
{
    PORTB  |= bit(4);//switch on pin 10
    stepsToGo[3]--;
    if (stepsToGo[3]<=0){
          TIMSK5 &= ~(1 << OCIE5A);
    }  
    PORTB  &= ~bit(4);//switch off pin 10
}
