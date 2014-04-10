
/*

  OpenStage microscope controller software for Arduino Mega 2560

  Copyright (C) 2013 Robert AA Campbell 
  Cold Spring Harbor Laboratory
  July 2013
  rob@raacampbell.com

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.



* Citing
  This work has been published under the title: "Openstage: A Low-Cost 
  Motorized Microscope Stage with Sub-Micron Positioning Accuracy" by
  Campbell, Eifert, and Turner in PLoS ONE, 2014, DOI: 10.1371/journal.pone.0088977
  http://www.plosone.org/article/info%3Adoi%2F10.1371%2Fjournal.pone.0088977
  Please cite us if you publish using our methods. 
  

* Purpose
  OpenStage is a sub-micron positioning system for microscope stages. 
  It drives stepper motors coupled to micrometers that push translation stages. 
  It has been tested with three axes, but in theory can drive more. 


* Necessary Hardware
  - The microcontroller: the easiest way to get up and running is to use an Arduino 
  Mega 2560. This unit has enough IO pins to handle multiple axes, an LCD display, 
  etc. An Arduino Due is not suitable since it is not currently compatible with the 
  USB Host Shield we use to read the PS3 gamepad, which is our input device. The same 
  goes for ChipKit boards. So unless you want to write your own game-pad driver 
  (contact me if you do), you're stuck with the Mega. 

  - Input device: A PlayStation DualShock 3 controller that plugs into a USB 
  Host Shield. I used the SparkFun version of the USB shield. If you don't stack the
  shield you will have access to more of the Mega's pins. To wire the devices in this
  way you will need to do the following:
    a. Connect pin 7 to reset on the shield.
    b. Connect reset on the shield to reset on the Mega.
    c. Connect GND and VIN on the shield to GND and VIN on the Mega.
    d. Connect pin 13 on the shield to pin 52 on the Mega.
    e. Connect pin 12 on the shield to pin 50 on the Mega.
    f. Connect pin 11 on the shield to pin 51 on the Mega.
    g. Connect pin 10 on the shield to pin 53 on the Mega.
    h. Connect pin 10 on the Mega to pin 53 on the Mega

  The libraries to interface with the DualShock are found here: 
  http://www.circuitsathome.com/arduino_usb_host_shield_projects (see link to the 
  GitHub repository). You will note that there are drivers for XBox and Wii 
  controllers also. However, the code below assumes a DualShock. You can plug the
  controller directly into the USB shield with a mini USB cable or you can use 
  Bluetooth if you can find a compatible dongle. 

  - Per axis hardware: 
      a. one Big Easy Driver (or similar, see Sparkfun). The motors have two pairs of 
        leads called A and B. The letters A and B are also marked on the Easy Driver. 
        Inverting the connections of the motor to the driver (i.e. connecting the A 
        lead pair to B and vice versa) will invert the direction of rotation of the 
        motor. Connect as required to get the stage moving in the correct direction 
        according to button presses. The micro-controller software knows only stage
        direction, not how it maps onto left and right. You determine that according
        to how the motors are wired to the controllers. The micro-stepping pins on all
        axes are wired together. 
      b. one micrometer. We use regular Thor 50 TPI for X and Y and a Newport
        HR-13 (100 TPI) for Z. Obviously a translation stage is needed too. 
        Choose a stage from Newport or Thor according your needs. 
      c. one flexible coupler. (https://sdp-si.com/eStore/Catalog/Group/484)
        Choose operating torque over about 6 lb-in to minimise back-lash. 
        We use 8" couplers for X and Y and a 10" coupler for Z. 
      d. one stepper motor. We used Vexta motors sourced from orientalmotor.com
        We use Vexta PK243M-02BA for XY and PK243M-01BA for Z. 
      e. Machined male/female adapters to attach the flexible couplers to the micrometer 
        head and stepper motor shaft (if shaft is not 1/4"). 

  - Misc: You will also need a power supply for the motors (24V), a power supply for the
   Arduino and associated electronics, a 20x4 character LCD display, LEDs, 
   power switches, etc. If you don't want to bother with the display, then you
   can just skip those items and comment out those code lines. Finally, a Sparkfun 
   RS232 Serial Shifter is needed if you want to interface the controller with a PC
   via the serial port.



* Wiring tips: it should be obvious which pins do what from the global definitions, below. 
  Check that the Big Easy Driver output pins behave the way they should before you hook up 
  the hardware to your stage. A cheap stepper motor from Sparkfun can be used to sanity 
  check everything. Check the step size, direction, and enable behavor of the Big Easy
  Driver. Check that the stepper motor revolves the number of times you expect, etc. If not,
  look over the values for step size, gear ratio, etc, in the global definitions. It's 
  also helpful to visualise the motor pulses with an oscilscope, as triggering on the 
  step pulses will provide an exact measure of the pulse rate driving the motor. Again:
  * * Do Not Connect Motors To Your Stage Until You Have Tested The Assembled Controller * *


* Usage instructions:
  The DualShock controls the stage in the following ways:
  1. Left hat-stick for X and Y. Right hat-stick up/down for Z. Right hat-stick left/right
     does not do anything.
  2. D-pad executes single motion steps in X and Y motions. D-pad plus Triangle performs 
     fixed-step motions in Z.
  3. Shoulder buttons (L1 and R1) cycle through four different speed modes. Selected speed
     mode indicated by LEDs on the DualShock. 1 is slow and 4 is fast. See global variables
     below for speed values these select. Speed modes influence hat-stick motions and D-pad
     fixed speed motions. 
  4. Pressing one of the four right-hand buttons stores the current stage location. Storage
     is signalled by two beeps. Double-clicking the button will cause the stage to move to the
     stored location. All four buttons default to the position of the stage on reboot. 


 * Other files included in this distribution:
   - OS_pollPS3.ino contains the function that polls the PS3 controller and initiates moves.
   - OS_moveToTarget.ino contains the function the performs moves following button double-clicks
   - OS_HelperFunctions.ino contains other (more peripheral) functions
   - OS_SerialFunctions.ino contains functions required for the serial port interface
*/



