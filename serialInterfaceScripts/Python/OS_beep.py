# Issue beep from OpenStage controller buzzer
# This function does not accept input or output arguments. 
#
# Rob Campbell - CSHL, December 2013

from OpenStageSerial import ser

ser.write("b")
ser.close()
