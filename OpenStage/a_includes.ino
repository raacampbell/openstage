
//------------------------------------------------------------------------------------------------
// * Load libraries
//
// Load the libraries necessary to interface with the DualShock3 via the USB Host Shield.
// Can comment out the PS3USB if you also disable the PS3 gamepad in c_userSettings
#include <SPI.h>
#include <PS3USB.h> //see https://github.com/felis/USB_Host_Shield_2.0
#include <AccelStepper.h>  //Control of stepper motors: http://www.airspayce.com/mikem/arduino/AccelStepper/
#include <LiquidCrystal.h> //Write to LCD display (https://bitbucket.org/fmalpartida/new-liquidcrystal/wiki/Home)
#include <Wire.h> //Seems to be needed on some systems
