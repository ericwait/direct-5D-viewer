function EnableBuffer( bufferNum )
%ENABLEBUFFER1 Summary of this function goes here
%   Detailed explanation goes here
global D3dUICtrlHandles

if (~isempty(D3dUICtrlHandles))
    if (bufferNum==1)
        set(D3dUICtrlHandles.handles.rb_orgImage,'value',1,'enable','on');
        set(D3dUICtrlHandles.handles.rb_Processed,'value',0);
        D3d.Viewer('viewTexture','original');
    elseif (bufferNum==2)
        set(D3dUICtrlHandles.handles.rb_Processed,'value',1,'enable','on');
        set(D3dUICtrlHandles.handles.rb_orgImage,'value',0);
        D3d.Viewer('viewTexture','processed');
    else
        error('There are currently only 2 buffers available!');
    end
end
end

