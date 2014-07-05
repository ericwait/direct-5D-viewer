function UpdateTime(time, fromC)
global uiFrameTb uiFrameSlider imageData

if(time < 0)
    time = 0;
elseif(time >= imageData.NumberOfFrames -1 )
    time = imageData.NumberOfFrames  - 1;
end

set(uiFrameTb,'String',num2str(time+1));
set(uiFrameSlider,'Value', time+1);

UpdateTimeIndicatorLine(time);

if (~exist('fromC','var') || isempty(fromC) || fromC~=1)
    lever_3d('setFrame',time);
end
end
