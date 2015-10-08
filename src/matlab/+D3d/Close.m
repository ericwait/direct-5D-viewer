function Close()
global d3dtimer D3dUICtrlHandles

if (~isempty(D3dUICtrlHandles))
    close(D3dUICtrlHandles.hObject);
    clear D3dUICtrlHandles
end

D3d.Viewer('close');

stop(d3dtimer);
delete(d3dtimer);

clear mex

end
