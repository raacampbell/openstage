# Purpose
# Set or read the acceleration parameter of each axis. If no
# inputs are provided, prints values to screen. If three inputs 
# are provided, the function uses these to set the acceleration 
# parameters on the stage. Parameters relate to Go To serial 
# motions and right-button motions only. 
#
#
# Inputs
# Three numbers defining the accelerations each axis in steps per s per s. 
# Axes ordered X,Y,Z and all must be provided.
#
# Examples
# OS_moveAccel     #prints axes accelerations to screen
# OS_moveAccel 500 500 1000 #set accelerations in X, Y, and Z
#
#
# Rob Campbell - CSHL, December 2013



import sys
from OpenStageSerial import ser, readAndPrintBuffer, readBuffer

import re

ser.flushInput() #flush the buffer, to be safe


#First read from controller. This is done regardles of what the
#user asked for in order to get the number of axes.
ser.write('ar')
buf=readBuffer()
numAxes=len([m.start() for m in re.finditer(',', buf)]) + 1


#Either report parameters to the screen or change them
if len(sys.argv)<2:
  print(buf)
  sys.exit(0)


if len(sys.argv)<numAxes+1:
  sys.stderr.write("Must provide all axes\n")
  sys.exit(0)



#Build string to send
strToSend='as'

for arg in sys.argv[1:4]:
  arg = int(arg)

  if arg<0:
    sys.stderr.write("Value out of range\n")
    sys.exit(0)

  strToSend += str(arg) + ','

strToSend=strToSend[:-1] + '$'


#Send string
ser.write(strToSend)
ser.close()