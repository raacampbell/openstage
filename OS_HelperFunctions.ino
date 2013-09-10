//------------------------------------------------------------------------------------------------
// This file contains various peripheral functions required for OpenStage.ino. When using the 
// Arduino IDE it is automatically appended to OpenStage.ino during the Arduino compile process.
//
// The functions that do the heavy lifting and orchestrate the stage moves are located either in 
// their own file or in OpenStage.ino




//------------------------------------------------------------------------------------------------
// * lcdStagePos
// Show position of selected axis on LCD display
void lcdStagePos(byte axis,float pos, float speed){

  lcd.setCursor(2,axis);

  if (sgn(pos)>=0)
    lcd.print(" ");

  lcd.print(pos);
  if (pos<10)
    lcd.print(" ");

  if (pos<100)
    lcd.print(" ");

  if (pos>=10000){
    lcd.setCursor(8,axis);
    } else {
     lcd.setCursor(9,axis);
    }



  lcd.print(char(228));//mu
  lcd.print("m ");

  lcd.print(speed);
  if (speed<10)
    lcd.print(" ");
  if (speed<100)
    lcd.print(" ");
  if (speed<1000)
    lcd.print(" ");

} //End lcdStagePos()


//------------------------------------------------------------------------------------------------
// * setupLCD
// Print axis names on display 
void setupLCD(){
  char AX[4]={'X','Y','Z','z'};
  for (byte ii=0; ii<numAxes; ii++){
      lcd.setCursor(0,ii);
      lcd.print(AX[ii]);
      lcd.print(" ");
     }
} //End setupLCD()


//------------------------------------------------------------------------------------------------
// * updateStagePos
// updates axis by this number of steps. The sign must be specified.
//   axis - the integer defining the axis want to update
//   steps - how many steps the motor has travelled
//   stepSize - the micro-step size. e.g. 1/16.0 for the finest step size
void updateStagePos(int axis, long steps, float stepSize){
	stagePosition[axis] += steps * (fullStep[axis]/360) * stepSize * gearRatio[axis];
} //End updateStagePos()





//------------------------------------------------------------------------------------------------
// * dButtonMove
// Executes a move based upon a click of a D-Pad button. Uses globals from the top of the file to
// execute a motion of the appropriate magnitude, accelaration, and speed. 

void dButtonMove(int axis, int direction){
  byte verbose=0;
  
  if (verbose)
   Serial.println("Doing D-Button move");
 
  //Prepare for the step
  digitalWrite(stageLEDs[axis],HIGH);
  digitalWrite(enable[axis],LOW); //Ensure motor is enabled or there will be no motion
  setStep(DPadStepSize);
  
  (*mySteppers[axis]).setMaxSpeed(4.0E3);
  
  (*mySteppers[axis]).setAcceleration(DPadAccel[axis]);

  //Calculate the number of steps
  float move = DPadStep[coarseFine-1] / ((fullStep[axis]/360.0)*DPadStepSize*gearRatio[axis]);
  move=round(move * direction);

  //Perform a blocking motion to the new position. 
  (*mySteppers[axis]).runToNewPosition( (*mySteppers[axis]).currentPosition()+move);


  //Update the stage position
  updateStagePos(axis,move,DPadStepSize);
  stepperPreviousPos[axis]=(*mySteppers[axis]).currentPosition();
  lcdStagePos(axis,stagePosition[axis],-1);

  //Tidy up
  setStep(stepSize[coarseFine-1]);
  (*mySteppers[axis]).setAcceleration(0);
  
  if (disableWhenStationary[axis])
    digitalWrite(enable[axis],HIGH);

  digitalWrite(stageLEDs[axis],LOW);   
  setupLCD();//run here just in case this gets corrupted from time to time


} //End dButtonMove()




//------------------------------------------------------------------------------------------------
// * sgn
// Return sign of a long. Used to determine direction of motion. 
int sgn(long x){
   if (x > 0) return 1;
   if (x < 0) return -1;
   return 0;
} //End sgn()





//------------------------------------------------------------------------------------------------
// * beep
// Produces a beep from the Piezo of a given frequency (Hz) and duration (seconds)
void beep(int freq, float duration){
    int delay4Freq = 1E6/(freq*2);
 
    for (long i=0; i<freq*duration; i++ ){
       digitalWrite(beepPin, HIGH);
       delayMicroseconds(delay4Freq);
       digitalWrite(beepPin, LOW);
       delayMicroseconds(delay4Freq);
      }
} //End beep()





