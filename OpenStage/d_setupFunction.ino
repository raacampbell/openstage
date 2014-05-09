
void setup() {
  bool verbose=0;


  // Connect to the PC's serial port via the Arduino programming USB connection. 
  // This used mainly for printing de-bugging information to the PC's serial terminal
  // during testing. [In future we will add the option for controlling the stage through
  // the USB port]

  Serial.begin(115200); //Open software serial for debugging

  if (doSerialInterface){
    if (!controlViaUSB){
       HARDWARE_SERIAL_PORT.begin(115200);
       SerialComms = &HARDWARE_SERIAL_PORT;

     } else {
       SerialComms = &Serial;
     }
  } //if doSerialInterface
  if (verbose){
     Serial.println(" ");
     Serial.println("Established serial connection");
  }

  //pinMode(13,OUTPUT); //PIN 13 is optionally used for testing and debugging with an osciliscope
  pinMode(beepPin,OUTPUT); //Produce sound on this pin


  #ifdef AXIS_1
   mySteppers[0] = &stepper1;
   axisPresent[0]=1; 
  #endif
  #ifdef AXIS_2
   mySteppers[1] = &stepper2;
   axisPresent[1]=1;
  #endif
  #ifdef AXIS_3
   mySteppers[2] = &stepper3;
   axisPresent[2]=1;
  #endif
  #ifdef AXIS_4
   mySteppers[3] = &stepper4;
   axisPresent[3]=1;
  #endif


  



  //Set the micro-step pins as outputs
  pinMode(MS1,OUTPUT);
  pinMode(MS2,OUTPUT);
  pinMode(MS3,OUTPUT);  


  if (verbose){
    Serial.print("Setting up pins for axes: ");
  }
  //Set up motor control pins, LED pins, etc, as output lines
  for (byte ii=0; ii<numAxes; ii++){
    if (!axisPresent[ii]) //Skip axes that aren't present 
      continue;
    
    pinMode(stepDir[ii],OUTPUT); 

    pinMode(stepOut[ii],OUTPUT); 
    digitalWrite(stepOut[ii],LOW);

    pinMode(enable[ii],OUTPUT);
    digitalWrite(enable[ii],LOW);//power to motors

    pinMode(stageLEDs[ii],OUTPUT);
    digitalWrite(stageLEDs[ii],LOW);

    if (verbose){
      Serial.print(ii);
      Serial.print(" ");
   }
  } 
  if (verbose){
    Serial.println(" ");
  }

  //Initialise the 20 by 4 LCD display 
  #ifdef DO_LCD
   lcd.begin(20,4);               
   lcd.home ();                   
   lcd.clear();
  #endif



  // Connect to the USB Shield
  #ifdef DO_GAMEPAD
    if (verbose){
      Serial.println("USB shield connecting...");
     }
    if (Usb.Init() == -1) {
      if (verbose){
        Serial.print(F("\r\nConnection to USB shield failed"));
      }
    
      //halt and flash warning
      #ifdef DO_LCD
      while(1){ //infinite while loop
       lcd.setCursor (0, 1);   
       lcd.print (" No USB connection!");
       delay(1000);
       lcd.clear();
       delay(1000);
      }// while
      #endif  
    }
   //Pre-calculate the speeds for different hat-stick values. This moves these
   //calculations out of the main loop, and allows for smoother closed-loop hat-stick motions.
   for (byte ii=0; ii<128; ii++){
    for (byte jj=0; jj<4; jj++){         
       //SPEEDMAT[ii][jj]=(ii/127.5)*maxSpeed[jj]; //Plain linear
       SPEEDMAT[ii][jj]=fscale(hatStickThresh, 127.5, 0.04, maxSpeed[jj], ii, curve[jj]); //non-linear mapping
     }
    }

  #endif

  //Display boot message on LCD screen  
  #ifdef DO_LCD
   lcd.setCursor (0,0);   
   lcd.print ("Booting OpenStage");
   lcd.setCursor (0,1);   
  #endif



  //the variable thisStep can take on one of four values for each axis
  //Calculate all of these here
  for (byte ii=0; ii<numAxes; ii++){
    for (byte jj=0; jj<4; jj++){
      if (axisPresent[ii]){
           thisStep[ii][jj] = (fullStep[ii]/360) * stepSize[jj] * gearRatio[ii];
      } //if axisPresent
    } // jj for loop
  } //ii for loop


  #ifdef DO_GAMEPAD
    if (verbose){
        Serial.print("Polling USB");
     }
    // Poll the USB interface a few times. Failing to do this causes the motors to move during 
    // following a reset.
    for (byte ii=1; ii<10; ii++){
      Usb.Task(); 
      delay(100); 
      if (verbose){
        Serial.print(".");
      }
      #ifdef DO_LCD
       lcd.print(".");
      #endif
     } //for loop
    setPSLEDS(); //Set the LEDs on the DualShock to the correct states
    if (verbose){
        Serial.println("Done polling");
    }
  #endif


  //Set default values for the AccelStepper instances
  for (byte ii=0; ii<numAxes; ii++){
    (*mySteppers[ii]).setMaxSpeed(4.0E3); //The max the Arduino Mega is capable of 
    (*mySteppers[ii]).setAcceleration(0); //Must be zero to allow hat stick speed motions
  }


  //An analog stick value of zero likely means that the controller is not connected. 
  //If the game pad is enabled, don't proceed until a contoller is found, or the 
  //stage will move by itself.
  #ifdef DO_GAMEPAD
    while (PS3.getAnalogHat(LeftHatX)==0){
      #ifdef DO_LCD
        lcd.setCursor(0,1);
        lcd.print("Connect DualShock");
        lcd.setCursor(0,2);
        lcd.print("And Re-Boot");
      #endif
      if (verbose){
        Serial.println("Connect PS3 Controller!");
      }
    } //while PS3
  #endif
  
  #ifdef DO_LCD
    lcd.clear();
    lcd.home();
    setupLCD();//Print axis names to LCD
  #endif


  if (verbose){
    Serial.println("Finished setup()");
  }


}//End of setup function 



