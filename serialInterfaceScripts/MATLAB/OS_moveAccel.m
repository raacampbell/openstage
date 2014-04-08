function varargout=OS_moveAccel(mAccel)
% Set or read axes acceleration parameters on OpenStage
%
% function accels=OS_moveAccel(mAccel)
%
%
% Purpose
% Set or read the acceleration parameter of each axis. If no
% inputs or outputs are provided, prints values to screen.
% If an output is provided, values are assigned to this as a
% vector. If mAccel is provided, the function sets the 
% acceleration parameters on the stage. Parameters relate to
% Go To serial motions and right-button motions only. 
%
%
% Inputs
% mAccel - [optional] a vector defining the accelerations of
%          each axis in steps per s per s. Axes ordered X,Y,Z 
%          by default. All must be provided.
%
% Outputs
% accels - [optional] a vector listing the accelerations of each 
%          axis in steps per second per second. 
%
%
% Examples
% OS_moveAccel     %prints axes accelerations to screen
% ac=OS_moveAccel; %returns axes accelerations as variable ac
% OS_moveAccel([500,500,1000]) %set accelerations in X, Y, and Z
%
%
% Rob Campbell - CSHL, August 2013



global OS;
if isempty(OS), OS=connectOpenStage; end

%flush buffer if needed
if get(OS,'BytesAvailable')>0
    fread(OS);
end


%First read from controller. This is done regardles of what the
%user asked for in order to get the number of axes.
fwrite(OS,'ar')
str=fscanf(OS);

numAxes=length(strfind(str,','))+1;
scanString=repmat('%f,',1,numAxes);
scanString(end)=[];

accels=textscan(str,[scanString,'$']);
accels=cell2mat(accels);



%Handle parameter change request
if nargin==1
  if any(mAccel<0)
      fprintf('one or more mAccel values out of range\n')
      return
  end    
  if length(mAccel) ~= numAxes
      fprintf('Expected %d axes but got %d\n',...
          numAxes, length(mAccel))
      return
  end
  
  scanString=repmat('%d,',1,numAxes);
  scanString(end)=[];
  str=sprintf(['as',scanString,'$'],mAccel);
  fwrite(OS,str)
  
end

%Handle outputs
if nargout==0 & nargin==0
    fprintf('%s\n',str(1:end-1))
end

if nargout>0
    varargout{1}=accels;
end
