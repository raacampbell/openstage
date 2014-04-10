//------------------------------------------------------------------------------------------------
// This file contains functions necessary for controlling OpenStage via the serial port


//------------------------------------------------------------------------------------------------
//Based on code from The Arduino Cookbook, M. Margolis, recipe 4.5
//read CSV from serial port and store in the global array values
//ADD ERROR CHECKING IF TOO MANY OR TOO FEW VALUES ARE SENT
int readSerialCSV(){
  int fieldIndex = 0;  // the current field being received
  int sSign[numAxes]={1,1,1}; //sign 

  char ch;
  while (ch != '$'){ //read until the terminator ($) is reached
     if (Serial1.available()){
         ch = Serial1.read();
      }else{
         continue;
      }

    // is this an ASCII digit between 0 and 9?
    if(ch >= '0' && ch <= '9'){
      // yes, accumulate the value if the fieldIndex is within range
      // additional fields are not stored
      if(fieldIndex < numAxes)
        values[fieldIndex] = (values[fieldIndex] * 10) + (ch - '0'); 
      } else if (ch == '-') { //deal with negative numbers
        if(fieldIndex < numAxes)
          sSign[fieldIndex] = -1;        
      } else if (ch == ',') {  // comma is our separator, so move on to the next field
         fieldIndex++;   // increment field index 
     }
   }
   
   //set the signs
   for (byte ii=0; ii<fieldIndex+1; ii++)
      values[ii]=values[ii]*sSign[ii];
     
   return fieldIndex;
}

//------------------------------------------------------------------------------------------------
//Make a relative or absolute motion 
void serialMove()
{
   //Determine if this will be an absolute or relative motion
   while(Serial1.available() == 0 );//block until char arrives
   char moveType=Serial1.read();
    
   //Define variables
   int doneReading=0; //1 when all axis fields have been read


   float serialTarget[numAxes]; //Where we will be going to in microns
   
   int fieldIndex=readSerialCSV();

   //Determine where to move [**IS THIS MIN STUFF RIGHT?**]
   for (byte ii=0; ii<min(numAxes, fieldIndex+1); ii++){
      if (moveType=='r')
          serialTarget[ii]=stagePosition[ii]+values[ii]/1.0E3;
        if (moveType=='a') 
          serialTarget[ii]=values[ii]/1.0E3; //because least sig digits are after decimal point
          values[ii] = 0; // set the values to zero, ready for the next message
     }

    moveToTarget(serialTarget);
    Serial1.print('$'); //send a dollar, which is the terminator
 
} //End serialRelativeMove()



//------------------------------------------------------------------------------------------------
// * serialSetMode
// Set speed mode on the DualShock
void serialSetMode(){
  if (!doGamePad){
    return;
  }

  while(Serial1.available() == 0 );//block until char arrives

  char ch = Serial1.read();
      
   if (ch=='1')
     coarseFine=1;
   if (ch=='2')
     coarseFine=2;
   if (ch=='3')
     coarseFine=3;
   if (ch=='4')
     coarseFine=4;
     
   setPSLEDS();
} //End serialSetMode()


//------------------------------------------------------------------------------------------------
// * zero stage
// Current position becomes the new zero. 
// Also updates any stored locations so that we can still returns to them
void zeroStage(){
  float delta[numAxes]; //difference between current position and previous zero
  for (byte ii=0; ii<numAxes; ii++){
    delta[ii]=stagePosition[ii];
    stagePosition[ii]=0;
    lcdStagePos(ii,stagePosition[ii],0);
  }  
  
  for (byte ii=0; ii<4; ii++){
    if (locationStored==0)
       continue;       
    for (byte jj; jj<numAxes; jj++)
      buttonStageLocations[jj][ii]=buttonStageLocations[jj][ii]-delta[ii];
  }
   
     
} //End zeroStage()


//------------------------------------------------------------------------------------------------
// * tell position
// Report the current stage position via the serial port
void tellPosition(){

  for (byte ii=0; ii<numAxes; ii++){
    Serial1.print(stagePosition[ii]);
    if (ii<numAxes-1)
      Serial1.print(",");
  }
  
  Serial1.print("$");
     
} //End tell position


