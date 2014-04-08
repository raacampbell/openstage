# Set Speed Mode on OpenStage 
#
# e.g. OS_setMode 3
#
# Purpose
# Sets the Speed Mode for analog stick and D-pad motions on the OpenStage. 
# This does not affect serial commands for Go To motions or return 
# motions to locations stored on the right hand buttons.
#
#
# Rob Campbell - CSHL, December 2013

import sys
from OpenStageSerial import ser



#Check input arguments
if len(sys.argv)<2:
	sys.stderr.write("No input arguments provided\n")
	sys.exit(0)

m=sys.argv[1]

if int(m)>4 or int(m)<1:
    sys.stderr.write('mode out of range\n');
    sys.exit(0)


#Send command to OpenStage
ser.write("m" + m)
ser.close()
