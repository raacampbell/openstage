# OpenStageSerial.py
# makes serial connection to OpenStage

import serial
import sys

#change serial port number in following line to that you are using 
ser=serial.Serial(3, baudrate=115200, timeout=2)

def readAndPrintBuffer():
	print('\n' + readBuffer() + '\n')
	

def readBuffer():
	c=ser.read();
	thisStr='';
	while c is not '$':
	    thisStr += c
	    c=ser.read()

	return thisStr
