function varargout=OS_stepSize(stepSize)
% Set or report step size for automated OpenStage motions   
%
% function sSize=OS_stepSize(stepSize)
%
%
% Purpose 
% Set or read (if no stepSize arg provided) the step sizes (in fractions 
% of a full step) for all axes. All axes share the same step size.
%
% Inputs
% stepSize - [optional] Sets a particular step size for right-button and
%            serial port Go To motions. If stepSize is empty, the command
%            returns the current step size of the controller. This is returned
%            as a fraction of a full step. To define step size, the user 
%            supplies an integer between 1 and 5. These correspond to the f
%            following fractional step sizes:
%            1 - full steps
%            2 - 1/2
%            3 - 1/4
%            4 - 1/8
%            5 - 1/16
%
%
% Examples
% OS_stepSize     %print current step size to screen
% stp=OS_stepSize %store step size in variable stp. Do not display to screen
% OS_stepSize(3)  %set step size to 1/4 steps
%
%
% Rob Campbell - CSHL, August 2013



global OS;
if isempty(OS), OS=connectOpenStage; end

%flush buffer if needed
if get(OS,'BytesAvailable')>0
    fread(OS);
end


%Get step size if no input argument was provided
if nargin==0
  fwrite(OS,'sr')
  str=fscanf(OS);
  sSize=textscan(str,'%f$');
  sSize=cell2mat(sSize);
end

%Set step size if an input argument was provided
if nargin==1
  if stepSize<1 || stepSize>5
      fprintf('stepSize out of range\n')
      return
  end    
  fwrite(OS,['ss',num2str(stepSize)])
end



%Display step size to screen if there are no inputs or outputs defined
if nargin==0 & nargout==0
  disp(sSize)
end

%Output step size if an output was requested
if nargin==0 & nargout>0
    varargout{1}=sSize;
end

%If the user both sets a step size and requests the step size we honour this 
if nargin==1 & nargout>0
    varargout{1}=OS_stepSize;
end
