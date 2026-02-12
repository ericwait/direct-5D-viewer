function Close()
%Close Closes the Direct 5D Viewer and cleans up resources.
%
%   D3d.Close()
%
%   Closes the viewer window, stops the message timer, and clears
%   global handles and flags.
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