//------------------------------------------------------------------------------------------------
// * set or report the moveToStepSize
// "ss" sets the step size and "sr" reports it
void serial_moveToStepSize(){
  while(Serial1.available() == 0 ){}//block until char arrives
  char ch = Serial1.read();  

  if (ch=='r'){
    Serial1.print(moveToStepSize);
    Serial1.print("$");
  }
  if (ch=='s'){
    while(Serial1.available() == 0 ); //block until char arrives
    char ch = Serial1.read();  
    
    if (ch=='1')
      moveToStepSize=1.0/1.0;
    if (ch=='2')
      moveToStepSize=1.0/2.0;
    if (ch=='3')
      moveToStepSize=1.0/4.0;
    if (ch=='4')
      moveToStepSize=1.0/8.0;
    if (ch=='5')
      moveToStepSize=1.0/16.0;
  }
} //End serial_moveToStepSize()


//------------------------------------------------------------------------------------------------
// * set or report the moveTo speeds (microns per second)
// "vs" sets the step size and "vr" reports it
void serial_moveToSpeed(){
  while(Serial1.available() == 0 ); //block until char arrives
  char ch = Serial1.read();  

  if (ch=='r'){
   for (byte ii=0; ii<numAxes; ii++){
    Serial1.print(moveToSpeed[ii]);
    if (ii<numAxes-1)
      Serial1.print(",");
    }
      Serial1.print("$");
   }
 
  if (ch=='s'){
    readSerialCSV();
    for (byte ii=0; ii<numAxes; ii++){
      moveToSpeed[ii]=values[ii];
      values[ii] = 0; // zero to ready for next message
     }
  }
} //End serial_moveToSpeed


//------------------------------------------------------------------------------------------------
// * set or report the moveTo accelerations (steps / s^2)
// "as" sets the step size and "ar" reports it
void serial_moveToAccel(){
  while(Serial1.available() == 0 ); //block until char arrives
  char ch = Serial1.read();  

  if (ch=='r'){
   for (byte ii=0; ii<numAxes; ii++){
    Serial1.print(moveToAccel[ii]);
    if (ii<numAxes-1)
      Serial1.print(",");
    }
      Serial1.print("$");
   }
 
  if (ch=='s'){
    readSerialCSV();
    for (byte ii=0; ii<numAxes; ii++){
      moveToAccel[ii]=values[ii];
      values[ii] = 0; // zero to ready for next message
     }
  }
} //End serial_moveToAccel



//------------------------------------------------------------------------------------------------
// Reports a bunch of useful information to the serial port as text
void reportInfo(){
  byte ii;

  Serial1.print("Gear Ratios: ");
  for (ii=0; ii<numAxes; ii++){
    Serial1.print(gearRatio[ii]);
    if (ii<numAxes-1)
       Serial1.print(",");
    }
    Serial1.print('\n');

  Serial1.print("Full Steps Per Rev: ");
  for (ii=0; ii<numAxes; ii++){
    Serial1.print(int(1/(fullStep[ii]/360.0)));
    if (ii<numAxes-1)
       Serial1.print(",");
   }
    Serial1.print('\n');

  Serial1.print("Hat Stick max speeds: ");
  for (ii=0; ii<4; ii++){
    Serial1.print(maxSpeed[ii]);
    if (ii<3)
       Serial1.print(",");
  }
  Serial1.print('\n');
  Serial1.print('\n');
  
  Serial1.print("moveTo() speeds:");
  Serial1.print('\n');
  for (ii=0; ii<numAxes; ii++){
    Serial1.print(ii+1);
    Serial1.print(". Speed [um/s]: ");
    Serial1.print(moveToSpeed[ii]);
    Serial1.print(".  RPM: ");
    Serial1.print(int(60 * (moveToSpeed[ii]/(gearRatio[ii]*1.0))));
    Serial1.print(";  min step [um]: ");
    Serial1.print(fullStep[ii]/360.0*gearRatio[ii]*moveToStepSize);
    Serial1.print(";  pulse rate [Hz]: ");
    Serial1.print(short(moveToSpeed[ii] / ((fullStep[ii]/360) * moveToStepSize * gearRatio[ii])));
    if (ii<numAxes-1)
      Serial1.print('\n');
  }
  
  Serial1.print("$");
} //End reportInfo




//------------------------------------------------------------------------------------------------
// Reports a bunch of useful information to the serial port as text
void serialBeep(){
  beep(5500,0.12);
} //End serialBeep