//------------------------------------------------------------------------------------------------
// * Load libraries
//
//PS3 DualShock3 driver libraries
// Load the libraries necessary to interface with the DualShock3 via the USB Host Shield.
#include <PS3USB.h>
USB Usb;
// The PS3USB class is used read the state of the DualShock. You can create an instance
// of this class in one of two ways:
PS3USB PS3(&Usb); // This will just create the instance
// This will also store the bluetooth address - this can be obtained from the dongle when running the sketch
// PS3upStickB PS3(&Usb,0x00,0x15,0x83,0x3D,0x0A,0x57); 


#include <AccelStepper.h>  //Control of stepper motors
#include <LiquidCrystal.h> //Write to LCD display (https://bitbucket.org/fmalpartida/new-liquidcrystal/wiki/Home)
#include <Wire.h> //Seems to be needed on some systems


//------------------------------------------------------------------------------------------------
// * Enable/disable major OpenStage functions 
//
bool doSerialInterface=1; //Set to 1 to communicate with the stage via a PC serial port. 
bool controlViaUSB=0; //Control stage via USB serial (disables verbose messages via this port)
bool doGamePad=1; //Set to 1 to enable PS3 DualShock as an input device
bool doLCD=1; //Set to 1 to enable LCD character display



//------------------------------------------------------------------------------------------------
// * Microscope axis hardware definitions. 
//
//                                   === WARNING ===
//   ******************************************************************************
//  ==> This section must be filled out correctly or you may damage your hardware <==
//   ******************************************************************************
//
//
// The properties of each axis are defined as separate variables (mostly arrays with a length 
// equal to the number of axes). 
//
// The elements in the arrays define the properties of X, Y, and Z in that order. Further 
// axes can be added. A Z-only stage can also be set up by altering the code below. 


// numAxes
const byte numAxes=3; //The number of motorised axes

// axisPresent
// Allows particular axes to be skipped. Useful for testing. 1 means present. 0 means absent.
bool axisPresent[numAxes]={1,1,1}; 

// gearRatio
// Micrometer gear ratios on X,Y,Z in microns per revolution. 
unsigned short gearRatio[numAxes]={635,635,250}; 


