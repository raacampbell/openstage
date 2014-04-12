
void loop() {
  //Define main loop static variables
  static bool moving=0; //0 if stationary moving

  // Polling the USB Host Shield to which the PS3 controller is connected takes time, so we don't
  // do it on every pass through the loop. About 30 to 50 times per second is adequate. 
  static unsigned short n=0;          //Counter to implement the periodic polling of the DualShock
  static unsigned short nCycles=3000; //The DualShock is polled on every "nCycles" passses through the loop. 

  //LCD updating is slow, so we do it even more rarely than polling the PS3 controller, and also we 
  //do it in different cycles by making sure it's an odd number. 
  static unsigned short lcdCounter=0;
  static unsigned short lcdCycles=3301; //10k works out as roughly 6 Hz
  static unsigned short lcdAxis=0;      //counter to update axes on different cycles
  static unsigned short lcdAxisTimer=0; //counter to update axes on different cycles


  //Poll DualShock
  #ifdef DO_GAMEPAD
  if (++n == nCycles){
    moving=pollPS3();  
    n=0;
  }
  #endif

  #ifdef DO_LCD
  //Updating the LCD creates some motor choppiness at the faster speeds.
  //With the current parameters, the motor stalls or is not smooth at
  //faster motions if the LCD display is being updated. So no updates 
  //at this speed. 
  if (lcdCounter++ == lcdCycles+lcdAxisTimer) {
    
      if (coarseFine<4 || moving==0){
          lcdStagePos(lcdAxis,stagePosition[lcdAxis],currentSpeed[lcdAxis]);
      }
      
      //update counters   
      lcdAxis++;
      lcdAxisTimer+=500;
      if (lcdAxis==numAxes){
        lcdAxis=0;
        lcdAxisTimer=0;
        lcdCounter=0;
      }
  } //if lcdCounter
  #endif


  #ifdef DO_GAMEPAD
    //Move motors based on hat-stick positions
    for (byte ii=0; ii<numAxes; ii++){
       (*mySteppers[ii]).runSpeed();
    }
  #endif

  //Move based on serial commands 
  #ifdef DO_SERIAL_INTERFACE

    if (SerialComms->available()){
        char ch=SerialComms->read(); //read first character
        if (ch=='g') //Absolute and relative motion
          serialMove(); 
        if (ch=='m') //Set speed mode on DualShock
          serialSetMode();
        if (ch=='z') //Zero stage position counter
          zeroStage();
        if (ch=='p') //Report stage position to PC
          tellPosition();
        if (ch=='s') //Report or change step size 
          serial_moveToStepSize();
        if (ch=='v') //Report or change motion speed
          serial_moveToSpeed();
        if (ch=='a') //Report or change motion acceleration
          serial_moveToAccel();
        if (ch=='i') //Report various settings to PC
          reportInfo();
        if (ch=='b') //Issue beep from controller
          serialBeep();
        
       SerialComms->flush();
    }
  #endif

}//End loop()


