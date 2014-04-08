# Purpose 
# Set or read (if no stepSize arg provided) the step sizes (in fractions 
# of a full step) for all axes. All axes share the same step size.
#
# Inputs
# stepSize - [optional] Sets a particular step size for right-button and
#            serial port Go To motions. If stepSize is empty, the command
#            returns the current step size of the controller. This is returned
#            as a fraction of a full step. To define step size, the user 
#            supplies an integer between 1 and 5. These correspond to the f
#            following fractional step sizes:
#            1 - full steps
#            2 - 1/2
#            3 - 1/4
#            4 - 1/8
#            5 - 1/16
#
#
# Examples
# OS_stepSize     #print current step size to screen
# OS_stepSize 3   #set step size to 1/4 steps
#
#
# Rob Campbell - CSHL, December 2013


import sys
from OpenStageSerial import ser, readAndPrintBuffer

ser.flushInput() #flush the buffer, to be safe

#Print step size to screen if no inputs provided
if len(sys.argv)<2:
  ser.write('sr')
  readAndPrintBuffer()


#Set step size if an input argument was provided
if len(sys.argv)==2:
  stepSize=int(sys.argv[1])

  if stepSize<1 or stepSize>5:
      sys.stderr.write('stepSize out of range\n')
      sys.exit(0)


  ser.write('ss' + str(stepSize))



ser.close()