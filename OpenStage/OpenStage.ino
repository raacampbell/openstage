
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






