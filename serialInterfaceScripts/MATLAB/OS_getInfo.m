function varargout=OS_getInfo
% Obtain info on OpenStage controller state. 
%
% function outStr=OS_getInfo
%
% Purpose
% Collect information about the settings on the stage. Information
% is printed to screen and, optionally, output to a string. 
%
% Outputs
% outStr [optional] - a string containing the text that was printed
%                     to the screen
%
%
% Rob Campbell - CSHL, August 2013

global OS;
if isempty(OS), OS=connectOpenStage; end

%flush buffer if needed
if get(OS,'BytesAvailable')>0
    fread(OS);
end


%Issue command 
fwrite(OS,'i')

%Read respone
str=fscanf(OS);
str(end)=[]; %chop terminator

fprintf(1,'%s\n',str);



if nargout>0
	varargout{1}=str;
end