// fullStep
// Stepper motor full step size (in degrees) for X,Y,Z. We have tested 1.8 degree and 0.9
// degree step sizes. Motors with finer step sizes are available but driving them in 
// quickly using sub-micron steps is a limiting factor. If you want to get up and running 
// in the shortest time, 0.9 degree motors are suggested. 
float fullStep[numAxes]={0.9,0.9,0.9}; //In degrees

// disableWhenStationary
// bool to tell the system whether or not a motor should be disabled when the stage isn't moving.
// Disabling will reduce noise but can cause the motors to move to the nearest full step when the
// power is switched off. Perhaps it makes make sense to do this in X and Y but not Z. 
bool disableWhenStationary[numAxes]={0,0,0};


//------------------------------------------------------------------------------------------------
// * Speed modes (hat-stick)
//
// There will be four (coarse to fine) speeds which may be selected via the two shoulder buttons 
// (L1 and R1). The currently selected speed will be inidicated by the 4 LEDs on the DualShock.
// 1 is fine and slow and 4 is coarse and fast. Here we define the step size and max speed for those
// 4 speed settings. If you change these, you should verify with an osciloscope and the serial 
// monitor that the right values are being produced. You will also need to verify that you are not
// over-driving you motor and causing it to miss steps. Missing steps means the controller will lose
// its absolute position (there is no feedback from the motors). Note that the selection of speed
// values and step sizes are chosen based on the resulting step frequencies the controller must 
// produce and on resonances the motors might exhibit.   
byte coarseFine=2; //boot up in a fine mode
unsigned short maxSpeed[4]={3.5,25,100,750}; //defined in microns per second


// stepSize
// The microstep sizes for each speed mode.
float stepSize[4]={1/16.0, 1/8.0, 1/4.0, 1/2.0}; //Defined in fractions of a full step


// curve
// Curve is the non-linear mapping value to convert analog stick position to speed. The idea
// is to make the conversion from analog stick value to motor pulse rate logarithmic. 
// 0 is linear. See the fscale function for details. 
float curve[4]={-7,-6,-5,-4};



//------------------------------------------------------------------------------------------------
// * Speed mode (D-pad)
//
// The D-pad will be used for making fixed-distance motions of a high speed. The size of the motions
// depends on the coarseFine setting and is defined by the DPadStep array. There's a trade-off between
// speed and accuracy. For these fast motions, I'd like to be hit about 1000 um/s but the AccelStepper 
// moveTo function can only churn out about 4.3 kHz on an Arduino Mega. At 1/4 steps we get reliable
// motions and about 700 um/s. A 1/4 step gives us a 0.156 micron resolution in Z (0.9 degree stepper 
// and 250 um per rev micrometer). The motors MUST be enabled throughout or they will slip and become
// hopelessly lost. To get increased accuracy *and* higher speeds we would need a faster micro-controller,
// or write our own hardware-timer based routines, or slave comercial controllers.
float DPadStep[4]={3,5,10,50}; 
float DPadStepSize=1/2.0;
// Acceleration in X, Y, and Z
unsigned long DPadAccel[numAxes]={1.0E4, 1.0E4, 1.0E4};



//------------------------------------------------------------------------------------------------
// * moveTo speeds
//
// The moveToTarget() function is executed when the user travels to a right-button set point
// or responds to a serial command. It does this using the settings described below. The minimum
// step size and motor RPM are reported to the serial terminal on bootup.
float moveToStepSize=1.0/2.0;
unsigned int moveToSpeed[numAxes]={1600,1600,1200}; 
unsigned int moveToAccel[numAxes]={1.0E4,1.0E4,1.0E4};


//------------------------------------------------------------------------------------------------
// * Stage Position
// The stage controller unit keeps track of how far the stage has moved along each axis. This
// allows the unit to do handy things such as moving to a pre-defined absolute location. When
// the controller boots up it assumes it is at [0,0,0]. Note that there are no encoders: 
// if you manually move the stage, it loses it's position and needs to be reset.
// **BE CAREFUL** Manually moving the stage and forgetting to reset might cause a 
// dangerous stage motion. 
//
// The absolute positioning abilities of the stage can be accessed most diectly using the right
// hand buttons on the DualShock. Pressing down a button hard for 1 second stores the current
// location of the stage. The stage will return to this location when the button is double-clicked.
// The return motion is a straight line. It's up to the user to ensure that making this motion
// won't cause the stage to hit anything. 
// 
// The following global variables are used by the controller code. They aren't user settings.  

