function OS_beep
% Issue beep from OpenStage controller buzzer
%
% function OS_beep
%
%
% This function does not accept input or output arguments. 
%
% Rob Campbell - CSHL, August 2013

global OS;
if isempty(OS), OS=connectOpenStage; end


fwrite(OS,'b')
