
void setup() {

  //Initialise loop counters
  byte ii; 
  byte jj;


  // Connect to the PC's serial port via the Arduino programming USB connection. 
  // This used mainly for printing de-bugging information to the PC's serial terminal
  // during testing. [In future we will add the option for controlling the stage through
  // the USB port]
  Serial.begin(115200); //This is a bit horrible
  if (doSerialInterface){
    if (!controlViaUSB){
       HARDWARE_SERIAL_PORT.begin(115200);
       SerialComms = &HARDWARE_SERIAL_PORT;
     } else {
        SerialComms = &HARDWARE_SERIAL_PORT;
     }
  } //if doSerialInterface


  //Set the micro-step pins as outputs
  pinMode(MS1,OUTPUT);
  pinMode(MS2,OUTPUT);
  pinMode(MS3,OUTPUT);  


  //Set up motor control pins, LED pins, etc, as output lines
  for (ii=0; ii<numAxes; ii++){
    if (!axisPresent[ii]) //Skip axes that aren't present 
      continue;
    
    pinMode(stepDir[ii],OUTPUT); 

    pinMode(stepOut[ii],OUTPUT); 
    digitalWrite(stepOut[ii],LOW);

    pinMode(enable[ii],OUTPUT);
    digitalWrite(enable[ii],LOW);//power to motors
  } 

  for (ii=0; ii<4; ii++){
    pinMode(stageLEDs[ii],OUTPUT);
    digitalWrite(stageLEDs[ii],LOW);
  }




  //Initialise the 20 by 4 LCD display 
  if (doLCD){
   lcd.begin(20,4);               
   lcd.home ();                   
   lcd.clear();
  }

  // Connect to the USB Shield
  if (doGamePad){
    if (Usb.Init() == -1) {
      Serial.print(F("\r\nConnection to USB shield failed"));
    
      //halt and flash warning
      if (doLCD){
        while(1){ //infinite while loop
           lcd.setCursor (0, 1);   
           lcd.print (" No USB connection!");
           delay(1000);
           lcd.clear();
           delay(1000);
         }// while
      }  
    }//if USB.init 

     //Pre-calculate the speeds for different hat-stick values. This moves these
     //calculations out of the main loop, and allows for smoother closed-loop hat-stick motions.
     for (ii=0; ii<128; ii++){
       for (jj=0; jj<4; jj++){         
          //SPEEDMAT[ii][jj]=(ii/127.5)*maxSpeed[jj]; //Plain linear
          SPEEDMAT[ii][jj]=fscale(hatStickThresh, 127.5, 0.04, maxSpeed[jj], ii, curve[jj]); //non-linear mapping
       }  
     }

  }//if doGamePad
  

  //Display boot message on LCD screen  
  if (doLCD){
   lcd.setCursor (0,0);   
   lcd.print ("Booting OpenStage");
   lcd.setCursor (0,1);   
  }


 
   

  //the variable thisStep can take on one of four values for each axis
  //Calculate all of these here
  for (ii=0; ii<numAxes; ii++){
    for (jj=0; jj<4; jj++){
      if (axisPresent[ii]){
           thisStep[ii][jj] = (fullStep[ii]/360) * stepSize[jj] * gearRatio[ii];
      } //if axisPresent
    } // jj for loop
  } //ii for loop


  if (doGamePad){
    // Poll the USB interface a few times. Failing to do this causes the motors to move during 
    // following a rest. I don't know why the following code fixes this, but it does. 
    for (ii=1; ii<10; ii++){
      Usb.Task(); 
      delay(100); 
      if (doLCD){
        lcd.print(".");
      } //if doLCD
     } //for loop
    setPSLEDS(); //Set the LEDs on the DualShock to the correct states
  } //if doGamePad


  //Set default values for the AccelStepper instances
  for (ii=0; ii<numAxes; ii++){
    (*mySteppers[ii]).setMaxSpeed(4.0E3); //The max the Arduino Mega is capable of 
    (*mySteppers[ii]).setAcceleration(0); //Must be zero to allow hat stick speed motions
  }


  pinMode(13,OUTPUT); //PIN 13 is optionally used for testing and debugging with an osciliscope
  pinMode(beepPin,OUTPUT); //Produce sound on this pin

  //An analog stick value of zero likely means that the controller is not connected. 
  //If the game pad is enabled, don't proceed until a contoller is found, or the 
  //stage will move by itself.
  if (doGamePad){ 
    while (PS3.getAnalogHat(LeftHatX)==0){
      if (doLCD){
        lcd.setCursor(0,1);
        lcd.print("Connect DualShock");
        lcd.setCursor(0,2);
        lcd.print("And Re-Boot");
     } //if doLCD
    } //while PS3
  } //if doGamePad
  
  if (doLCD){
    lcd.clear();
    lcd.home();
    setupLCD();//Print axis names to LCD
  } //if doLCD


}//End of setup function 



