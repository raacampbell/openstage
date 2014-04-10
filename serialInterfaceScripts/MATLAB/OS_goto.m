function OS_goto(coords,motionType,nAxes)
% Go To an X,Y, and Z position with OpenStage
%
% function OS_goto(coords,motionType)
%
% Purpose
% Moves stage to a defined absolute or relative position.
%
%
% Inputs 
% coords - Desired position of each axis in microns. All axes must be
%          set. By convention axes are ordered: x,y,z.  
% motionType [optional] - a character 'a' [default] or 'r' for absolute or relative. 
% nAxes [optional] - define the number of axes. 3 by default.
%
% Examples
% OS_goto([0,200.5,0])   %moves to position X: 0, Y: 200.5, Z: 0
% OS_goto([0,0,-10],'r') %relative move of -10 microns in Z
%
%
% Notes
% - Signed values are possible and influence direction of motion. The 
% way the system is wired determines the motion direction.
% - coords are transmitted over the serial line as
% integers (not binary). The three least significant digits are after the decimal
% point. This function takes care of this automatically.  So if we want the stage to 
% move 1.5 microns, this routine submits 1500 to the controller. However, the user
% supplies the value "1.5"
% - The function induces a blocking pause until motion is complete. 
%
% Rob Campbell - CSHL, August 2013

global OS;
if isempty(OS), OS=connectOpenStage; end

if nargin<2
    motionType='a';
end

if nargin<3
  nAxes=3;
end


coords=round(coords*1.0E3); 

AX=repmat('%d,',nAxes);
AX(end)=[];
str=sprintf(['g%s',AX], motionType, coords);
fwrite(OS,str)
    


%Now block and wait for terminator
while get(OS,'BytesAvailable')==0
end
fscanf(OS);