// stagePosition
// The current stage position in microns. 
float stagePosition[numAxes]={0,0,0}; 


// stepperPreviousPos
// The previous position of the steppers. We need to keep track of this in a counter in order to 
// figure out how far we've pushed the stage each pass through the main loop. The stage position 
// will be updated whenever pollPS3 is called. Note that stepperPreviousPos relates to stepper 
// motor counts (which are integers) not the actual stage position (which a float and measured 
// in microns).
long stepperPreviousPos[3]={0,0,0}; 



// thisStep
// How far the last step has pushed the stage in microns. This is used to update the stage postion.
float thisStep[numAxes][4];


// SPEEDMAT
// This matrix stores the motor speed for each hat-stick value at each speed mode. These numbers
// are calculated once in the setup function and never change during execution. Pre-calculating 
// them is a good way of improving performance during hat-stick motions. 
float SPEEDMAT[128][4];

// hatStickThresh
//Ensure we don't get motion for stick values smaller than the following threshold
short hatStickThresh=40; 






//------------------------------------------------------------------------------------------------
// * DualShock controller variables
//
// The inputs to the PlayStation controller modify the following global variables which
// are used to move the stage. These are not user settings. 

// buttonStageLocations
// The buttons will be used to store 4 different stage locations. We store these in a 2-D
// array, where the first dimension is button and the second is axis:
float buttonStageLocations[4][numAxes] = {{0,0,0},
                                          {0,0,0},
                                          {0,0,0},
                                          {0,0,0}};

// locationStored
// zero indicates that no position was stored. As positions are stored we update these to 1. 
// This is important, as it minimises the possibility of the user double-clicking a button 
// unbound to a location as it may make the stage perform a large motion to back to zero.
bool locationStored[4]={0,0,0,0};



//------------------------------------------------------------------------------------------------
// * Motor control DIO lines
//

// stepOut
// One pulse at these pins moves the motor by one step (or one micro-step)
byte stepOut[numAxes]={22,24,26}; //Ordered X, Y, and Z

// stepDir
// These pins tell the Big Easy Driver to which they connect which direction to rotate the motor
byte stepDir[numAxes]={23,25,27}; //Ordered X, Y, and Z

// enable
// If these pins are low, the motor is enabled. If high it's disabled. Disabling might decrease 
// electrical noise but will lead to errors in absolute positioning. 
byte enable[numAxes]={28,29,30}; //Ordered X, Y, and Z

// The microstep pins.
// These pins define the microstep size. The MS pins on all axes are wired together.
byte MS1=45;
byte MS2=47;
byte MS3=49;



//------------------------------------------------------------------------------------------------
// * AccelStepper 
// Set up three AccelStepper instances for managing the three axes of motion. There are are a 
// couple of other ways to issue steps and keep track of the stage position. 
// 1. Software pulse generation and software stage position counters
// 2. Hardware timer pulse generation and hardware counter for step pulses. 
// AccelStepper does a good job and makes the code more compact so we've gone with it.
AccelStepper stepperX(1, stepOut[0], stepDir[0]); 
AccelStepper stepperY(1, stepOut[1], stepDir[1]); 
AccelStepper stepperZ(1, stepOut[2], stepDir[2]); 

//Make an array of pointers to the AccelStepper instances to make loops possible
AccelStepper *mySteppers[3]={&stepperX, &stepperY, &stepperZ};



//------------------------------------------------------------------------------------------------
// * Outputs
// The following are pin definitions of controller outputs. These signal information to the user

//Misc
byte beepPin=9; //This is the pin to which the Piezo buzzer is connected 

