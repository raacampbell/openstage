
//------------------------------------------------------------------------------------------------
// * moveToTarget
// This function is responsible for moving the stage to an absolute location. It takes two 
// inputs: an array of floats defining how much to move the stage in each axis and the step size. 
// The length of the array should be equal to the number of axes. The step size indicates the 
// fraction of a full step being taken and is a float. 
//
// **  You will need to change the speed and step sizes in this file for your motors **
void moveToTarget(float target[]){
  
 byte verbose=0;
 int ii; //for loop counter
 float intendedMove[numAxes]; //How far to move on each axis. 
 setStep(moveToStepSize);
 
 
  
  #ifdef DO_GAMEPAD
    PS3.setAllOff(); //Switch off the LEDs to indicate that we're making a non-manual motion
  #endif

  float oneStep[numAxes]; //how far one step takes us
  long stepsToTake[numAxes]; //how many steps to take on each axis


  for (ii=0; ii<numAxes; ii++){
    if (!axisPresent[ii]){
      continue;
    }

    intendedMove[ii]=target[ii]-stagePosition[ii]; //How far we need to move
  
  /*
    //compensate for backlash
    if (intendedMove[ii]<0 && digitalRead(stepDir[ii])==1)
       intendedMove[ii]=intendedMove[ii]-0.32; //the measured backlash in z
    if (intendedMove[ii]>0 && digitalRead(stepDir[ii])==0)
       intendedMove[ii]=intendedMove[ii]+0.32; //the measured backlash in z
  */  
  
    oneStep[ii] = (fullStep[ii]/360.0) * moveToStepSize * gearRatio[ii];
  
    stepsToTake[ii]=round(intendedMove[ii]/oneStep[ii]); //total number of steps we have to take

    if (verbose && !controlViaUSB){
      Serial.print(ii+1);
      Serial.print(". Targ: ");
      Serial.print(target[ii]);
      Serial.print("; Pos: ");
      Serial.print(stagePosition[ii]);
      Serial.print("; Spd: ");
      Serial.print(moveToSpeed[ii]);
      Serial.print("; oneStp: ");
      Serial.print(oneStep[ii]);
      Serial.print("; steps2take: ");
      Serial.print(stepsToTake[ii]);
      Serial.print("; CurPos B4: ");
      Serial.println((*mySteppers[ii]).currentPosition() );
    }
    
    //Now set the maximum pulse speed on this stepper axis
    (*mySteppers[ii]).setSpeed(moveToSpeed[ii]/oneStep[ii]); //seems to be necessary that this is set to a reasonable value
    (*mySteppers[ii]).setMaxSpeed(moveToSpeed[ii]/oneStep[ii]);
    (*mySteppers[ii]).setAcceleration(moveToAccel[ii]);       

      if (disableWhenStationary[ii] && abs(intendedMove[ii])>0)
         digitalWrite(enable[ii],LOW);
  
      //Update the stage position to what it will be once the move is complete
      //No point waiting to do this, since there is no feedback from the stepper
      //anyway.
      updateStagePos(ii,stepsToTake[ii],moveToStepSize); 
      
    
      //Set up axes for motions
      (*mySteppers[ii]).moveTo( (*mySteppers[ii]).currentPosition()+stepsToTake[ii] );
          
  }


  //Now run the steppers until they get there
  if (verbose && !controlViaUSB){
    Serial.println("Moving");
  }

  runSteppersToPos();  


  //Return LEDs and Big Easy Drivers to previously selected step size
  //This also re-calculates the step size (which we re-defined above).
  setPSLEDS();
  
  for (ii=0; ii<numAxes; ii++){
     if (!axisPresent[ii])
       continue;
       
       #ifdef DO_LCD
       lcdStagePos(ii,stagePosition[ii],0);
       #endif

       stepperPreviousPos[ii]=(*mySteppers[ii]).currentPosition();
       (*mySteppers[ii]).setAcceleration(0);
       (*mySteppers[ii]).setSpeed(0);
       
       if (disableWhenStationary[ii] && abs(intendedMove[ii])>0)
         digitalWrite(enable[ii],HIGH);
   
       
    }// End for (ii=0; ii<numAxes; ii++){

}//End moveToTarget()



void runSteppersToPos(){
  bool verbose=0;
  //int n=0; //counter to monitor if the controller is locked in this loop
  bool keepMoving=1;
  bool axisFinished[numAxes];
  
  for (int ii=0; ii<numAxes; ii++){
    axisFinished[ii]=1;
  }
  
  if (verbose){
    Serial.println("Entering while loop in runSteppersToPos()");
  }
  
  while (keepMoving){
    
    keepMoving=0;
    for (int ii=0; ii<numAxes; ii++){
      if (!axisPresent[ii]){
        continue;
      }

      if ((*mySteppers[ii]).distanceToGo() != 0){
         keepMoving=1;
         axisFinished[ii]=0;         
      } 
    }
    
    //Motion is a touch faster if we place run code here
    for (int ii=0; ii<numAxes; ii++){
      if (axisFinished[ii] || !axisPresent[ii]){
        continue;
      }
      (*mySteppers[ii]).run();
    }
    
     
      //I haven't seen lock-ups in ages, so we comment out this code. 
      //Sometimes the controller locks up following analog stick motions
      //This is due to it getting stuck in this loop with the motors at zero
      //speed. Not yet clear why this happens, but in the mean time we can catch
      //it with the following code and deliver an audible indication that something
      //is wrong. This enables us to keep going and not lock up. 
       /*  long currentTotalSpeed=0;
      for (int ii=0; ii<numAxes; ii++){
        if (!axisPresent[ii]){
           continue;
        }
        currentTotalSpeed+=(*mySteppers[ii]).speed();
      }
      
      if (currentTotalSpeed==0){
            n++;
      }
      
      if (n>50){ //If we're stuck here not moving for 50 cycles then something's wrong!
        for (int ii=0; ii<5; ii++){
            serialBeep();
            delay(70);
          }
        break;
      }
      */
   } //close while loop

} //End runSteppersToPos