//------------------------------------------------------------------------------------------------
// setStep
// This function takes as its input a float defining the fractional step size. It 
// sets the microstep pins appropriately for the Big Easy Drivers to implement the 
// desired microstep size. 
void setStep(float stepMode)
{

  //Set the microstep pins on the Big Easy Driver
  //Seperate if statements, otherwise loop overhead will
  //vary with stepMode NOTE: if hardware timing works we can use switches
  if (stepMode==1.0/1.0)
  {
    digitalWrite(MS1,LOW);
    digitalWrite(MS2,LOW);
    digitalWrite(MS3,LOW);
  } 
  
  if (stepMode==(1.0/2.0))
  {
    digitalWrite(MS1,HIGH);
    digitalWrite(MS2,LOW);
    digitalWrite(MS3,LOW);
  }  

  
  if (stepMode==(1.0/4.0))
  {
   digitalWrite(MS1,LOW);
   digitalWrite(MS2,HIGH);
   digitalWrite(MS3,LOW);
  }
  
  if (stepMode==(1.0/8.0))
  {
    digitalWrite(MS1,HIGH);
    digitalWrite(MS2,HIGH);
    digitalWrite(MS3,LOW);
  }  
  
  if (stepMode==(1.0/16.0))
  {
   digitalWrite(MS1,HIGH);
   digitalWrite(MS2,HIGH);
   digitalWrite(MS3,HIGH);
  }

} //End setStep()





//------------------------------------------------------------------------------------------------
// * setPSLEDS
// The coarseFine variable defines the speed mode of the controller. This function
// updates the LEDs on the DualShock in order to reflect the currently chosen speed mode. 
void setPSLEDS(){
  PS3.setAllOff();
  switch (coarseFine){
    case 1:
         PS3.setLedOn(LED1);
         setStep(stepSize[0]);
     break;
    case 2:
         PS3.setLedOn(LED2);
         setStep(stepSize[1]);
         break;
    case 3:
         PS3.setLedOn(LED3);
         setStep(stepSize[2]);
         break;
    case 4:
         PS3.setLedOn(LED4);
         setStep(stepSize[3]);
         break;
       }
}//End setPSLEDS()








//------------------------------------------------------------------------------------------------
 /* fscale
 Floating Point Autoscale Function V0.1
 Paul Badger 2007
 Modified from code by Greg Shakar

 This function will scale one set of floating point numbers (range) to another set of floating 
 point numbers (range). It has a "curve" parameter so that it can be made to favor either the
 end of the output. (Logarithmic mapping). It takes 6 inputs

 originalMin - the minimum value of the original range - this MUST be less than origninalMax

 originalMax - the maximum value of the original range - this MUST be greater than orginalMin

 newBegin - the end of the new range which maps to orginalMin - it can be smaller, or larger, 
            than newEnd, to facilitate inverting the ranges
 newEnd - the end of the new range which maps to originalMax  - it can be larger, or smaller, 
          than newBegin, to facilitate inverting the ranges

 inputValue - the variable for input that will mapped to the given ranges, this variable is 
              constrained to originaMin <= inputValue <= originalMax
 curve - curve is the curve which can be made to favor either end of the output scale in the 
         mapping. Parameters are from -10 to 10 with 0 being a linear mapping (which basically 
        takes curve out of the equation)
*/  

float fscale( float originalMin, float originalMax, float newBegin, float newEnd, float inputValue, float curve){

  float OriginalRange = 0;
  float NewRange = 0;
  float zeroRefCurVal = 0;
  float normalizedCurVal = 0;
  float rangedValue = 0;
  boolean invFlag = 0;


  // condition curve parameter
  // limit range
  if (curve > 10) curve = 10;
  if (curve < -10) curve = -10;

  curve = (curve * -.1) ; // - invert and scale (more intuitive) - postive numbers give more weight to high end on output
  curve = pow(10, curve); // convert linear scale into lograthimic exponent for other pow function
 
  // Check for out of range inputValues
  if (inputValue < originalMin) {
    inputValue = originalMin;
  }
  if (inputValue > originalMax) {
    inputValue = originalMax;
  }

  // Zero Refference the values
  OriginalRange = originalMax - originalMin;

  if (newEnd > newBegin){
    NewRange = newEnd - newBegin;
  }
  else
  {
    NewRange = newBegin - newEnd;
    invFlag = 1;
  }

  zeroRefCurVal = inputValue - originalMin;
  normalizedCurVal  =  zeroRefCurVal / OriginalRange;   // normalize to 0 - 1 float

  // Check for originalMin > originalMax  - the math for all other cases i.e. negative numbers seems to work out fine
  if (originalMin > originalMax ) {
    return 0;
  }

  if (invFlag == 0){
    rangedValue =  (pow(normalizedCurVal, curve) * NewRange) + newBegin;

  }
  else     // invert the ranges
  {  
    rangedValue =  newBegin - (pow(normalizedCurVal, curve) * NewRange);
  }

  return rangedValue;
} //End fscale