// stageLEDs
// LEDs will light when the stage moves or an axis is reset, 
// *NOTE: Updating the stage LEDs for hat-stick motions requires fast digital writes as motor speed
//        is set in a closed-loop based on hat-stick position. So we set the stage LEDs using 
//        direct port writes. The code expects the LEDS to be on the first 3 pins of port C of the
//        Mega. You can use a different port (or even different pins on that port), but then you will
//        have to change the code in pollPS3. Direct port writes on these pins are only implemented 
//        for the closed-loop hat-stick control code. Elsewhere we use conventional Arduino 
//        digitalWrites, as these are adequate. 
//
// PC0: 37 (X axis)
// PC1: 36 (Y axis)
// PC2: 35 (Z axis)
// PC3: 34 (a good idea to reserve this for a 4th axis, such as a PIFOC)
byte stageLEDs[4]={37,36,35,35};



// Define pins for LCD display (http://learn.adafruit.com/character-lcds/wiring-a-character-lcd)
LiquidCrystal lcd(7, 6, 5, 4, 3, 2 );


//------------------------------------------------------------------------------------------------
// * Serial communiciation via serial shifter (Sparkfun) to allow the PC to interface with the
// the stage. We use the serial shifter rather than the virtual serial port because this makes
// debugging easier. 
long values[numAxes]; // array holding values for all the received fields from the seria1 port
                     // see serialMove(). For some reason this must be a global. 





//------------------------------------------------------------------------------------------------
// * setup function
// The setup function is run once when the microcontroller is powered on, reset, or programmed. 
// Opening the serial terminal in the Arduino IDE will cause the controller to re-boot if it's 
// connected to the PC. 
//
// The setup function performs basic operations such as defining the DIO direction of each pin, 
// connecting to a serial device, etc. 
  
HardwareSerial *SerialComms; //Make *SerialComms global
void setup() {

  //Initialise loop counters
  byte ii; 
  byte jj;


  // Connect to the PC's serial port via the Arduino programming USB connection. 
  // This used mainly for printing de-bugging information to the PC's serial terminal
  // during testing. [In future we will add the option for controlling the stage through
  // the USB port]
  Serial.begin(115200);
  if (doSerialInterface){
    if (!controlViaUSB){
       Serial1.begin(115200);
       SerialComms = &Serial1;
     } else {
        SerialComms = &Serial;
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











//------------------------------------------------------------------------------------------------
// * main loop
// This is the microcontroller's main loop, which cycles continuously following successful 
// completion of the setup function.
float currentSpeed[3]; //An array defining the speed of each axis 

void loop() {
  //Define main loop static variables
  static bool moving=0; //0 if stationary moving; 1 more than one axis is moving (this isn't a user setting)

  // Polling the USB Host Shield to which the PS3 controller is connected takes time, so we don't
  // do it on every pass through the loop. About 30 to 50 times per second is adequate. In other words,
  // we read the positions of the analog sticks, the button states, etc, at about these rates.
  static unsigned short n=0;          //Counter to implement the periodic polling of the DualShock
  static unsigned short nCycles=3000; //The DualShock is polled on every "nCycles" passses through the loop. 

  //LCD updating is slow, so we do it even more rarely than polling the PS3 controller, and also we 
  //do it in different cycles by making sure it's an odd number. 
  static unsigned short lcdCounter=0;
  static unsigned short lcdCycles=3301; //10k works out as roughly 6 Hz
  static unsigned short lcdAxis=0;      //counter to update axes on different cycles
  static unsigned short lcdAxisTimer=0; //counter to update axes on different cycles




  //Poll DualShock
  if (doGamePad && ++n == nCycles){
    moving=pollPS3();  
    n=0;
  }

  //Updating the LCD creates some motor choppiness at the faster speeds.
  //With the current parameters, the motor stalls or is not smooth at
  //faster motions if the LCD display is being updated. So no updates 
  //at this speed. 
  if (doLCD && lcdCounter++ == lcdCycles+lcdAxisTimer) {
    
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


  if (doGamePad){
    //Move motors based on hat-stick positions
    for (byte ii=0; ii<numAxes; ii++){
       (*mySteppers[ii]).runSpeed();
    }
  } //if doGamePad


  //Move based on serial commands 
  if (doSerialInterface){
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
  }

}//End loop()


