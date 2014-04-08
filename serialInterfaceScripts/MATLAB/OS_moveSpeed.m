function varargout=OS_moveSpeed(mSpeed)
% Set or read axes speed parameters on OpenStage
%
% function speeds=OS_moveSpeed(mSpeed)
%
%
% Purpose
% Set or read the speed parameter of each axis. If no
% inputs or outputs are provided, prints values to screen.
% If an output is provided, values are assigned to this as a
% vector. If mSpeed is provided, the function sets the 
% speed parameters on the stage. Parameters relate to
% Go To serial motions and right-button motions only. 
%
%
% Inputs
% mSpeed - [optional] a vector defining the speed in microns 
%          per second for each axis. Axes ordered X,Y,Z by
%          default. All must be provided.
%
% Outputs
% speeds - [optional] a vector listing the speeds of each 
%          axis in microns per second. 
%
%
% Examples
% OS_moveSpeed     %prints axes max speeds to screen
% sp=OS_moveSpeed; %returns axes max speeds as variable sp
% OS_moveSpeed([500,500,1000]) %set speeds in X, Y, and Z
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
fwrite(OS,'vr')
str=fscanf(OS);

numAxes=length(strfind(str,','))+1;
scanString=repmat('%f,',1,numAxes);
scanString(end)=[];

speeds=textscan(str,[scanString,'$']);
speeds=cell2mat(speeds);


%Handle parameter change request
if nargin==1
  if any(mSpeed<0)
      fprintf('one or more mSpeed values out of range\n')
      return
  end    
  if length(mSpeed) ~= numAxes
      fprintf('Expected %d axes but got %d\n',...
          numAxes, length(mSpeed))
      return
  end
  
  scanString=repmat('%d,',1,numAxes);
  scanString(end)=[];
  str=sprintf(['vs',scanString,'$'],mSpeed);
  fwrite(OS,str)
  
end


%Handle outputs
if nargout==0 & nargin==0
    fprintf('%s\n',str(1:end-1))
end

if nargout>0
    varargout{1}=speeds;
end
