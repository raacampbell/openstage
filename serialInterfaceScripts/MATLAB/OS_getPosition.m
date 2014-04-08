function varargout=OS_getPosition
% Obtain current location from OpenStage
% 
% function pos=OS_getPosition
%
% Purpose
% Obtain stage location on each axis and either display on screen
% or collect as a vector.
%
% Outputs
% pos [optional] - a vector with the same length as the number of axes
% on the stage. By convention, axes are ordered: X,Y,Z. If pos is not
% assigned the command simply displays the current stage on the screen.
%
% Examples
% OS_getPosition %Displays stage location to screen
% pos=OS_getPosition; %Assigns location to pos and supresses printing to screen
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
fwrite(OS,'p')

%Read response
str=fscanf(OS);

%Convert to a vector 
numAxes=length(strfind(str,','))+1;
scanString=repmat('%f,',1,numAxes);
scanString(end)=[];
pos=textscan(str,[scanString,'$']);

pos=cell2mat(pos);


%Output or display to screen as necessary
if nargout==0
    disp(pos)
end

if nargout>0
    varargout{1}=pos;
end
