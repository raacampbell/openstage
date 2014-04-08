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



if nargin==0
    DEV='COM4'; %Might want to modify this to your default port
end

delete(instrfind({'Port'},{DEV}))

OS=serial(DEV,...
    'TimeOut',2,...
    'BaudRate',115200,...
    'Terminator','$');


fopen(OS);
