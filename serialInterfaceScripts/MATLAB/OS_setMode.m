function OS_setMode(m)
% Set Speed Mode on OpenStage 
%
% function OS_setMode(m)
%
% Purpose
% Sets the Speed Moce for analog stick and D-pad motions on the OpenStage. 
% This does not affect serial commands for Go To motions or return 
% motions to locations stored on the right hand buttons.
%
%
% Rob Campbell - CSHL, August 2013


global OS;
if isempty(OS), OS=connectOpenStage; end


if m>4 || m<1
    fprintf('mode out of range\n');
    return
end

str=sprintf('m%d\n',m);
fwrite(OS,str)
    
       
      