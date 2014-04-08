These Python functions are designed to be called from the command line (e.g. Linux or OS X terminal or Windows command line). 

e.g. From Windows you might do:
python.exe OS_beep.py 

That will make the stage emit a beep. 

Functions that return an output do so by printing to screen. 

None of this is terribly Pythonic, of course. If you want to integrate OpenStage serial commands into a Python application, you might wish to coalesce these functions into an object.

** You will need to modify the serial port number in OpenStageSerial.py **