function Close()
global d3dtimer D3dUICtrlHandles D3dIsOpen

if (~isempty(D3dUICtrlHandles))
    if (isvalid(D3dUICtrlHandles.hObject))
        delete(D3dUICtrlHandles.hObject);
    end
end

D3d.Viewer.Close();

D3d.Messaging.DeleteTimer();

D3dUICtrlHandles = [];
d3dtimer = [];

clear mex

D3dIsOpen = false;
end
