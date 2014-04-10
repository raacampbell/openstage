
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
