function EnableBuffer( bufferNum )
%ENABLEBUFFER1 Summary of this function goes here
%   Detailed explanation goes here
global D3dUICtrlHandles

if (~isempty(D3dUICtrlHandles))
    if (bufferNum==1)
        set(D3dUICtrlHandles.handles.uipanel7.Children,'value','1');
        D3d.Viewer.ShowTexture('original');
    elseif (bufferNum==2)
        set(D3dUICtrlHandles.handles.uipanel7.Children,'value','2','enable','on');
        D3d.Viewer.ShowTexture('processed');
    else
        error('There are currently only 2 buffers available!');
    end
end
end

