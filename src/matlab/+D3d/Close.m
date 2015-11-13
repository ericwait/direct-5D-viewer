function Close()
global d3dtimer D3dUICtrlHandles

if (~isempty(D3dUICtrlHandles))
    if (isvalid(D3dUICtrlHandles.hObject))
        delete(D3dUICtrlHandles.hObject);
    end
end

D3d.Viewer('close');

D3d.Messaging.DeleteTimer();

D3dUICtrlHandles = [];
d3dtimer = [];

clear mex

end
