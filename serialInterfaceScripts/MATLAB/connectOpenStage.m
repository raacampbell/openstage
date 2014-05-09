function OS=connectOpenStage(DEV)
% Create OpenStage serial port connection 
%
% function OS=connectOpenStage(DEV)
%
% Purpose
% Make connection to serial port on which OpenStage is listening for inputs
% Set up connection with the appropriate baud rate and terminator character. 
% This function is called automatically by the OS_* commands that interface 
% the stage. The user usually won't have to call this function directly.
%
% Inputes
% DEV [optional] - a string defining the serial port at which to make the 
%                  connection. 
%
% Outputs
% OS - the a serial port object associated with the OpenStage controller unit.
%
%
% Examples
% OS = connectOpenStage;
% OS = connectOpenStage('COM3');
%
%
%
% Rob Campbell - CHSL, August 2013
%
% -KNOWN ISSUES:
% You will likely need to hard-code your serial port for the
% automatic connection to work.



if nargin==0
    if ispc
      DEV='COM4'; %Might want to modify this to your default port
    else
      DEV='/dev/tty.usbmodemfd121';
    end
end

delete(instrfind({'Port'},{DEV}))

OS=serial(DEV,...
    'TimeOut',0.5,...
    'BaudRate',115200,...
    'Terminator','$');


fopen(OS);

Connected = [];
fprintf('Connecting')
warning off

while isempty(Connected)
	fwrite(OS, 'H');
	fprintf('.')
	Connected = fread(OS, 1);
end
warning on
fprintf('\nConnection with OpenStage established at %s\n', DEV)
