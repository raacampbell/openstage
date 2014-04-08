# Zeros the stage position of the OpenStage
#
# Purpose
# Zeros the position counters on the OpenStage LCD display. This also affects
# the stage position value read back on the serial port. Any locations bound
# To the right-hand buttons (triangle, square, etc) are retained, meaning that
# the stage will seek back to the original stored physical location.
#
#
# Rob Campbell - CSHL, December 2013

from OpenStageSerial import ser

ser.write("z")
ser.close()
