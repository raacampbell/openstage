# Obtain current location from OpenStage
# 
# Purpose
# Obtain stage location on each axis and display on screen
#
# Rob Campbell - CSHL, December 2013

from OpenStageSerial import ser, readAndPrintBuffer


ser.flushInput() #flush input buffer 

ser.write('p') #Issue command 
readAndPrintBuffer()
ser.close()

