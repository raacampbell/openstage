# Collect information about the settings on the stage. Information
#
# Rob Campbell - CSHL, December 2013

from OpenStageSerial import ser, readAndPrintBuffer

ser.flushInput() #flush the buffer, to be safe

ser.write("i")
readAndPrintBuffer()
ser.close()
