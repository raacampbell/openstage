
//------------------------------------------------------------------------------------------------
// * pollPS3
//
// This the main workhorse function. It polls the PS3 gamepad and responds to user input. 

int pollPS3(){

  Usb.Task(); //Performs unknown USB magic

  // Counters for right hand buttons. 
  // The right hand buttons are used to store and return to a maximum of 4 different absolute 
  // positions. Storing a position is achieved by a "long press" (squeeze the button hard for
  // 1.5 seconds. Returning is achieved by double-clicking. We need variables to store timings
  // for implementing that behavior. These variables are defined below. The buttons are stored
  // in the order: Triangle, Circle, Cross, Square. So clockwise from 12 o'clock. 
  static unsigned long roundButtonPressTimer[4]={0,0,0,0};
  static unsigned long roundButtonClickTimer[4]={0,0,0,0};

  //Read Hat Sticks and set pulse rates

  // hatVals
  // The current values of the so-called "hat sticks", which are the DualShock's two analog sticks, 
  // are stored in the "hatVals" array.  The stick's position are digitised at an 8 bit resolution. 
  // The home (upright) position has a value of 127, so motion speed in any particular direction 
  // can be defined with a 7 bit resolution at most. 
  short hatVals[numAxes]; //X,Y,Z stick values 

  hatVals[0]=127.5-PS3.getAnalogHat(LeftHatX);
  hatVals[1]=127.5-PS3.getAnalogHat(LeftHatY);
  hatVals[2]=127.5-PS3.getAnalogHat(RightHatY);
  //hatVals[3]=127.5-PS3.getAnalogHat(RightHatX); //FOURTH AXIS

  // stage LEDpins
  //This number will be incremented using bit shifts to allow rapid LED switching
  //http://www.arduino.cc/en/Reference/PortManipulation
  byte stageLEDpins = 0; //Stores which LED pins to flip high 
  bool moving=0;

  for (byte ii=0; ii<numAxes; ii++){
    if (!axisPresent[ii]){
      continue;
    }
    
    //Update stage posision based on how far it's moved since we last entered this function
    long deltaPos = (*mySteppers[ii]).currentPosition() - stepperPreviousPos[ii];
    if (abs(deltaPos)>0){
      updateStagePos(ii,deltaPos,stepSize[coarseFine-1]);
      stepperPreviousPos[ii]=(*mySteppers[ii]).currentPosition(); //reset it
    } 
   


   
    //Set stepper speed based on hat-stick value 
    if (abs(hatVals[ii])>hatStickThresh){
       stageLEDpins += 1<<ii; //bits shift LED pins as needed
       currentSpeed[ii]=SPEEDMAT[abs(hatVals[ii])][coarseFine-1];
       (*mySteppers[ii]).setSpeed( (currentSpeed[ii]/thisStep[ii][coarseFine-1])*sgn(hatVals[ii]) );    
       moving=1;
       if (disableWhenStationary[ii])
          digitalWrite(enable[ii],LOW);
          
      } else {
        currentSpeed[ii]=0;
        (*mySteppers[ii]).setSpeed(0); 
        if (disableWhenStationary[ii])
          digitalWrite(enable[ii],HIGH);    
      } 

     //Write to stageLEDs on Port C
     PORTC &= 240; //zero first 4 pins but leave the rest the same
     PORTC |= stageLEDpins;

  }//End for (byte ii=0; ii<numAxes; ii++), the motion interval setting loop

  // Don't waste time checking buttons if the user is on the hat-stick. No motion
  // happens until we exit this loop. Must exit shoulder button evaluation so user
  // can't change mode whilst motors are moving. If this occurs, the position 
  // calculation will be incorrect.
  if (moving){
    return moving;
  }



  //Set coarse/fine using the shoulder buttons
  if(PS3.getButtonClick(L1)){
    coarseFine++;
    if (coarseFine>4){
      coarseFine=4;
    }
    setPSLEDS();
   }

  if (PS3.getButtonClick(R1)){
     coarseFine--;
     if (coarseFine<1){
      coarseFine=1;
     }
     setPSLEDS();
   }




  //Handle long-presses
  //When the user long-presses we store the current stage position
  //in the slot reserved for that button
  int roundButtonAnalog[4];
  int longPress=1000;
    
  roundButtonAnalog[0]=PS3.getAnalogButton(TRIANGLE); 
  roundButtonAnalog[1]=PS3.getAnalogButton(CIRCLE); 
  roundButtonAnalog[2]=PS3.getAnalogButton(CROSS); 
  roundButtonAnalog[3]=PS3.getAnalogButton(SQUARE); 
    
  for (byte ii=0; ii<4; ii++){
    if (roundButtonAnalog[ii]==255){
      if (millis()-roundButtonPressTimer[ii]>longPress){

        //Store stage location in this button
        Serial.print("Stored: ");
        for (byte jj=0; jj<numAxes; jj++){
          if (axisPresent[jj]==0)
            continue;
          buttonStageLocations[ii][jj]=stagePosition[jj];
          Serial.print(stagePosition[jj]);
          Serial.print(" ");
         }
      
      
      Serial.println(' ');
      beep(7750,0.16);
      beep(500,0.20);
      locationStored[ii]=1; //flag that a location is bound to this button

      roundButtonPressTimer[ii]=millis(); //Reset timer
     }    
    } else {
    roundButtonPressTimer[ii]=millis(); //Set timer
    }
  }//End loop for checking long button presses





  //Handle double-clicks
  //When the user double-clicks, the stage moves to the location 
  //stored on that button
  int clickInterval=500;
  int clickedButton = -1;
  if (PS3.getButtonClick(TRIANGLE))
     clickedButton=0;
  if (PS3.getButtonClick(CIRCLE))
     clickedButton=1;
  if (PS3.getButtonClick(CROSS))
     clickedButton=2;
  if (PS3.getButtonClick(SQUARE))
     clickedButton=3;
    
  if (clickedButton > -1 && locationStored[clickedButton]){
    if (millis()-roundButtonClickTimer[clickedButton] < clickInterval){
      moveToTarget(buttonStageLocations[clickedButton]);
      beep(5500,0.12);
     }
    roundButtonClickTimer[clickedButton]=millis();    
  }



  //Handle the D-Pad: make fixed size (blocking) motions in X, Y, or Z

  //Z motion (up/down plus triangle button)
  if (PS3.getButtonPress(TRIANGLE)){
    if (PS3.getButtonClick(UP))
      dButtonMove(2,+1);
    if (PS3.getButtonClick(DOWN))
    dButtonMove(2,-1);

  } else {

    //X motion
    if (PS3.getButtonClick(UP))
      dButtonMove(1,+1);
    if( PS3.getButtonClick(DOWN))
      dButtonMove(1,-1);

    //Y motion
    if (PS3.getButtonClick(LEFT))
      dButtonMove(0,+1);
    if (PS3.getButtonClick(RIGHT))
      dButtonMove(0,-1);
  }//End (PS3.getButtonPress(TRIANGLE))


  return moving;

}//End pollPS3()
    
