# Purpose
# Moves OpenStage to a defined absolute or relative position.
#
#
# Inputs 
# coords - Desired position of each axis in microns. All axes must be
#          set. By convention axes are ordered: x,y,z.  
# motionType [optional]- a character 'a' [default] or 'r' for absolute or relative. 
#
#
# Examples
# OS_goto 0 200.5 0   #moves to position X: 0, Y: 200.5, Z: 0
# OS_goto 0 0 -10 'r' #relative move of -10 microns in Z
#
#
# Notes
# - Signed values are possible and influence direction of motion. The 
# way the system is wired determines the motion direction.
# - coords are transmitted over the serial line as
# integers (not binary). The three least significant digits are after the decimal
# point. This function takes care of this automatically.  So if we want the stage to 
# move 1.5 microns, this routine submits 1500 to the controller. However, the user
# supplies the value "1.5"
# - The function induces a blocking pause until motion is complete. 
#
# Rob Campbell - CSHL, August 2013
# Rob Campbell - CSHL, December 2013

import sys
from OpenStageSerial import ser


#Check input arguments
if len(sys.argv)==1:
	sys.stderr.write("No input arguments provided\n")
	sys.exit(0)

if len(sys.argv)<4:
	sys.stderr.write("Must provide X, Y, and Z\n")
	sys.exit(0)

if len(sys.argv)<5:
	motionType='a'
else:
	motionType=sys.argv[4]

if len(sys.argv)>5:
	sys.stderr.write("Too many inputs provided\n")
	sys.exit(0)


#Build string to send to controller
strToSend='g' + motionType
for arg in sys.argv[1:4]:
	thisNumber=int(round(float(arg)*1000))
	strToSend += str(thisNumber) + ','

strToSend=strToSend[:-1] + '$'


#Send string to OpenStage (initiates motion automatically)
ser.write(strToSend)

#Now block and wait for terminator
while ser.read() is not "$":
	0


ser.close()
